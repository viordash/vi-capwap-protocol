#pragma once

#include "elements/ElementHeader.h"
#include <cstdint>

struct __attribute__((packed)) CAPWAPTimers : ElementHeader {
    uint8_t Discovery;
    uint8_t EchoInterval;

    CAPWAPTimers(const CAPWAPTimers &) = default;
    CAPWAPTimers(CAPWAPTimers &&) = default;
    CAPWAPTimers(uint8_t discovery, uint8_t echo_interval);

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static CAPWAPTimers *Deserialize(RawData *raw_data);

    uint16_t GetTotalLength() const;
    void Log() const;
};