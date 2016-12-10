
#include "NetServerBackendEnet.h"
#include "NetTransmitterEnet.h"
#include "NetBitReaderVector.h"

#include <stdexcept>
#include <enet/enet.h>


NetServerBackendEnet::NetServerBackendEnet(NetServerInterface * iface, const NetServerSettingsEnet & settings) :
  m_Interface(iface),
  m_Transmitters(settings.MaxConnections)
{
  ENetAddress addr;
  addr.host = settings.LocalAddr;
  addr.port = settings.Port;

  m_Host = enet_host_create(&addr, settings.MaxConnections, 1, settings.MaxIncBandwidth, settings.MaxOutBandwidth);
  if (m_Host == nullptr)
  {
    throw std::runtime_error("Could not create host");
  }
}

NetServerBackendEnet::~NetServerBackendEnet()
{
  enet_host_destroy(m_Host);
}

void NetServerBackendEnet::Update()
{
  ENetEvent e;
  NetTransmitterEnet * trans;

  while (enet_host_service(m_Host, &e, 0))
  {
    switch (e.type)
    {
    case ENET_EVENT_TYPE_CONNECT:
      printf("A new client connected from %x:%u.\n",
        e.peer->address.host,
        e.peer->address.port);

      trans = m_Transmitters.Allocate();
      trans->m_ConnectionId = (uint32_t)m_Transmitters.GetAllocationId(trans);
      trans->m_Peer = e.peer;

      e.peer->data = trans;

      m_Interface->GotNewConnection((uint32_t)trans->m_ConnectionId, e.peer->address.host, e.peer->address.port, trans);
      break;
    case ENET_EVENT_TYPE_RECEIVE:
      trans = (NetTransmitterEnet *)e.peer->data;
      printf("A packet of length %zu was received from %d on channel %u.\n",
        e.packet->dataLength,
        (uint32_t)trans->m_ConnectionId,
        e.channelID);

      if (trans->m_Disconnected == false)
      {
        NetBitReaderBuffer reader(e.packet->data, e.packet->dataLength);
        m_Interface->GotMessage((uint32_t)trans->m_ConnectionId, reader);
      }

      enet_packet_destroy(e.packet);
      break;

    case ENET_EVENT_TYPE_DISCONNECT:

      trans = (NetTransmitterEnet *)e.peer->data;
      printf("%u disconnected.\n", (uint32_t)trans->m_ConnectionId);

      m_Interface->ConnectionLost((uint32_t)trans->m_ConnectionId);

      e.peer->data = NULL;
      m_Transmitters.Free(trans);
    }
  }
}

void NetServerBackendEnet::ForceDisconnect(uint32_t connection_id)
{
  NetTransmitterEnet * trans = m_Transmitters.GetElementForId(connection_id);
  if (trans->m_Disconnected)
  {
    return;
  }

  enet_peer_reset(trans->m_Peer);
  trans->m_Disconnected = true;
}