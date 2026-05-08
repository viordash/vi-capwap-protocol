#pragma once

#include "Helpers.h"
#include "IElement.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <vector>

// RFC 5416 Section 6.18 - IEEE 802.11 Tx Power
//
//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |    Radio ID   |   Reserved    |       Current Tx Power        |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Type:   1041 for IEEE 802.11 Tx Power
// Length:  4

struct __attribute__((packed)) TxPower : ElementHeader {
  public:
    // Radio ID: An 8-bit value representing the radio, whose value is between 0 and 31.
    uint8_t RadioID;

    // Reserved: An 8-bit field that MUST be set to zero (0) on transmission
    // and ignored on reception.
    uint8_t Reserved;

    // Current Tx Power: A 16-bit unsigned value containing the current transmit
    // power level in units of dBm currently being used by the radio.
    NetworkU16 CurrentTxPower;

    TxPower(const TxPower &) = default;
    TxPower(uint8_t radio_id, uint16_t current_tx_power);

    bool Validate() const;
};

struct WritableTxPowerArray : IWritableConfigurationStatusRequestOptionalElement,
                              IWritableConfigurationStatusResponseOptionalElement,
                              IWritableConfigurationUpdateRequestOptionalElement {
  private:
    std::vector<TxPower> items;

  public:
    WritableTxPowerArray(const WritableTxPowerArray &) = delete;
    WritableTxPowerArray();

    void Add(TxPower element);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const override;
    void Log() const override;
};

struct ReadableTxPowerArray : IReadableConfigurationStatusRequestOptionalElement,
                              IReadableConfigurationStatusResponseOptionalElement,
                              IReadableConfigurationUpdateRequestOptionalElement {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const TxPower *, max_count> items;
    size_t count;

  public:
    ReadableTxPowerArray(const ReadableTxPowerArray &) = delete;
    ReadableTxPowerArray();

    bool Deserialize(RawData *raw_data) override;
    ElementHeader::ElementType GetElementType() const override;
    bool IsPresent() const override;
    nonstd::span<const TxPower *const> Get() const;
    void Log() const override;
};
