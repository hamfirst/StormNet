#pragma once

#include <sb/static_sized_array_alloc.h>

#include "NetServerInterface.h"
#include "NetServerBackend.h"
#include "NetProtocol.h"

template <typename ClientDef, typename ServerProtocolDef, typename ClientProtocolDef>
class NetServer : public NetServerInterface
{
public:

  using ProtocolType = typename NetProtocolInfo<ServerProtocolDef>::template AsymmetricProtocolType<ClientProtocolDef>;

  NetServer(std::size_t max_clients, NetServerBackend * backend) :
    m_Clients(max_clients), m_Backend(backend)
  {

  }

  ~NetServer()
  {

  }

  void DisconnectClient(ClientDef * client_def)
  {
    uint32_t connection_id =(uint32_t)m_Clients.GetIndexAt(client_def);
    DisconnectClient(connection_id);
  }

  void DisconnectClient(uint32_t connection_id)
  {
    m_Backend->ForceDisconnect(connection_id);
  }

  virtual void Update()
  {
    m_Backend->Update();
  }

protected:

  virtual bool AllowNewConnection(uint32_t remote_host, uint16_t remote_port)
  {
    return true;
  }

  virtual ClientDef * ConstructClient(void * client_mem, uint32_t connection_id, ProtocolType & proto) = 0;

  virtual void CleanupClient(ClientDef & client, ProtocolType & proto)
  {

  }

  virtual void GotNewConnection(uint32_t connection_id, uint32_t remote_host, uint16_t remote_port, NetTransmitter * transmitter) override
  {
    if (AllowNewConnection(remote_host, remote_port) == false)
    {
      return;
    }

    ClientInfo * slot_ptr = static_cast<ClientInfo *>(m_Clients.AllocateRaw(connection_id));
    
    new (&slot_ptr->m_Protocol) ProtocolType(transmitter);
    ConstructClient(&slot_ptr->m_Client, connection_id, slot_ptr->m_Protocol);
  }

  virtual void ConnectionLost(uint32_t connection_id) override
  {
    ClientInfo * slot_ptr = m_Clients.GetElementForId(connection_id);
    if (slot_ptr == nullptr)
    {
      return;
    }

    CleanupClient(slot_ptr->m_Client, slot_ptr->m_Protocol);
    m_Clients.Free(slot_ptr);
  }

  virtual void GotMessage(uint32_t connection_id, NetBitReader & reader) override
  {
    ClientInfo * ptr = m_Clients.GetElementForId(connection_id);
    if (ptr == nullptr)
    {
      return;
    }

    ptr->m_Protocol.GotMessage(reader);
  }

private:

  struct ClientInfo
  {
    ClientDef m_Client;
    ProtocolType m_Protocol;
  };

  NetServerBackend * m_Backend;
  StaticSizedArrayAlloc<ClientInfo> m_Clients;
};
