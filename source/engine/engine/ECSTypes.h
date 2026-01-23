#pragma once
#include <bitset>
#include <cstdint>

// 1. The Entity is just an ID.
using Entity = std::uint32_t;

// 2. Constants
const Entity MAX_ENTITIES = 5000;
const std::uint8_t MAX_COMPONENTS = 32;

// 3. The Signature (The Bitmask)
// A list of T/F booleans to track which components an entity has.
using Signature = std::bitset<MAX_COMPONENTS>;