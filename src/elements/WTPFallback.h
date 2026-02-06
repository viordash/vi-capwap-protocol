#pragma once

#include "elements/ElementHeader.h"
#include <cstdint>

struct __attribute__((packed)) WTPFallback : ElementHeader {
    enum Mode : uint8_t { Reserved = 0, Enabled = 1, Disabled = 1 };

    Mode mode;

    WTPFallback(const WTPFallback &) = default;
    WTPFallback(WTPFallback &&) = default;
    WTPFallback(Mode mode);

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static WTPFallback *Deserialize(RawData *raw_data);

    uint16_t GetTotalLength() const;
    void Log() const;
};