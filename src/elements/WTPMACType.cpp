#include "WTPMACType.h"
#include "lassert.h"
#include "logging.h"

WTPMACType::WTPMACType(Type type)
    : ElementHeader(ElementHeader::WTPMACType, sizeof(WTPMACType) - sizeof(ElementHeader)),
      type{ type } {
}
bool WTPMACType::Validate() const {
    static_assert(sizeof(WTPMACType) == 5);
    return ElementHeader::GetElementType() == ElementHeader::WTPMACType
        && ElementHeader::GetLength() == (sizeof(WTPMACType) - sizeof(ElementHeader)) //
        && type >= Local_MAC && type <= Both;
}
void WTPMACType::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(WTPMACType) <= raw_data->end);
    WTPMACType *dst = (WTPMACType *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(WTPMACType);
}
WTPMACType *WTPMACType::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(WTPMACType) > raw_data->end) {
        return nullptr;
    }

    auto res = (WTPMACType *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += sizeof(WTPMACType);
    return res;
}
uint16_t WTPMACType::GetTotalLength() const {
    return GetLength() + sizeof(ElementHeader);
}

void WTPMACType::Log() const {
    log_i("ME WTPMACType Type:%u", (unsigned)type);
}