#pragma once

#include "Helpers.h"
#include "IElement.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <vector>

// RFC 5416 Section 6.20 - IEEE 802.11 Update Station QoS
//
//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |    Radio ID   |                                               |
// +-+-+-+-+-+-+-+-+                                               +
// |                           MAC Address                         |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |        QoS Sub-Element        |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Type:   1043 for IEEE 802.11 Update Station QoS
// Length:  9

// QoS Sub-Element (RFC 5416 Section 6.20)
//  0                   1
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// | Reserved|8021p|RSV| DSCP Tag  |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Reserved: 5 bits (must be zero)
// 8021p: 3 bits (802.1p priority value)
// RSV: 2 bits (reserved, must be zero)
// DSCP Tag: 6 bits (DSCP label)

struct __attribute__((packed)) UpdateStationQoS : ElementHeader {
  public:
    static const size_t mac_address_size = 6;

    // Radio ID: An 8-bit value representing the radio, whose value is between 0 and 31.
    uint8_t RadioID;

    // MAC Address: A 48-bit value containing the MAC Address of the station.
    uint8_t MACAddress[mac_address_size];

    // QoS Sub-Element: A 16-bit value containing the QoS Sub-Element value.
    // Bits 15-11: Reserved (5 bits)
    // Bits 10-8: 802.1p Tag (3 bits)
    // Bits 7-6: Reserved (2 bits)
    // Bits 5-0: DSCP Tag (6 bits)
    NetworkU16 QoSSubElement;

    UpdateStationQoS(const UpdateStationQoS &) = default;
    UpdateStationQoS(uint8_t radio_id,
                     const uint8_t *mac_address,
                     uint8_t priority_8021p,
                     uint8_t dscp_tag);

    uint8_t Get8021pTag() const;
    uint8_t GetDscpTag() const;
    void Set8021pTag(uint8_t tag);
    void SetDscpTag(uint8_t tag);

    bool Validate() const;
};

struct WritableUpdateStationQoSArray : IWritableElement {
  private:
    std::vector<UpdateStationQoS> items;

  public:
    WritableUpdateStationQoSArray(const WritableUpdateStationQoSArray &) = delete;
    WritableUpdateStationQoSArray();

    void Add(UpdateStationQoS element);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const override;
    void Log() const override;
};

struct ReadableUpdateStationQoSArray : IReadableElement {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const UpdateStationQoS *, max_count> items;
    size_t count;

  public:
    ReadableUpdateStationQoSArray(const ReadableUpdateStationQoSArray &) = delete;
    ReadableUpdateStationQoSArray();

    bool Deserialize(RawData *raw_data) override;
    ElementHeader::ElementType GetElementType() const override;
    bool IsPresent() const override;
    nonstd::span<const UpdateStationQoS *const> Get() const;
    void Log() const override;
};
