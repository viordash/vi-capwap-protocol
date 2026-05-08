#pragma once

#include "Helpers.h"
#include "IElement.h"
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

    SupportedRates(const SupportedRates &) = default;
    SupportedRates(uint8_t radio_id, uint16_t length);

    uint8_t GetRadioID() const;
    uint8_t GetRatesCount() const;

    bool Validate() const;
};

struct WritableSupportedRatesArray : IWritableConfigurationStatusRequestOptionalElement,
                                     IWritableConfigurationStatusResponseOptionalElement {
  public:
    struct Item {
        nonstd::span<const uint8_t> data;
        SupportedRates header;

        Item(const Item &) = default;
        Item(uint8_t radio_id, nonstd::span<const uint8_t> rates)
            : data(rates), header(radio_id, (uint16_t)data.size()) {
        }
    };

  private:
    std::vector<Item> items;

  public:
    WritableSupportedRatesArray(const WritableSupportedRatesArray &) = delete;
    WritableSupportedRatesArray();

    void Add(Item element);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableSupportedRatesArray : IReadableConfigurationStatusRequestOptionalElement,
                                     IReadableConfigurationStatusResponseOptionalElement {
  public:
    static const size_t max_count = 32;

    struct Item : SupportedRates {
        uint8_t data[];
        Item(const Item &) = delete;
    };

  protected:
    std::array<const Item *, max_count> items;
    size_t count;

  public:
    ReadableSupportedRatesArray(const ReadableSupportedRatesArray &) = delete;
    ReadableSupportedRatesArray();

    bool Deserialize(RawData *raw_data) override final;
    nonstd::span<const ReadableSupportedRatesArray::Item *const> Get() const;
    void Log() const override final;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};
