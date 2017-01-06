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
  NetServerSettingsEnet(uint16_t port = 60000, uint32_t local_addr = 0, uint32_t max_connections = 256, uint32_t max_inc_bw = 0, uint32_t max_out_bw = 0) :
    Port(port),
    LocalAddr(local_addr),
    MaxConnections(max_connections),
    MaxIncBandwidth(max_inc_bw),
    MaxOutBandwidth(max_out_bw)
  {

  }

  uint16_t Port;
  uint32_t LocalAddr;

  uint32_t MaxConnections;
  uint32_t MaxIncBandwidth;
  uint32_t MaxOutBandwidth;
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