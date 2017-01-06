#pragma once

class NetClientBackend
{
public:
  virtual ~NetClientBackend() {};

  virtual void Update() = 0;
  virtual void Disconnect() = 0;
};

