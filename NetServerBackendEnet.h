#pragma once

#include "NetServerBackend.h"
#include "NetServerInterface.h"

#include <sb/static_sized_alloc.h>

#include <unordered_map>
#include <memory>

struct _ENetHost;
struct NetTransmitterEnet;

struct NetServerSettingsEnet
{
  uint16_t Port = 60000;
  uint32_t LocalAddr = 0;

  uint32_t MaxConnections = 256;
  uint32_t MaxIncBandwidth = 0;
  uint32_t MaxOutBandwidth = 0;
};


class NetServerBackendEnet : public NetServerBackend
{
public:
  NetServerBackendEnet(NetServerInterface * iface, const NetServerSettingsEnet & settings);
  ~NetServerBackendEnet();

  NetServerBackendEnet(const NetServerBackendEnet & rhs) = delete;
  NetServerBackendEnet(NetServerBackendEnet && rhs) = delete;

  NetServerBackendEnet & operator = (const NetServerBackendEnet & rhs) = delete;
  NetServerBackendEnet & operator = (NetServerBackendEnet && rhs) = delete;

  void Update() override;
  void ForceDisconnect(uint32_t connection_id) override;

private:
  _ENetHost * m_Host;
  NetServerInterface * m_Interface;

  StaticSizedAlloc<NetTransmitterEnet> m_Transmitters;
};