#pragma once

#include "elements/ElementHeader.h"
#include <cstdint>

struct __attribute__((packed)) WTPMACType : ElementHeader {
    enum Type : uint8_t { Local_MAC = 0, Split_MAC = 1, Both = 2 };

    Type type;

    WTPMACType(const WTPMACType &) = delete;
    WTPMACType(Type type);

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static WTPMACType *Deserialize(RawData *raw_data);
    uint16_t GetTotalLength() const;
    void Log() const;
};