#pragma once

#include "elements/ElementHeader.h"
#include <cstdint>

struct __attribute__((packed)) WTPFrameTunnelMode : ElementHeader {
    // reserved bit for future use
    uint8_t U : 1;
    // When Local Bridging is used, the WTP does not tunnel user traffic to the AC; all user traffic is locally bridged.  This value MUST NOT be used when the WTP MAC Type is set to Split MAC
    uint8_t L : 1;
    // requires the WTP and AC to encapsulate all user payload as native IEEE 802.3 frames.  All user traffic is tunneled to the AC. This value MUST NOT be used when the WTP MAC Type is set to Split MAC.
    uint8_t E : 1;
    //requires the WTP and AC to encapsulate all user payloads as native wireless frames, as defined by the wireless binding
    uint8_t N : 1;

    uint8_t Reservd : 4;

    WTPFrameTunnelMode(const WTPFrameTunnelMode &) = delete;
    WTPFrameTunnelMode(bool l, bool e, bool n);

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static WTPFrameTunnelMode *Deserialize(RawData *raw_data);
    uint16_t GetTotalLength() const;
    void Log() const;
};