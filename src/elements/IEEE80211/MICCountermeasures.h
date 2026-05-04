#pragma once

#include "Helpers.h"
#include "IElement.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <vector>

struct __attribute__((packed)) MICCountermeasures : ElementHeader {
  public:
    static const size_t mac_address_size = 6;

    // Radio ID: An 8-bit value representing the radio, whose value is between 0 and 31.
    uint8_t RadioID;

    // WLAN ID: An 8-bit value specifying the WLAN Identifier. The value MUST be between one (1) and 16.
    uint8_t WlanID;

    // MAC Address: The MAC Address of the station that caused the MIC failure.
    uint8_t MACAddress[mac_address_size];

    MICCountermeasures(const MICCountermeasures &) = default;
    MICCountermeasures(uint8_t radio_id, uint8_t wlan_id, const uint8_t *mac_address);

    bool Validate() const;
};

struct WritableMICCountermeasuresArray : IWritableWTPEventRequestOptionalElement {
  private:
    std::vector<MICCountermeasures> items;

  public:
    WritableMICCountermeasuresArray(const WritableMICCountermeasuresArray &) = delete;
    WritableMICCountermeasuresArray();

    void Add(MICCountermeasures cm);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const override;
    void Log() const override;
};

struct ReadableMICCountermeasuresArray : IReadableWTPEventRequestOptionalElement {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const MICCountermeasures *, max_count> items;
    size_t count;

  public:
    ReadableMICCountermeasuresArray(const ReadableMICCountermeasuresArray &) = delete;
    ReadableMICCountermeasuresArray();

    bool Deserialize(RawData *raw_data) override;
    ElementHeader::ElementType GetElementType() const override;
    bool IsPresent() const override;
    nonstd::span<const MICCountermeasures *const> Get() const;
    void Log() const override;
};
