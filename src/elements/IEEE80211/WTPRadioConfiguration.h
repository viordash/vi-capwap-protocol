#pragma once

#include "Helpers.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <vector>

// RFC 5416 Section 6.23 - IEEE 802.11 WTP Radio Configuration
//
//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |    Radio ID   |S Preamble |  Num BSSIDs   |   DTIM Period     |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                              BSSID                            |
// +                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                               |        Beacon Period          |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                         Country String                        |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Type:   1046 for IEEE 802.11 WTP Radio Configuration
// Length:  16

struct __attribute__((packed)) WTPRadioConfiguration : ElementHeader {
  public:
    static const size_t bssid_size = 6;
    static const size_t country_string_size = 4;
    static const size_t min_NumBSSIDs = 1;
    static const size_t max_NumBSSIDs = 16;

    // Radio ID: An 8-bit value representing the radio, whose value is between 0 and 31.
    uint8_t RadioID;

    // Short Preamble: An 8-bit Boolean value set to one (1) for short preamble,
    // or zero (0) for long preamble.
    uint8_t ShortPreamble;

    // Num BSSIDs: An unsigned 8-bit value indicating the number of BSSIDs
    // supported on this radio, and is between 1 and 16.
    uint8_t NumBSSIDs;

    // DTIM Period: An unsigned 8-bit value indicating the DTIM period.
    uint8_t DTIMPeriod;

    // BSSID: A 48-bit value containing the BSSID for the primary BSSID.
    uint8_t BSSID[bssid_size];

    // Beacon Period: An unsigned 16-bit value indicating the beacon period
    // in TUs (1024 microseconds).
    NetworkU16 BeaconPeriod;

    // Country String: ISO 3166-1 country code (3 bytes + 1 reserved).
    // First 2 bytes: ISO 3166-1 alpha-2 country code
    // Third byte: Environment indicator (I=indoor, O=outdoor, space=both)
    // Fourth byte: Reserved (padding)
    uint8_t CountryString[country_string_size];

    WTPRadioConfiguration(const WTPRadioConfiguration &) = default;
    WTPRadioConfiguration(uint8_t radio_id,
                          uint8_t short_preamble,
                          uint8_t num_bssids,
                          uint8_t dtim_period,
                          const uint8_t *bssid,
                          uint16_t beacon_period,
                          const char *country);

    bool Validate() const;
};

struct WritableWTPRadioConfigurationArray {
  private:
    std::vector<WTPRadioConfiguration> items;

  public:
    WritableWTPRadioConfigurationArray(const WritableWTPRadioConfigurationArray &) = delete;
    WritableWTPRadioConfigurationArray();

    void Add(WTPRadioConfiguration element);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const;
    void Log() const;
};

struct ReadableWTPRadioConfigurationArray {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const WTPRadioConfiguration *, max_count> items;
    size_t count;

  public:
    ReadableWTPRadioConfigurationArray(const ReadableWTPRadioConfigurationArray &) = delete;
    ReadableWTPRadioConfigurationArray();

    bool Deserialize(RawData *raw_data);
    nonstd::span<const WTPRadioConfiguration *const> Get() const;
    void Log() const;
};
