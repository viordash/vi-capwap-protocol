#pragma once

#include "elements/ElementHeader.h"
#include <cstdint>

struct __attribute__((packed)) ECNSupport : ElementHeader {
    enum Type : uint8_t { LimitedECN = 0, FullAndLimitedECN = 1 };

    Type type;

    ECNSupport(const ECNSupport &) = delete;
    ECNSupport(Type type);

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static ECNSupport *Deserialize(RawData *raw_data);

    uint16_t GetTotalLength() const;
    void Log() const;
};