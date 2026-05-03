#pragma once

#include "Helpers.h"
#include "IElement.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <vector>

// RFC 5416 Section 6.19 - IEEE 802.11 Tx Power Level
//
//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |    Radio ID   |   Num Levels  |         Power Level           |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |         Power Level           |   ...
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
//
// Type:   1042 for IEEE 802.11 Tx Power Level
// Length:  >= 4

struct __attribute__((packed)) TxPowerLevel : ElementHeader {
  public:
    // Maximum number of power levels
    static const size_t max_levels = 8;

  protected:
    // Radio ID: An 8-bit value representing the radio, whose value is between 0 and 31.
    uint8_t radio_id;

    // Num Levels: An 8-bit value that specifies the number of power levels.
    uint8_t num_levels;

  public:
    // Power Level: A 16-bit signed value, in dBm, for each of the supported power levels.
    NetworkS16 power_levels[];

    TxPowerLevel(const TxPowerLevel &) = default;
    TxPowerLevel(uint8_t radio_id, uint8_t num_levels);

    uint8_t GetRadioID() const;
    uint8_t GetNumLevels() const;

    bool Validate() const;
};

struct WritableTxPowerLevelArray : IWritableElement {
  public:
    struct Item {
        nonstd::span<const int16_t> levels_data;
        TxPowerLevel header;

        Item(const Item &) = default;
        Item(uint8_t radio_id, nonstd::span<const int16_t> levels);

        uint8_t GetRadioID() const;
    };

  private:
    std::vector<Item> items;

  public:
    WritableTxPowerLevelArray(const WritableTxPowerLevelArray &) = delete;
    WritableTxPowerLevelArray();

    void Add(Item element);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableTxPowerLevelArray : IReadableElement {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const TxPowerLevel *, max_count> items;
    size_t count;

  public:
    ReadableTxPowerLevelArray(const ReadableTxPowerLevelArray &) = delete;
    ReadableTxPowerLevelArray();

    bool Deserialize(RawData *raw_data) override final;
    nonstd::span<const TxPowerLevel *const> Get() const;
    void Log() const override final;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};
