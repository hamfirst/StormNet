#pragma once

#include <utility>
#include <vector>
#include <type_traits>

#include "NetProtocol.h"
#include "NetMetaUtil.h"

template <typename MessagePipe>
struct NetProtocolPipeInfo
{
  using Pipe = MessagePipe;
};

template <typename ... MessagePipeTypes>
std::vector<NetPipeMode> NetGetProtocolPipeModes(const NetProtocolDefinition<MessagePipeTypes...> & proto_def)
{
  std::vector<NetPipeMode> modes = { NetProtocolPipeInfo<MessagePipeTypes>::Pipe::PipeMode... };
  return modes;
}
