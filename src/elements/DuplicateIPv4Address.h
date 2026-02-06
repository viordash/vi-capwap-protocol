#pragma once

#include "Helpers.h"
#include "elements/DuplicateStatus.h"
#include "elements/ElementHeader.h"
#include "elements/MacAddress.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <vector>

struct __attribute__((packed)) DuplicateIPv4Address : ElementHeader {
  public:
    // IP Address:   The IP address currently used by the WTP.
    uint32_t IPAddress;

    // Status:   The status of the duplicate IP address.  The value MUST be set to 1 when a duplicate address is detected, and 0 when the duplicate address has been cleared
    DuplicateStatus Status;

    // MAC Address:   The MAC address of the offending device.
    ReadableMacAddress MACAddress;

    DuplicateIPv4Address(const DuplicateIPv4Address &) = default;
    DuplicateIPv4Address(uint32_t ipaddress, DuplicateStatus status, uint8_t mac_length);

    bool Validate() const;
};

struct WritableDuplicateIPv4AdrArray {
  public:
    struct Item {
        MacAddress Mac;
        DuplicateIPv4Address header;
        Item(const Item &) = default;
        Item(uint32_t ipaddress, DuplicateStatus status, MacAddress &&mac_address)
            : Mac{ std::move(mac_address) }, header{ ipaddress, status, (uint8_t)Mac.Length } {};
    };

  private:
    std::vector<Item> items;

  public:
    WritableDuplicateIPv4AdrArray(const WritableDuplicateIPv4AdrArray &) = delete;
    WritableDuplicateIPv4AdrArray();

    void Add(uint32_t ipaddress, DuplicateStatus status, MacAddress mac_address);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const;
    uint16_t GetTotalLength() const;
    void Log() const;
};

struct ReadableDuplicateIPv4AdrArray {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const DuplicateIPv4Address *, max_count> items;
    size_t count;

  public:
    ReadableDuplicateIPv4AdrArray(const ReadableDuplicateIPv4AdrArray &) = delete;
    ReadableDuplicateIPv4AdrArray();

    bool Deserialize(RawData *raw_data);
    nonstd::span<const DuplicateIPv4Address *const> Get() const;
    void Log() const;
};