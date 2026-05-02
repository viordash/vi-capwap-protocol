#include "WTPMACType.h"
#include "Logging.h"
#include "lassert.h"
#include <cstring>

WTPMACType::WTPMACType(Type type)
    : ElementHeader(ElementHeader::WTPMACType, sizeof(WTPMACType) - sizeof(ElementHeader)),
      type{ type } {
}

bool WTPMACType::Validate() const {
    static_assert(sizeof(WTPMACType) == 5);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
    return ElementHeader::GetElementType() == ElementHeader::WTPMACType
        && ElementHeader::GetLength() == (sizeof(WTPMACType) - sizeof(ElementHeader)) //
        && type >= Local_MAC && type <= Both;
#pragma GCC diagnostic pop
}

void WTPMACType::Log() const {
    log_i("ME WTPMACType Type:%u", (unsigned)type);
}

WritableWTPMACType::WritableWTPMACType(WTPMACType::Type type) : element{ type } {
    static_assert(sizeof(element) == 5);
}

void WritableWTPMACType::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(WTPMACType) <= raw_data->end);
    std::memcpy(raw_data->current, &element, sizeof(element));
    raw_data->current += sizeof(element);
}

uint16_t WritableWTPMACType::GetTotalLength() const {
    return element.GetLength() + sizeof(ElementHeader);
}

void WritableWTPMACType::Log() const {
    element.Log();
}

bool ReadableWTPMACType::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(WTPMACType) > raw_data->end) {
        return false;
    }

    auto res = (WTPMACType *)raw_data->current;
    if (!res->Validate()) {
        return false;
    }
    raw_data->current += sizeof(WTPMACType);

    element = res;
    is_present = true;
    return true;
}

const WTPMACType *const ReadableWTPMACType::Get() const {
    return element;
}

void ReadableWTPMACType::Log() const {
    ASSERT(element != nullptr);
    element->Log();
}

ElementHeader::ElementType ReadableWTPMACType::GetElementType() const {
    return ElementHeader::WTPMACType;
}

bool ReadableWTPMACType::IsPresent() const {
    return is_present;
}