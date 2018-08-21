#pragma once

#include "NetClientBackend.h"
#include "NetClientInterface.h"
#include "NetProtocol.h"
#include "NetException.h"

#include "optional/optional.hpp"

template <typename ServerProtocolDef, typename ClientProtocolDef>
class NetClient : public NetClientInterface
{
public:

  using ProtocolType = typename NetProtocolInfo<ClientProtocolDef>::template AsymmetricProtocolType<ServerProtocolDef>;

  NetClient(NetClientBackend * backend) :
    m_Backend(backend)
  {

  }

  virtual void Update()
  {
    m_Backend->Update();
  }

  virtual void Disconnect()
  {
    m_Backend->Disconnect();
  }

protected:

  virtual void InitConnection(ProtocolType & protocol)
  {

  }

  virtual void GotConnection(NetTransmitter * transmitter) override
  {
    m_Protocol = ProtocolType(transmitter);
    InitConnection(*m_Protocol);
  }

  virtual void ConnectionFailed() override
  {

  }

  virtual void Disconnected() override
  {

  }

  virtual void GotMessage(NetBitReader & reader, bool ack, int channel_index) override
  {
    m_Protocol->GotMessage(reader, ack, channel_index);
    if (reader.IsEmpty() == false)
    {
      NET_THROW(std::runtime_error("Not all data from packet was consumed"));
    }
  }

  virtual void GotMessage(NetBitReader & reader) override
  {
    m_Protocol->GotMessage(reader);
    if (reader.IsEmpty() == false)
    {
      NET_THROW(std::runtime_error("Not all data from packet was consumed"));
    }
  }

private:
  NetClientBackend * m_Backend;
  std::experimental::optional<ProtocolType> m_Protocol;
};

