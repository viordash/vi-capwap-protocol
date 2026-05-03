#pragma once

#include "Helpers.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <vector>

// RFC 5416 Section 6.9 - IEEE 802.11 Multi-Domain Capability
//
//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |    Radio ID   |    Reserved   |        First Channel #        |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |       Number of Channels      |       Max Tx Power Level      |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Type:   1032 for IEEE 802.11 Multi-Domain Capability
// Length: 8

struct __attribute__((packed)) MultiDomainCapability : ElementHeader {
  protected:
    // Radio ID: An 8-bit value representing the radio, whose value is between 0 and 31.
    uint8_t radio_id;

    // Reserved: Set to zero
    uint8_t reserved;

    // First Channel #: The lowest channel number in the sub-band
    NetworkU16 first_channel;

    // Number of Channels: Total number of channels allowed in the sub-band
    NetworkU16 number_of_channels;

    // Max Tx Power Level: Maximum transmit power in dBm allowed in the sub-band
    NetworkU16 max_tx_power_level;

  public:
    MultiDomainCapability(const MultiDomainCapability &) = default;
    MultiDomainCapability(uint8_t radio_id,
                          uint16_t first_channel,
                          uint16_t number_of_channels,
                          uint16_t max_tx_power_level);

    uint8_t GetRadioID() const;
    uint16_t GetFirstChannel() const;
    uint16_t GetNumberOfChannels() const;
    uint16_t GetMaxTxPowerLevel() const;

    bool Validate() const;
};

struct WritableMultiDomainCapabilityArray {
  protected:
    std::vector<MultiDomainCapability> items;

  public:
    WritableMultiDomainCapabilityArray(const WritableMultiDomainCapabilityArray &) = delete;
    WritableMultiDomainCapabilityArray();

    void Add(MultiDomainCapability element);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const;
    void Log() const;
};

struct ReadableMultiDomainCapabilityArray {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const MultiDomainCapability *, max_count> items;
    size_t count;

  public:
    ReadableMultiDomainCapabilityArray(const ReadableMultiDomainCapabilityArray &) = delete;
    ReadableMultiDomainCapabilityArray();

    bool Deserialize(RawData *raw_data);
    nonstd::span<const MultiDomainCapability *const> Get() const;
    void Log() const;
};
