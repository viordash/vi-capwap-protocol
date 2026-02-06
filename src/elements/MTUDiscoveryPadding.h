#pragma once

#include "elements/ElementHeader.h"
#include <cstdint>

struct __attribute__((packed)) MTUDiscoveryPadding : ElementHeader {
    MTUDiscoveryPadding(const MTUDiscoveryPadding &) = delete;
    MTUDiscoveryPadding(uint16_t size);

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static MTUDiscoveryPadding *Deserialize(RawData *raw_data);
    uint16_t GetTotalLength() const;
    void Log() const;
};
