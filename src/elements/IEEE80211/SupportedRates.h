#pragma once

#include "Helpers.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <vector>

// RFC 5416 Section 6.17 - IEEE 802.11 Supported Rates
//
//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |    Radio ID   |        Supported Rates...
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Type:   1040 for IEEE 802.11 Supported Rates
// Length:  >= 3

struct __attribute__((packed)) SupportedRates : ElementHeader {
  public:
    // Maximum number of supported rates (IEEE 802.11 limit is typically 8 per IE)
    static const size_t max_rates = 8;
    // Minimum length: RadioID (1) + at least 2 rates
    static const size_t min_rates = 2;

    // Basic Rate flag (bit 7 of each rate byte)
    static const uint8_t RATE_BASIC_FLAG = 0x80;

  protected:
    // Radio ID: An 8-bit value representing the radio, whose value is between 0 and 31.
    uint8_t radio_id;

  public:
    // Supported Rates: A variable-length field containing the supported rates.
    // Each rate is 8 bits: bit 7 = basic rate flag, bits 6-0 = rate in 0.5 Mbps units.
    uint8_t rates[];

    SupportedRates(const SupportedRates &) = default;
    SupportedRates(uint8_t radio_id, uint8_t rates_count);

    uint8_t GetRadioID() const;
    uint8_t GetRatesCount() const;

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static SupportedRates *Deserialize(RawData *raw_data);
};

struct WritableSupportedRatesArray {
  public:
    struct Item {
        nonstd::span<const uint8_t> rates_data;
        SupportedRates header;

        Item(const Item &) = default;
        Item(uint8_t radio_id, nonstd::span<const uint8_t> rates);

        uint8_t GetRadioID() const;
    };

  private:
    std::vector<Item> items;

  public:
    WritableSupportedRatesArray(const WritableSupportedRatesArray &) = delete;
    WritableSupportedRatesArray();

    void Add(Item element);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const;
    void Log() const;
};

struct ReadableSupportedRatesArray {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const SupportedRates *, max_count> items;
    size_t count;

  public:
    ReadableSupportedRatesArray(const ReadableSupportedRatesArray &) = delete;
    ReadableSupportedRatesArray();

    bool Deserialize(RawData *raw_data);
    nonstd::span<const SupportedRates *const> Get() const;
    void Log() const;
};
