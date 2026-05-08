#pragma once

#include "IElement.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <vector>

struct __attribute__((packed)) DeleteWlan : ElementHeader {
  public:
    // Radio ID: An 8-bit value representing the radio, whose value is between 0 and 31.
    uint8_t RadioID;

    // WLAN ID: An 8-bit value specifying the WLAN Identifier. The value MUST be between one (1) and 16.
    uint8_t WlanID;

    DeleteWlan(const DeleteWlan &) = default;
    DeleteWlan(uint8_t radio_id, uint8_t wlan_id);

    bool Validate() const;
};

struct WritableDeleteWlanArray : IWritableWlanConfigurationRequestOptionalElement {
  private:
    std::vector<DeleteWlan> items;

  public:
    WritableDeleteWlanArray(const WritableDeleteWlanArray &) = delete;
    WritableDeleteWlanArray();

    void Add(DeleteWlan wlan);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const override;
    void Log() const override;
};

struct ReadableDeleteWlanArray : IReadableWlanConfigurationRequestOptionalElement {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const DeleteWlan *, max_count> items;
    size_t count;

  public:
    ReadableDeleteWlanArray(const ReadableDeleteWlanArray &) = delete;
    ReadableDeleteWlanArray();

    bool Deserialize(RawData *raw_data) override;
    ElementHeader::ElementType GetElementType() const override;
    bool IsPresent() const override;
    nonstd::span<const DeleteWlan *const> Get() const;
    void Log() const override;
};
