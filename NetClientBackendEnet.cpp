
#include "NetClientBackendEnet.h"
#include "NetBitReaderBuffer.h"

#include <enet/enet.h>

NetClientBackendEnet::NetClientBackendEnet(NetClientInterface * iface, const char * host_addr, uint16_t port)
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
      printf("Client connected to %x:%u.\n",
        e.peer->address.host,
        e.peer->address.port);

      m_Connected = true;
      m_Interface->GotConnection(&m_Transmitter);
      break;
    case ENET_EVENT_TYPE_RECEIVE:
      printf("A client packet of length %zu was received on channel %u.\n",
        e.packet->dataLength,
        e.channelID);

      if (m_Transmitter.m_Disconnected == false)
      {
        NetBitReaderBuffer reader(e.packet->data, e.packet->dataLength);
        m_Interface->GotMessage(reader);
      }

      enet_packet_destroy(e.packet);
      break;

    case ENET_EVENT_TYPE_DISCONNECT:

      printf("Client disconnected.\n");

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

  enet_peer_reset(m_Transmitter.m_Peer);
  m_Transmitter.m_Disconnected = true;
}
