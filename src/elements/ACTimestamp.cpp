
#include "ACTimestamp.h"
#include "lassert.h"
#include "logging.h"
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

void ACTimestamp::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(ACTimestamp) <= raw_data->end);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
    memcpy(raw_data->current, this, sizeof(ACTimestamp));
#pragma GCC diagnostic pop
    raw_data->current += sizeof(ACTimestamp);
}

ACTimestamp *ACTimestamp::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ACTimestamp) > raw_data->end) {
        return nullptr;
    }

    auto res = (ACTimestamp *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }

    uint8_t *last = raw_data->current + sizeof(ElementHeader) + res->GetLength();
    if (last > raw_data->end) {
        return nullptr;
    }

    raw_data->current = last;
    return res;
}

void ACTimestamp::Log() const {
    log_i("ME ACTimestamp timestamp: %u", GetTimestamp());
}
