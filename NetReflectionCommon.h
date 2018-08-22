
#pragma once

#include <sb/cstr.h>
#include <sb/match_const.h>
#include <sb/match_ref.h>

#include <hash/Hash.h>

#include <cstdint>

#include <optional/optional.hpp>

#include <gsl/gsl>

template <typename Type, class NetBitReader>
struct NetDeserializer;


template <typename Type, class NetBitWriter>
struct NetSerializer;
