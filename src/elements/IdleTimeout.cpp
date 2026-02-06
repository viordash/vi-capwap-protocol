
#include "IdleTimeout.h"
#include "lassert.h"
#include "logging.h"
#include <string.h>

IdleTimeout::IdleTimeout(uint32_t timeout)
    : ElementHeader(ElementHeader::IdleTimeout, (sizeof(IdleTimeout) - sizeof(ElementHeader))),
      timeout{ timeout } {
}

uint32_t IdleTimeout::GetTimeout() const {
    return timeout.Get();
}

bool IdleTimeout::Validate() const {
    static_assert(sizeof(IdleTimeout) == 8);
    return GetElementType() == ElementHeader::IdleTimeout
        && GetLength() == (sizeof(IdleTimeout) - sizeof(ElementHeader));
}

uint16_t IdleTimeout::GetTotalLength() const {
    return GetLength() + sizeof(ElementHeader);
}

void IdleTimeout::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(IdleTimeout) <= raw_data->end);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
    memcpy(raw_data->current, this, sizeof(IdleTimeout));
#pragma GCC diagnostic pop
    raw_data->current += sizeof(IdleTimeout);
}

IdleTimeout *IdleTimeout::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(IdleTimeout) > raw_data->end) {
        return nullptr;
    }

    auto res = (IdleTimeout *)raw_data->current;
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

void IdleTimeout::Log() const {
    log_i("ME IdleTimeout timeout: %u", GetTimeout());
}
