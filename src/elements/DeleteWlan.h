#pragma once

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
    void Serialize(RawData *raw_data) const;
    static DeleteWlan *Deserialize(RawData *raw_data);
};

struct WritableDeleteWlanArray {
  private:
    std::vector<DeleteWlan> items;

  public:
    WritableDeleteWlanArray(const WritableDeleteWlanArray &) = delete;
    WritableDeleteWlanArray();

    void Add(DeleteWlan wlan);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const;
    void Log() const;
};

struct ReadableDeleteWlanArray {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const DeleteWlan *, max_count> items;
    size_t count;

  public:
    ReadableDeleteWlanArray(const ReadableDeleteWlanArray &) = delete;
    ReadableDeleteWlanArray();

    bool Deserialize(RawData *raw_data);
    nonstd::span<const DeleteWlan *const> Get() const;
    void Log() const;
};
