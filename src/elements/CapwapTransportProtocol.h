#pragma once

#include "elements/ElementHeader.h"
#include <cstdint>

struct __attribute__((packed)) CapwapTransportProtocol : ElementHeader {
    enum Type : uint8_t { UDPLite = 1, UDP = 2 };

    Type type;

    CapwapTransportProtocol(const CapwapTransportProtocol &) = delete;
    CapwapTransportProtocol(Type type);

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static CapwapTransportProtocol *Deserialize(RawData *raw_data);

    uint16_t GetTotalLength() const;
    void Log() const;
};