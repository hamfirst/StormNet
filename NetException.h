#pragma once

#if NET_ENABLE_EXCEPTIONS

#include <stdexcept>

#define NET_THROW(...) throw ...
#define NET_THROW_OR(thr, throw_or) thr

#else

#define NET_THROW(...)
#define NET_THROW_OR(thr, throw_or) throw_or

#endif
