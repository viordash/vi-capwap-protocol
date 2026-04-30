#include "WTPStaticIPAddressInformation.h"
#include "Logging.h"
#include "NetworkUtils.h"
#include "lassert.h"
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

void WTPStaticIPAddressInformation::Log() const {
    log_i("ME WTPStaticIPAddressInformation IP Address:%s, Netmask:%s, Gateway:%s, use static:%u",
          IpToString(IpAddress).c_str(),
          IpToString(Netmask).c_str(),
          IpToString(Gateway).c_str(),
          Static);
}

WritableWTPStaticIPAddressInformation::WritableWTPStaticIPAddressInformation(uint32_t ipaddress,
                                                                             uint32_t netmask,
                                                                             uint32_t gateway,
                                                                             bool use_static)
    : element{ ipaddress, netmask, gateway, use_static } {
}

void WritableWTPStaticIPAddressInformation::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(WTPStaticIPAddressInformation) <= raw_data->end);
#pragma GCC diagnostic push
#if __GNUC__ >= 8
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif
    memcpy(raw_data->current, &element, sizeof(WTPStaticIPAddressInformation));
#pragma GCC diagnostic pop
    raw_data->current += sizeof(WTPStaticIPAddressInformation);
}

void WritableWTPStaticIPAddressInformation::Log() const {
    element.Log();
}

bool ReadableWTPStaticIPAddressInformation::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(WTPStaticIPAddressInformation) > raw_data->end) {
        return false;
    }

    auto res = (WTPStaticIPAddressInformation *)raw_data->current;
    if (!res->Validate()) {
        return false;
    }
    element = res;
    raw_data->current += sizeof(WTPStaticIPAddressInformation);

    return true;
}

const WTPStaticIPAddressInformation *const ReadableWTPStaticIPAddressInformation::Get() const {
    return element;
}

void ReadableWTPStaticIPAddressInformation::Log() const {
    ASSERT(element != nullptr);
    element->Log();
}

ElementHeader::ElementType ReadableWTPStaticIPAddressInformation::GetElementType() const {
    return ElementHeader::WTPStaticIPAddressInformation;
}