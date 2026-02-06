#include "MTUDiscoveryPadding.h"
#include "lassert.h"
#include "logging.h"
#include <string.h>

MTUDiscoveryPadding::MTUDiscoveryPadding(uint16_t size)
    : ElementHeader(ElementHeader::MTUDiscoveryPadding, size) {
}
bool MTUDiscoveryPadding::Validate() const {
    static_assert(sizeof(MTUDiscoveryPadding) == 4);
    return ElementHeader::GetElementType() == ElementHeader::MTUDiscoveryPadding;
}
void MTUDiscoveryPadding::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(ElementHeader) + GetLength() <= raw_data->end);
    ElementHeader *dst = (ElementHeader *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(ElementHeader);
    memset(raw_data->current, 0xFF, GetLength());
    raw_data->current += GetLength();
}
MTUDiscoveryPadding *MTUDiscoveryPadding::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ElementHeader) > raw_data->end) {
        return nullptr;
    }

    auto res = (MTUDiscoveryPadding *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    if (raw_data->current + sizeof(ElementHeader) + res->GetLength() > raw_data->end) {
        return nullptr;
    }

#ifdef VALIDATE_MTUDiscoveryPadding
    for (size_t i = sizeof(ElementHeader); i < res->GetLength() + sizeof(ElementHeader); i++) {
        if (raw_data->current[i] != 0xFF) {
            return nullptr;
        }
    }
#endif // VALIDATE_MTUDiscoveryPadding
    raw_data->current += sizeof(ElementHeader) + res->GetLength();
    return res;
}
uint16_t MTUDiscoveryPadding::GetTotalLength() const {
    return GetLength() + sizeof(ElementHeader);
}

void MTUDiscoveryPadding::Log() const {
    log_i("ME MTUDiscoveryPadding Padding:%u", GetLength());
}