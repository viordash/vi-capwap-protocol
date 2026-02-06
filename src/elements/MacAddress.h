#pragma once
#include "ClearHeader.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>

struct WritableMacAddress {
    const nonstd::span<const uint8_t> MACAddress;
    WritableMacAddress(const WritableMacAddress &) = delete;
    WritableMacAddress(const nonstd::span<const uint8_t> &mac_address);
};

struct __attribute__((packed)) ReadableMacAddress {
    static const size_t max_length = 255;
    uint8_t Length;
    uint8_t MACAddresses[];

    bool Validate() const;
};

struct MacAddress {
    uint8_t Length;
    uint8_t Address[std::max(RadioMACAddress::mac_EUI48_size, RadioMACAddress::mac_EUI64_size)];
    MacAddress(const MacAddress &) = default;
    MacAddress(const nonstd::span<const uint8_t> &mac_address);
    MacAddress(const WritableMacAddress &mac_address);
    static void Log(const size_t index, const uint8_t length, const uint8_t *mac_address);

    bool operator==(const MacAddress &other) const;
};