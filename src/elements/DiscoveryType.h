#pragma once

#include "elements/ElementHeader.h"
#include <cstdint>

struct __attribute__((packed)) DiscoveryType : ElementHeader {
    enum Type : uint8_t { Unknown = 0, StaticConfiguration = 1, DHCP = 2, DNS = 3, ACReferral = 4 };

    Type type;

    DiscoveryType(const DiscoveryType &) = delete;
    DiscoveryType(Type type);

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static DiscoveryType *Deserialize(RawData *raw_data);

    uint16_t GetTotalLength() const;
    void Log() const;
};