
#pragma once

#include "NetReflectionCommon.h"
#include "NetReflectionNumber.h"
#include "NetReflectionFixedPoint.h"
#include "NetReflectionString.h"
#include "NetReflectionEnum.h"
#include "NetReflectionStruct.h"
#include "NetReflectionRandom.h"
#include "NetReflectionOptional.h"
#include "NetReflectionTuple.h"
#include "NetReflectionPtr.h"
#include "NetReflectionPolymorphic.h"
#include "NetReflectionList.h"
#include "NetReflectionMap.h"

// Full list of types
// bool
// standard layout types
// std::string
// std::unique_ptr
// std::shared_ptr
// std::pair
// std::tuple
// NetRangedNumber
// NetFixedPoint
// NetEnum
// NetRandom
// NetOptional
// NetStruct
// NetPolymorphic
// NetList
// NetMap

template <class T, std::size_t MaxVal>
using NetHashMap16 = NetMap<uint16_t, T, MaxVal>;
template <class T, std::size_t MaxVal>
using NetHashMap32 = NetMap<uint32_t, T, MaxVal>;
template <class T, std::size_t MaxVal>
using NetHashMap64 = NetMap<uint64_t, T, MaxVal>;

template <class T, std::size_t MaxVal>
using NetHashMap = NetMap<min_store_t<MaxVal>, T, MaxVal>;
