#pragma once

#include "Helpers.h"
#include "IElement.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <vector>

// RFC 5416 Section 6.11 - IEEE 802.11 Rate Set
//
//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |    Radio ID   |                 Rate Set...
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Type:   1034 for IEEE 802.11 Rate Set
// Length: >= 3

struct __attribute__((packed)) RateSet : ElementHeader {
  public:
    static const size_t min_rate_set_length = 2;
    static const size_t max_rate_set_length = 8;

  protected:
    // Radio ID: An 8-bit value representing the radio, whose value is between 0 and 31.
    uint8_t radio_id;

  public:
    // Rate Set: The AC generates the Rate Set that the WTP is to include
    // in its Beacon and Probe messages. The length of this field is
    // between 2 and 8 bytes.

    RateSet(const RateSet &) = default;
    RateSet(uint8_t radio_id, uint16_t length);

    uint8_t GetRadioID() const;
    bool Validate() const;
};

struct WritableRateSetArray : IWritableConfigurationStatusResponseOptionalElement,
                              IWritableConfigurationUpdateRequestOptionalElement {
  public:
    struct Item {
        nonstd::span<const uint8_t> data;
        RateSet header;
        Item(const Item &) = default;
        Item(uint8_t radio_id, nonstd::span<const uint8_t> rate_set_data)
            : data{ rate_set_data }, header{ radio_id, (uint16_t)data.size() } {};
    };

  protected:
    std::vector<Item> items;

  public:
    WritableRateSetArray(const WritableRateSetArray &) = delete;
    WritableRateSetArray();

    void Add(uint8_t radio_id, nonstd::span<const uint8_t> rate_set_data);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableRateSetArray : IReadableConfigurationStatusResponseOptionalElement,
                              IReadableConfigurationUpdateRequestOptionalElement {
  public:
    static const size_t max_count = 32;

    struct Item : RateSet {
        uint8_t data[];
        Item(const Item &) = delete;
    };

  protected:
    std::array<const Item *, max_count> items;
    size_t count;

  public:
    ReadableRateSetArray(const ReadableRateSetArray &) = delete;
    ReadableRateSetArray();

    bool Deserialize(RawData *raw_data) override final;
    nonstd::span<const ReadableRateSetArray::Item *const> Get() const;
    void Log() const override final;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};
