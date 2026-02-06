#pragma once

#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>

struct __attribute__((packed)) CAPWAPLocalIPv4Address : ElementHeader {
  private:
    // IP Address:   The IP address of an interface.
    uint32_t ipaddress;

  public:
    CAPWAPLocalIPv4Address(const CAPWAPLocalIPv4Address &) = delete;
    CAPWAPLocalIPv4Address(uint32_t ipaddress);

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static CAPWAPLocalIPv4Address *Deserialize(RawData *raw_data);
    uint16_t GetTotalLength() const;
    uint32_t GetIPAddress() const;
    void Log() const;
};

struct WritableCAPWAPLocalIPV4AdrArray {
  private:
    const nonstd::span<const CAPWAPLocalIPv4Address> items;

  public:
    WritableCAPWAPLocalIPV4AdrArray(const WritableCAPWAPLocalIPV4AdrArray &) = delete;
    WritableCAPWAPLocalIPV4AdrArray(const nonstd::span<const CAPWAPLocalIPv4Address> &items);

    void Serialize(RawData *raw_data) const;
    uint16_t GetTotalLength() const;
    void Log() const;
};

struct ReadableCAPWAPLocalIPV4AdrArray {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const CAPWAPLocalIPv4Address *, max_count> items;
    size_t count;

  public:
    ReadableCAPWAPLocalIPV4AdrArray(const ReadableCAPWAPLocalIPV4AdrArray &) = delete;
    ReadableCAPWAPLocalIPV4AdrArray();

    bool Deserialize(RawData *raw_data);
    nonstd::span<const CAPWAPLocalIPv4Address *const> Get() const;
    void Log() const;
};