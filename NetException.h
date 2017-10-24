#pragma once

#if NET_ENABLE_EXCEPTIONS

#include <stdexcept>

#define NET_THROW(...) throw ...
#define NET_THROW_OR(thr, throw_or) throw thr

#else

#include <cassert>

#define NET_THROW(...) assert(false)
#define NET_THROW_OR(thr, throw_or) throw_or

#endif
