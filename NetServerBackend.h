
#pragma once

#include <cstdint>

class NetServerBackend
{
public:

  virtual ~NetServerBackend() {};

  virtual void Update() = 0;
  virtual void ForceDisconnect(uint32_t connection_id) = 0;
};


