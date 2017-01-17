
#include "NetClientBackendEnet.h"
#include "NetBitReaderBuffer.h"

#include <enet/enet.h>

NetClientBackendEnet::NetClientBackendEnet(NetClientInterface * iface, const char * host_addr, uint16_t port) :
  m_Interface(iface)
{
  m_Host = enet_host_create(nullptr, /* create a client host */
    1,
    1,
    57600 / 8 /* 56K modem with 56 Kbps downstream bandwidth */,
    14400 / 8 /* 56K modem with 14 Kbps upstream bandwidth */);

  if (m_Host == nullptr)
  {
    throw std::runtime_error("Could not create host");
  }

  ENetAddress addr;
  enet_address_set_host(&addr, host_addr);
  addr.port = port;

  m_Transmitter.m_Peer = enet_host_connect(m_Host, &addr, 1, 0);
  m_Connected = false;
}

NetClientBackendEnet::~NetClientBackendEnet()
{
  enet_peer_disconnect(m_Transmitter.m_Peer, 0);
  enet_host_flush(m_Host);
  enet_host_destroy(m_Host);
}

void NetClientBackendEnet::Update()
{
  ENetEvent e;

  while (enet_host_service(m_Host, &e, 0))
  {
    switch (e.type)
    {
    case ENET_EVENT_TYPE_CONNECT:
      m_Connected = true;
      m_Interface->GotConnection(&m_Transmitter);
      break;
    case ENET_EVENT_TYPE_RECEIVE:
      if (m_Transmitter.m_Disconnected == false)
      {
        NetBitReaderBuffer reader(e.packet->data, e.packet->dataLength);
        m_Interface->GotMessage(reader);
      }

      enet_packet_destroy(e.packet);
      break;

    case ENET_EVENT_TYPE_DISCONNECT:
      if (m_Connected)
      {
        m_Interface->ConnectionFailed();
      }
      else
      {
        m_Interface->Disconnected();
      }
    }
  }
}

void NetClientBackendEnet::Disconnect()
{
  if (m_Transmitter.m_Disconnected)
  {
    return;
  }

  enet_peer_disconnect(m_Transmitter.m_Peer, 0);
  enet_host_flush(m_Host);
  m_Transmitter.m_Disconnected = true;
}
