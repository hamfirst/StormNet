<<<<<<< HEAD
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
=======
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
>>>>>>> 82abf53995350c7136a2b761bbb2fb350eacf404
