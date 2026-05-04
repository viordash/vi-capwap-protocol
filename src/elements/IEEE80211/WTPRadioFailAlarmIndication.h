#pragma once

#include "Helpers.h"
#include "IElement.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <vector>

// RFC 5416 Section 6.24 - IEEE 802.11 WTP Radio Fail Alarm Indication
//
//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |    Radio ID   |     Type      |    Status     |      Pad      |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Type:   1047 for IEEE 802.11 WTP Radio Fail Alarm Indication
// Length:  4

struct __attribute__((packed)) WTPRadioFailAlarmIndication : ElementHeader {
  public:
    // Type: An 8-bit value indicating the source of the alarm.
    enum AlarmType : uint8_t { Receiver = 1, Transmitter = 2 };

    // Status: An 8-bit value indicating the status of the alarm.
    enum AlarmStatus : uint8_t { Reserved = 0, Cleared = 1, Minor = 2, Major = 3, Critical = 4 };

    // Radio ID: An 8-bit value representing the radio, whose value is between 0 and 31.
    uint8_t RadioID;

    // Type: An 8-bit value indicating the source of the alarm.
    AlarmType Type;

    // Status: An 8-bit value indicating the status of the alarm.
    AlarmStatus Status;

    // Pad: An 8-bit field that MUST be set to zero (0) on transmission
    // and ignored on reception.
    uint8_t Pad;

    WTPRadioFailAlarmIndication(const WTPRadioFailAlarmIndication &) = default;
    WTPRadioFailAlarmIndication(uint8_t radio_id, AlarmType type, AlarmStatus status);

    bool Validate() const;
};

struct WritableWTPRadioFailAlarmIndicationArray : IWritableChangeStateEventRequestOptionalElement {
  private:
    std::vector<WTPRadioFailAlarmIndication> items;

  public:
    WritableWTPRadioFailAlarmIndicationArray(const WritableWTPRadioFailAlarmIndicationArray &) =
        delete;
    WritableWTPRadioFailAlarmIndicationArray();

    void Add(WTPRadioFailAlarmIndication element);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const override;
    void Log() const override;
    ElementHeader::ElementType GetElementType() const override final;
};

struct ReadableWTPRadioFailAlarmIndicationArray : IReadableChangeStateEventRequestOptionalElement {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const WTPRadioFailAlarmIndication *, max_count> items;
    size_t count;

  public:
    ReadableWTPRadioFailAlarmIndicationArray(const ReadableWTPRadioFailAlarmIndicationArray &) =
        delete;
    ReadableWTPRadioFailAlarmIndicationArray();

    bool Deserialize(RawData *raw_data) override;
    ElementHeader::ElementType GetElementType() const override;
    bool IsPresent() const override;
    nonstd::span<const WTPRadioFailAlarmIndication *const> Get() const;
    void Log() const override;
};
