
#include "ACTimestamp.h"
#include "Logging.h"
#include "lassert.h"
#include <cstring>
#include <string.h>

ACTimestamp::ACTimestamp(uint32_t timestamp)
    : ElementHeader(ElementHeader::ACTimestamp, (sizeof(ACTimestamp) - sizeof(ElementHeader))),
      timestamp{ timestamp } {
}

uint32_t ACTimestamp::GetTimestamp() const {
    return timestamp.Get();
}

bool ACTimestamp::Validate() const {
    static_assert(sizeof(ACTimestamp) == 8);
    return GetElementType() == ElementHeader::ACTimestamp
        && GetLength() == (sizeof(ACTimestamp) - sizeof(ElementHeader));
}

void ACTimestamp::Log() const {
    log_i("ME ACTimestamp timestamp: %u", GetTimestamp());
}

WritableACTimestamp::WritableACTimestamp(uint32_t timestamp) : element{ timestamp } {
    static_assert(sizeof(element) == 8);
}

void WritableACTimestamp::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(ACTimestamp) <= raw_data->end);
    std::memcpy(raw_data->current, &element, sizeof(element));
    raw_data->current += sizeof(element);
}

void WritableACTimestamp::Log() const {
    element.Log();
}

bool ReadableACTimestamp::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ACTimestamp) > raw_data->end) {
        return false;
    }

    auto res = (ACTimestamp *)raw_data->current;
    if (!res->Validate()) {
        return false;
    }
    raw_data->current += sizeof(ACTimestamp);

    element = res;
    is_present = true;
    return true;
}

const ACTimestamp *const ReadableACTimestamp::Get() const {
    return element;
}

void ReadableACTimestamp::Log() const {
    ASSERT(element != nullptr);
    element->Log();
}

ElementHeader::ElementType ReadableACTimestamp::GetElementType() const {
    return ElementHeader::ACTimestamp;
}

bool ReadableACTimestamp::IsPresent() const {
    return is_present;
}