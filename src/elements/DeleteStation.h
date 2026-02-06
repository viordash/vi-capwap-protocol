#pragma once

#include "Helpers.h"
#include "elements/DuplicateStatus.h"
#include "elements/ElementHeader.h"
#include "elements/MacAddress.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <vector>

struct __attribute__((packed)) DeleteStation : ElementHeader {
  public:
    // Radio ID: An 8-bit value representing the radio, whose value is between one (1) and 31.
    uint8_t RadioID;

    // MAC Address: The station's MAC address.
    ReadableMacAddress MACAddress;

    DeleteStation(const DeleteStation &) = default;
    DeleteStation(uint8_t radio_id, uint8_t mac_length);

    bool Validate() const;
};

struct WritableDeleteStationArray {
  public:
    struct Item {
        MacAddress Mac;
        DeleteStation header;
        Item(const Item &) = default;
        Item(uint8_t radio_id, MacAddress &&mac_address)
            : Mac{ std::move(mac_address) }, header{ radio_id, (uint8_t)Mac.Length } {};
    };

  private:
    std::vector<Item> items;

  public:
    WritableDeleteStationArray(const WritableDeleteStationArray &) = delete;
    WritableDeleteStationArray();

    void Add(uint8_t radio_id, MacAddress mac_address);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const;
    uint16_t GetTotalLength() const;
    void Log() const;
};

struct ReadableDeleteStationArray {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const DeleteStation *, max_count> items;
    size_t count;

  public:
    ReadableDeleteStationArray(const ReadableDeleteStationArray &) = delete;
    ReadableDeleteStationArray();

    bool Deserialize(RawData *raw_data);
    nonstd::span<const DeleteStation *const> Get() const;
    void Log() const;
};