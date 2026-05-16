#include "DiscoveryType.h"
#include "Logging.h"
#include "lassert.h"
#include <cstring>

DiscoveryType::DiscoveryType(Type type)
    : ElementHeader(ElementHeader::DiscoveryType, sizeof(DiscoveryType) - sizeof(ElementHeader)),
      type{ type } {
}

bool DiscoveryType::Validate() const {
    static_assert(sizeof(DiscoveryType) == 5);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
    return GetElementType() == ElementHeader::DiscoveryType
        && GetLength() == (sizeof(DiscoveryType) - sizeof(ElementHeader)) //
        && type >= Unknown && type <= ACReferral;
#pragma GCC diagnostic pop
}

void DiscoveryType::Log() const {
    log_i("ME DiscoveryType Type:{}", (unsigned)type);
}

WritableDiscoveryType::WritableDiscoveryType(DiscoveryType::Type type) : element{ type } {
    static_assert(sizeof(element) == 5);
}

void WritableDiscoveryType::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(DiscoveryType) <= raw_data->end);
    std::memcpy(raw_data->current, &element, sizeof(element));
    raw_data->current += sizeof(element);
}

uint16_t WritableDiscoveryType::GetTotalLength() const {
    return element.GetLength() + sizeof(ElementHeader);
}

void WritableDiscoveryType::Log() const {
    element.Log();
}

bool ReadableDiscoveryType::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(DiscoveryType) > raw_data->end) {
        return false;
    }

    auto res = (DiscoveryType *)raw_data->current;
    if (!res->Validate()) {
        return false;
    }
    raw_data->current += sizeof(DiscoveryType);

    element = res;
    is_present = true;
    return true;
}

const DiscoveryType *ReadableDiscoveryType::Get() const {
    return element;
}

void ReadableDiscoveryType::Log() const {
    ASSERT(element != nullptr);
    element->Log();
}

ElementHeader::ElementType ReadableDiscoveryType::GetElementType() const {
    return ElementHeader::DiscoveryType;
}

bool ReadableDiscoveryType::IsPresent() const {
    return is_present;
}