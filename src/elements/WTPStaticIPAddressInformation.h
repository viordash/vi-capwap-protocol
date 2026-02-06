#pragma once

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
    void Serialize(RawData *raw_data) const;
    static WTPStaticIPAddressInformation *Deserialize(RawData *raw_data);
    uint16_t GetTotalLength() const;
    void Log() const;
};
