#include "DiscoveryType.h"
#include "lassert.h"
#include "logging.h"

DiscoveryType::DiscoveryType(Type type)
    : ElementHeader(ElementHeader::DiscoveryType, sizeof(DiscoveryType) - sizeof(ElementHeader)),
      type{ type } {
}
bool DiscoveryType::Validate() const {
    static_assert(sizeof(DiscoveryType) == 5);
    return ElementHeader::GetElementType() == ElementHeader::DiscoveryType
        && ElementHeader::GetLength() == (sizeof(DiscoveryType) - sizeof(ElementHeader)) //
        && type >= Unknown && type <= ACReferral;
}
void DiscoveryType::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(DiscoveryType) <= raw_data->end);
    DiscoveryType *dst = (DiscoveryType *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(DiscoveryType);
}
DiscoveryType *DiscoveryType::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(DiscoveryType) > raw_data->end) {
        return nullptr;
    }

    auto res = (DiscoveryType *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += sizeof(DiscoveryType);
    return res;
}
uint16_t DiscoveryType::GetTotalLength() const {
    return GetLength() + sizeof(ElementHeader);
}

void DiscoveryType::Log() const {
    log_i("ME DiscoveryType Type:%u", (unsigned)type);
}