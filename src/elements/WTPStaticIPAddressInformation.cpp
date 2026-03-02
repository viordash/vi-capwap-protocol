#include "WTPStaticIPAddressInformation.h"
#include "network_utils.h"
#include "lassert.h"
#include "logging.h"
#include <cstring>

WTPStaticIPAddressInformation::WTPStaticIPAddressInformation(uint32_t ipaddress,
                                                             uint32_t netmask,
                                                             uint32_t gateway,
                                                             bool use_static)
    : ElementHeader(ElementHeader::WTPStaticIPAddressInformation,
                    sizeof(WTPStaticIPAddressInformation) - sizeof(ElementHeader)),
      IpAddress{ ipaddress }, Netmask{ netmask }, Gateway{ gateway },
      Static{ use_static ? (uint8_t)1 : (uint8_t)0 } {
}
bool WTPStaticIPAddressInformation::Validate() const {
    static_assert(sizeof(WTPStaticIPAddressInformation) == 17);
    return ElementHeader::GetElementType() == ElementHeader::WTPStaticIPAddressInformation
        && ElementHeader::GetLength()
               == (sizeof(WTPStaticIPAddressInformation) - sizeof(ElementHeader));
}
void WTPStaticIPAddressInformation::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(WTPStaticIPAddressInformation) <= raw_data->end);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
    memcpy(raw_data->current, this, sizeof(WTPStaticIPAddressInformation));
#pragma GCC diagnostic pop
    raw_data->current += sizeof(WTPStaticIPAddressInformation);
}
WTPStaticIPAddressInformation *WTPStaticIPAddressInformation::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(WTPStaticIPAddressInformation) > raw_data->end) {
        return nullptr;
    }

    auto res = (WTPStaticIPAddressInformation *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += sizeof(WTPStaticIPAddressInformation);
    return res;
}
uint16_t WTPStaticIPAddressInformation::GetTotalLength() const {
    return GetLength() + sizeof(ElementHeader);
}

void WTPStaticIPAddressInformation::Log() const {
    log_i("ME WTPStaticIPAddressInformation IP Address:%s, Netmask:%s, Gateway:%s, use static:%u",
          IpToString(IpAddress).c_str(),
          IpToString(Netmask).c_str(),
          IpToString(Gateway).c_str(),
          Static);
}
