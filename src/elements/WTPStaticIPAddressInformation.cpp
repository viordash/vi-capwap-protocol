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
    log_i("ME WTPStaticIPAddressInformation IP Address:{}, Netmask:{}, Gateway:{}, use static:{}",
          IpToString(IpAddress),
          IpToString(Netmask),
          IpToString(Gateway),
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
    std::memcpy(raw_data->current, &element, sizeof(element));
    raw_data->current += sizeof(element);
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
    raw_data->current += sizeof(WTPStaticIPAddressInformation);

    element = res;
    is_present = true;
    return true;
}

const WTPStaticIPAddressInformation *ReadableWTPStaticIPAddressInformation::Get() const {
    return element;
}

void ReadableWTPStaticIPAddressInformation::Log() const {
    ASSERT(element != nullptr);
    element->Log();
}

ElementHeader::ElementType ReadableWTPStaticIPAddressInformation::GetElementType() const {
    return ElementHeader::WTPStaticIPAddressInformation;
}

bool ReadableWTPStaticIPAddressInformation::IsPresent() const {
    return is_present;
}