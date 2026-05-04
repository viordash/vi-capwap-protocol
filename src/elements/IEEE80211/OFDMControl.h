#pragma once

#include "Helpers.h"
#include "IElement.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <vector>

// RFC 5416 Section 6.10 - IEEE 802.11 OFDM Control
//
//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |    Radio ID   |    Reserved   | Current Chan  |  Band Support |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                         TI Threshold                          |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Type:   1033 for IEEE 802.11 OFDM Control
// Length: 8

struct __attribute__((packed)) OFDMControl : ElementHeader {
  public:
    // Band Support bit flags
    static const uint8_t BAND_5_15_5_25_GHZ = 0x01;     // Bit 0: 5.15-5.25 GHz
    static const uint8_t BAND_5_25_5_35_GHZ = 0x02;     // Bit 1: 5.25-5.35 GHz
    static const uint8_t BAND_5_725_5_825_GHZ = 0x04;   // Bit 2: 5.725-5.825 GHz
    static const uint8_t BAND_5_47_5_725_GHZ = 0x08;    // Bit 3: 5.47-5.725 GHz
    static const uint8_t BAND_LOWER_JP_5_25_GHZ = 0x10; // Bit 4: Lower Japanese 5.25 GHz
    static const uint8_t BAND_5_03_5_091_GHZ = 0x20;    // Bit 5: 5.03-5.091 GHz
    static const uint8_t BAND_4_94_4_99_GHZ = 0x40;     // Bit 6: 4.94-4.99 GHz

  protected:
    // Radio ID: An 8-bit value representing the radio, whose value is between 0 and 31.
    uint8_t radio_id;

    // Reserved: Set to zero
    uint8_t reserved;

    // Current Channel: Current operating frequency channel of the OFDM PHY
    uint8_t current_channel;

    // Band Support: Capability to operate in U-NII bands (bit field)
    uint8_t band_support;

    // TI Threshold: Threshold for detecting a busy medium
    NetworkU32 ti_threshold;

  public:
    OFDMControl(const OFDMControl &) = default;
    OFDMControl(uint8_t radio_id,
                uint8_t current_channel,
                uint8_t band_support,
                uint32_t ti_threshold);

    uint8_t GetRadioID() const;
    uint8_t GetCurrentChannel() const;
    uint8_t GetBandSupport() const;
    uint32_t GetTIThreshold() const;

    bool Validate() const;
};

struct WritableOFDMControlArray : IWritableElement {
  protected:
    std::vector<OFDMControl> items;

  public:
    WritableOFDMControlArray(const WritableOFDMControlArray &) = delete;
    WritableOFDMControlArray();

    void Add(OFDMControl element);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const override;
    void Log() const override;
};

struct ReadableOFDMControlArray : IReadableElement {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const OFDMControl *, max_count> items;
    size_t count;

  public:
    ReadableOFDMControlArray(const ReadableOFDMControlArray &) = delete;
    ReadableOFDMControlArray();

    bool Deserialize(RawData *raw_data) override;
    ElementHeader::ElementType GetElementType() const override;
    bool IsPresent() const override;
    nonstd::span<const OFDMControl *const> Get() const;
    void Log() const override;
};
