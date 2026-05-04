#pragma once

#include "Helpers.h"
#include "IElement.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <vector>

// RFC 5416 Section 6.13 - IEEE 802.11 Station
//
//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |    Radio ID   |        Association ID         |     Flags     |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                           MAC Address                         |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |          MAC Address          |          Capabilities         |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |   WLAN ID     |Supported Rates|
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Type:   1036 for IEEE 802.11 Station
// Length: >= 14 (13 fixed + at least 1 byte for Supported Rates)

struct __attribute__((packed)) Station : ElementHeader {
  public:
    static const size_t mac_address_size = 6;
    static const size_t max_supported_rates_length = 126;
    static const uint8_t min_wlan_id = 1;
    static const uint8_t max_wlan_id = 16;

  protected:
    // Radio ID: An 8-bit value representing the radio, whose value is between 0 and 31.
    uint8_t radio_id;

    // Association ID: IEEE 802.11 Association Identifier
    NetworkU16 association_id;

    // Flags: Reserved bits must be set to zero
    uint8_t flags;

    // MAC Address: The station's MAC Address
    uint8_t mac_address[mac_address_size];

    // Capabilities: IEEE 802.11 Capabilities Information Field
    NetworkU16 capabilities;

    // WLAN ID: An 8-bit value specifying the WLAN Identifier (1-16)
    uint8_t wlan_id;

  public:
    // Supported Rates: Variable-length field with supported rates

    Station(const Station &) = default;
    Station(uint8_t radio_id,
            uint16_t association_id,
            uint8_t flags,
            const uint8_t *mac_address,
            uint16_t capabilities,
            uint8_t wlan_id,
            uint16_t supported_rates_length);

    uint8_t GetRadioID() const;
    uint16_t GetAssociationID() const;
    uint8_t GetFlags() const;
    const uint8_t *GetMACAddress() const;
    uint16_t GetCapabilities() const;
    uint8_t GetWlanID() const;
    uint16_t GetSupportedRatesLength() const;

    bool Validate() const;
};

struct WritableStationArray : IWritableElement {
  public:
    struct Item {
        nonstd::span<const uint8_t> data;
        Station header;

        Item(const Item &) = default;
        Item(uint8_t radio_id,
             uint16_t association_id,
             uint8_t flags,
             const uint8_t *mac_address,
             uint16_t capabilities,
             uint8_t wlan_id,
             nonstd::span<const uint8_t> supported_rates)
            : data{ supported_rates },
              header{ radio_id, association_id,       flags, mac_address, capabilities,
                      wlan_id,  (uint16_t)data.size() } {};
    };

  private:
    std::vector<Item> items;

  public:
    WritableStationArray(const WritableStationArray &) = delete;
    WritableStationArray();

    void Add(Item element);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableStationArray : IReadableElement {
  public:
    static const size_t max_count = 32;

    struct Item : Station {
        uint8_t data[];
        Item(const Item &) = delete;
    };

  protected:
    std::array<const Item *, max_count> items;
    size_t count;

  public:
    ReadableStationArray(const ReadableStationArray &) = delete;
    ReadableStationArray();

    bool Deserialize(RawData *raw_data) override final;
    nonstd::span<const ReadableStationArray::Item *const> Get() const;
    void Log() const override final;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};
