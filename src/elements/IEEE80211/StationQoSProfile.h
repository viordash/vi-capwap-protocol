#pragma once

#include "Helpers.h"
#include "IElement.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <vector>

// RFC 5416 Section 6.14 - IEEE 802.11 Station QoS Profile
//
//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                           MAC Address                         |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |          MAC Address          |         Reserved        |8021p|
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Type:   1037 for IEEE 802.11 Station QoS Profile
// Length: 8

struct __attribute__((packed)) StationQoSProfile : ElementHeader {
  public:
    static const size_t mac_address_size = 6;
    static const uint8_t min_8021p_priority = 0;
    static const uint8_t max_8021p_priority = 7;

  protected:
    // MAC Address: The station's MAC Address
    uint8_t mac_address[mac_address_size];

    // Reserved: Must be set to zero
    uint8_t reserved;

    // 8021p: Maximum 802.1p priority value (0-7)
    uint8_t priority_8021p;

  public:
    StationQoSProfile(const StationQoSProfile &) = default;
    StationQoSProfile(const uint8_t *mac_address, uint8_t priority_8021p);

    const uint8_t *GetMACAddress() const;
    uint8_t Get8021pPriority() const;

    bool Validate() const;
};

struct WritableStationQoSProfileArray : IWritableElement {
  private:
    std::vector<StationQoSProfile> items;

  public:
    WritableStationQoSProfileArray(const WritableStationQoSProfileArray &) = delete;
    WritableStationQoSProfileArray();

    void Add(StationQoSProfile element);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const override;
    void Log() const override;
};

struct ReadableStationQoSProfileArray : IReadableElement {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const StationQoSProfile *, max_count> items;
    size_t count;

  public:
    ReadableStationQoSProfileArray(const ReadableStationQoSProfileArray &) = delete;
    ReadableStationQoSProfileArray();

    bool Deserialize(RawData *raw_data) override;
    ElementHeader::ElementType GetElementType() const override;
    bool IsPresent() const override;
    nonstd::span<const StationQoSProfile *const> Get() const;
    void Log() const override;
};
