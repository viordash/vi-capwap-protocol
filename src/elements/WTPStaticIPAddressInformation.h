#pragma once

#include "IOptionalElement.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>

struct __attribute__((packed)) WTPStaticIPAddressInformation : ElementHeader {
    // IP Address: The IP address to assign to the WTP. This field is only valid if the static field is set to one.
    uint32_t IpAddress;

    // Netmask: The IP Netmask. This field is only valid if the static field is set to one.
    uint32_t Netmask;

    // Gateway: The IP address of the gateway.  This field is only valid if the static field is set to one.
    uint32_t Gateway;

    // Static: An 8-bit Boolean stating whether or not the WTP should use a static IP address.
    uint8_t Static;

    WTPStaticIPAddressInformation(const WTPStaticIPAddressInformation &) = default;
    WTPStaticIPAddressInformation(WTPStaticIPAddressInformation &&) = default;
    WTPStaticIPAddressInformation(uint32_t ipaddress,
                                  uint32_t netmask,
                                  uint32_t gateway,
                                  bool use_static);

    bool Validate() const;
    void Log() const;
};

struct WritableWTPStaticIPAddressInformation : IWritableConfigurationStatusRequestOptionalElement {
  protected:
    WTPStaticIPAddressInformation element;

  public:
    WritableWTPStaticIPAddressInformation(uint32_t ipaddress,
                                          uint32_t netmask,
                                          uint32_t gateway,
                                          bool use_static);

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableWTPStaticIPAddressInformation : IReadableConfigurationStatusRequestOptionalElement {
  protected:
    WTPStaticIPAddressInformation *element = nullptr;
    bool is_present = false;

  public:
    bool Deserialize(RawData *raw_data) override final;
    void Log() const override final;
    const WTPStaticIPAddressInformation *const Get() const;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};