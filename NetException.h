#pragma once

#define NET_ENABLE_EXCEPTIONS

#ifdef NET_ENABLE_EXCEPTIONS

#include <stdexcept>

#define NET_THROW(...) throw __VA_ARGS__
#define NET_THROW_OR(thr, throw_or) throw thr

#else

#include <cassert>

#define NET_THROW(...) assert(false)
#define NET_THROW_OR(thr, throw_or) throw_or

#endif
