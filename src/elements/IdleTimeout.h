#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <string_view>

struct __attribute__((packed)) IdleTimeout : ElementHeader {
  protected:
    NetworkU32 timeout;

  public:
    IdleTimeout(const IdleTimeout &) = default;
    IdleTimeout(IdleTimeout &&) = default;
    IdleTimeout(uint32_t timeout);

    uint32_t GetTimeout() const;
    bool Validate() const;
    uint16_t GetTotalLength() const;
    void Serialize(RawData *raw_data) const;
    static IdleTimeout *Deserialize(RawData *raw_data);
    void Log() const;
};
