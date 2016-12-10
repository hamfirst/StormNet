#pragma once

#include "NetClientBackend.h"
#include "NetClientInterface.h"
#include "NetTransmitterEnet.h"

struct _ENetHost;

class NetClientBackendEnet : public NetClientBackend
{
public:
  NetClientBackendEnet(NetClientInterface * iface, const char * host_addr, uint16_t port);
  ~NetClientBackendEnet();

  NetClientBackendEnet(const NetClientBackendEnet & rhs) = delete;
  NetClientBackendEnet(NetClientBackendEnet && rhs) = delete;

  NetClientBackendEnet & operator = (const NetClientBackendEnet & rhs) = delete;
  NetClientBackendEnet & operator = (NetClientBackendEnet && rhs) = delete;

  virtual void Update() override;
  virtual void Disconnect() override;

private:
  NetTransmitterEnet m_Transmitter;
  _ENetHost * m_Host;

  NetClientInterface * m_Interface;
  bool m_Connected;
};



