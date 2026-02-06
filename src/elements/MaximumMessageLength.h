#pragma once

#include "elements/ElementHeader.h"
#include <cstdint>

struct __attribute__((packed)) MaximumMessageLength : ElementHeader {
  private:
    NetworkU16 length;

  public:
    MaximumMessageLength(const MaximumMessageLength &) = delete;
    MaximumMessageLength(uint16_t length);

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static MaximumMessageLength *Deserialize(RawData *raw_data);

    uint16_t GetValue() const;
    uint16_t GetTotalLength() const;
    void Log() const;
};