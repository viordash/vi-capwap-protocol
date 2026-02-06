#pragma once

#include "Helpers.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>

struct __attribute__((packed)) CAPWAPControlIPv4Address : ElementHeader {
  private:
    // IP Address:   The IP address of an interface.
    uint32_t ipaddress;

    // WTP Count:   The number of WTPs currently connected to the interface, with a maximum value of 65535.
    NetworkU16 wtp_count;

  public:
    CAPWAPControlIPv4Address(const CAPWAPControlIPv4Address &) = delete;
    CAPWAPControlIPv4Address(uint32_t ipaddress, uint16_t wtp_count);

    bool Validate() const;
    uint32_t GetIPAddress() const;
    uint16_t GetWTPCount() const;
    void Log() const;
};

struct WritableCAPWAPControlIPV4AdrArray {
  private:
    const nonstd::span<const CAPWAPControlIPv4Address> items;

  public:
    WritableCAPWAPControlIPV4AdrArray(const WritableCAPWAPControlIPV4AdrArray &) = delete;
    WritableCAPWAPControlIPV4AdrArray(const nonstd::span<const CAPWAPControlIPv4Address> &items);

    void Serialize(RawData *raw_data) const;
    uint16_t GetTotalLength() const;
    void Log() const;
};

struct ReadableCAPWAPControlIPV4AdrArray {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const CAPWAPControlIPv4Address *, max_count> items;
    size_t count;

  public:
    ReadableCAPWAPControlIPV4AdrArray(const ReadableCAPWAPControlIPV4AdrArray &) = delete;
    ReadableCAPWAPControlIPV4AdrArray();

    bool Deserialize(RawData *raw_data);
    nonstd::span<const CAPWAPControlIPv4Address *const> Get() const;
    void Log() const;
};