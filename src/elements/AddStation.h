#pragma once

#include "Helpers.h"
#include "IElement.h"
#include "elements/ElementHeader.h"
#include "elements/MacAddress.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <string>
#include <vector>

// RFC 5415 Section 4.6.8 - Add Station
//
//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |    Radio ID   |     Length    |          MAC Address ...
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |  VLAN Name...
// +-+-+-+-+-+-+-+-+
//
// Type:   8 for Add Station
// Length: >= 8

struct __attribute__((packed)) AddStation : ElementHeader {
  public:
    static const size_t max_vlan_name_length = 512;

    // Radio ID: An 8-bit value representing the radio, whose value is between one (1) and 31.
    uint8_t RadioID;

    // MAC Address: The station's MAC address.
    ReadableMacAddress MACAddress;

    AddStation(const AddStation &) = default;
    AddStation(uint8_t radio_id, uint8_t mac_length, uint16_t vlan_name_length);

    bool Validate() const;
    uint16_t GetVlanNameLength() const;
};

struct WritableAddStationArray : IWritableElement {
  public:
    struct Item {
        MacAddress Mac;
        std::string vlan_name;
        AddStation header;
        Item(const Item &) = default;
        Item(uint8_t radio_id, MacAddress &&mac_address, std::string vlan_name);
    };

  private:
    std::vector<Item> items;

  public:
    WritableAddStationArray(const WritableAddStationArray &) = delete;
    WritableAddStationArray();

    void Add(uint8_t radio_id, MacAddress mac_address, std::string vlan_name = "");
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableAddStationArray : IReadableElement {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const AddStation *, max_count> items;
    size_t count;

  public:
    ReadableAddStationArray(const ReadableAddStationArray &) = delete;
    ReadableAddStationArray();

    bool Deserialize(RawData *raw_data) override final;
    nonstd::span<const AddStation *const> Get() const;
    void Log() const override final;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};
