#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <string_view>

struct __attribute__((packed)) ACTimestamp : ElementHeader {
  protected:
    NetworkU32 timestamp;

  public:
    ACTimestamp(const ACTimestamp &) = default;
    ACTimestamp(ACTimestamp &&) = default;
    ACTimestamp(uint32_t timestamp);

    uint32_t GetTimestamp() const;
    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static ACTimestamp *Deserialize(RawData *raw_data);
    void Log() const;
};
