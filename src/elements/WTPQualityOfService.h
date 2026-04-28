#pragma once

#include "Helpers.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <vector>

// RFC 5416 Section 6.22 - IEEE 802.11 WTP Quality of Service
//
//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |    Radio ID   |Tagging Policy |                               |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  Voice QoS Sub-Element        +
// ...
//
// Type:   1045 for IEEE 802.11 WTP Quality of Service
// Length:  34

// RFC 5416 Section 6.22 - QoS Sub-Element (8 bytes)
//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |  Queue Depth  |             CWMin             |     CWMax     |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |     CWMax     |     AIFS      | Reserved|8021p|RSV| DSCP Tag  |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
struct __attribute__((packed)) QoSSubElement {
    // Queue Depth: The number of packets that can be on the specific QoS transmit queue
    uint8_t QueueDepth;

    // CWMin: The Contention Window minimum (CWmin) value (16 bits)
    NetworkU16 CWMin;

    // CWMax: The Contention Window maximum (CWmax) value (16 bits)
    NetworkU16 CWMax;

    // AIFS: The Arbitration Inter Frame Spacing (AIFS) value
    uint8_t AIFS;

    // Tag Fields: Reserved(5) + 8021p(3) + RSV(2) + DSCP(6) = 16 bits
    // Bits 15-11: Reserved (5 bits)
    // Bits 10-8: 802.1p priority value (3 bits)
    // Bits 7-6: Reserved (2 bits)
    // Bits 5-0: DSCP Tag (6 bits)
    NetworkU16 TagFields;

    uint8_t Get8021pTag() const;
    uint8_t GetDscpTag() const;
    void Set8021pTag(uint8_t tag);
    void SetDscpTag(uint8_t tag);
};

struct __attribute__((packed)) WTPQualityOfService : ElementHeader {
  public:
    // Tagging Policy bit field (RFC 5416 Section 6.22):
    //      0 1 2 3 4 5 6 7
    //     +-+-+-+-+-+-+-+-+
    //     |Rsvd |P|Q|D|O|I|
    //     +-+-+-+-+-+-+-+-+
    // Rsvd: Reserved (3 bits, must be zero)
    // P: 802.1p priority tag enable (bit 4)
    // Q: 802.1p priority tag queue depth behavior (bit 3)
    // D: DSCP tag enable (bit 2)
    // O: DSCP tag on outer IP header (bit 1)
    // I: DSCP tag on inner IP header (bit 0)

    // Radio ID: An 8-bit value representing the radio, whose value is between 0 and 31.
    uint8_t RadioID;

    // Tagging Policy: An unsigned 8-bit bit field
    uint8_t TaggingPolicy;

    // QoS Sub-Elements for each access category
    QoSSubElement Voice;
    QoSSubElement Video;
    QoSSubElement BestEffort;
    QoSSubElement Background;

    WTPQualityOfService(const WTPQualityOfService &) = default;
    WTPQualityOfService(uint8_t radio_id, uint8_t tagging_policy);

    // Tagging Policy bit field accessors
    bool GetP() const;    // 802.1p priority tag enable
    void SetP(bool value);
    bool GetQ() const;    // 802.1p priority tag queue depth behavior
    void SetQ(bool value);
    bool GetD() const;    // DSCP tag enable
    void SetD(bool value);
    bool GetO() const;    // DSCP tag on outer IP header
    void SetO(bool value);
    bool GetI() const;    // DSCP tag on inner IP header
    void SetI(bool value);

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static WTPQualityOfService *Deserialize(RawData *raw_data);
};

struct WritableWTPQualityOfServiceArray {
  private:
    std::vector<WTPQualityOfService> items;

  public:
    WritableWTPQualityOfServiceArray(const WritableWTPQualityOfServiceArray &) = delete;
    WritableWTPQualityOfServiceArray();

    void Add(WTPQualityOfService element);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const;
    void Log() const;
};

struct ReadableWTPQualityOfServiceArray {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const WTPQualityOfService *, max_count> items;
    size_t count;

  public:
    ReadableWTPQualityOfServiceArray(const ReadableWTPQualityOfServiceArray &) = delete;
    ReadableWTPQualityOfServiceArray();

    bool Deserialize(RawData *raw_data);
    nonstd::span<const WTPQualityOfService *const> Get() const;
    void Log() const;
};
