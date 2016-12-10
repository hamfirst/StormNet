#pragma once

class NetClientBackend
{
public:
  virtual ~NetClientBackend() = 0;

  virtual void Update() = 0;
  virtual void Disconnect() = 0;
};

