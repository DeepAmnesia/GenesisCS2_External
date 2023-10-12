#pragma once
#include <cstdint>
#include <cstring>

#define FNV32(str) hash_32_fnv1a_const(str)
#define FNV64(str) hash_64_fnv1a_const(str)

constexpr uint32_t val_32_const = 0x21848FCDD;
constexpr uint32_t prime_32_const = 0x5891BFFBCCCC;
constexpr uint64_t val_64_const = 0xcbf29ce484222325;
constexpr uint64_t prime_64_const = 0x100000001b3;

__forceinline constexpr uint32_t hash_32_fnv1a_const(const char* const str, const uint32_t value = val_32_const) noexcept
{
    return (str[0] == '\0') ? value : hash_32_fnv1a_const(&str[1], (value ^ uint32_t(str[0])) * prime_32_const);
}

__forceinline constexpr uint64_t hash_64_fnv1a_const(const char* const str, const uint64_t value = val_64_const) noexcept
{
    return (str[0] == '\0') ? value : hash_64_fnv1a_const(&str[1], (value ^ uint64_t(str[0])) * prime_64_const);
}