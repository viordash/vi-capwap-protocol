#pragma once

#include "IElement.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <vector>

struct __attribute__((packed)) AssignedWtpBssid : ElementHeader {
  public:
    static const size_t bssid_size = 6;

    // Radio ID: An 8-bit value representing the radio, whose value is between one (1) and 31.
    uint8_t RadioID;

    // WLAN ID: An 8-bit value specifying the WLAN Identifier. The value MUST be between one (1) and 16.
    uint8_t WlanID;

    // BSSID: The BSSID assigned by the WTP for the WLAN created as a result of receiving an IEEE 802.11 Add WLAN.
    uint8_t BSSID[bssid_size];

    AssignedWtpBssid(const AssignedWtpBssid &) = default;
    AssignedWtpBssid(uint8_t radio_id, uint8_t wlan_id, const uint8_t *bssid);

    bool Validate() const;
};

struct WritableAssignedWtpBssidArray : IWritableWlanConfigurationResponseOptionalElement {
  private:
    std::vector<AssignedWtpBssid> items;

  public:
    WritableAssignedWtpBssidArray(const WritableAssignedWtpBssidArray &) = delete;
    WritableAssignedWtpBssidArray();

    void Add(AssignedWtpBssid bssid);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableAssignedWtpBssidArray : IReadableWlanConfigurationResponseOptionalElement {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const AssignedWtpBssid *, max_count> items;
    size_t count;

  public:
    ReadableAssignedWtpBssidArray(const ReadableAssignedWtpBssidArray &) = delete;
    ReadableAssignedWtpBssidArray();

    bool Deserialize(RawData *raw_data) override final;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
    nonstd::span<const AssignedWtpBssid *const> Get() const;
    void Log() const override final;
};
