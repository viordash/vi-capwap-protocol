#pragma once

#include "IElement.h"
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

    WTPFrameTunnelMode(const WTPFrameTunnelMode &) = default;
    WTPFrameTunnelMode(bool l, bool e, bool n);

    bool Validate() const;
    uint16_t GetTotalLength() const;
    void Log() const;
};

struct WritableWTPFrameTunnelMode : IWritableElement {
  protected:
    WTPFrameTunnelMode element;

  public:
    WritableWTPFrameTunnelMode(bool l, bool e, bool n);

    void Serialize(RawData *raw_data) const override final;
    uint16_t GetTotalLength() const;
    void Log() const override final;
};

struct ReadableWTPFrameTunnelMode : IReadableElement {
  protected:
    WTPFrameTunnelMode *element = nullptr;
    bool is_present = false;

  public:
    bool Deserialize(RawData *raw_data) override final;
    void Log() const override final;
    const WTPFrameTunnelMode *Get() const;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};