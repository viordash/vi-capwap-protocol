
#include "IdleTimeout.h"
#include "Logging.h"
#include "lassert.h"
#include <cstring>
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

void IdleTimeout::Log() const {
    log_i("ME IdleTimeout timeout: %u", GetTimeout());
}

WritableIdleTimeout::WritableIdleTimeout(uint32_t timeout) : element{ timeout } {
    static_assert(sizeof(element) == 8);
}

void WritableIdleTimeout::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(IdleTimeout) <= raw_data->end);
    std::memcpy(raw_data->current, &element, sizeof(element));
    raw_data->current += sizeof(element);
}

void WritableIdleTimeout::Log() const {
    element.Log();
}

bool ReadableIdleTimeout::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(IdleTimeout) > raw_data->end) {
        return false;
    }

    auto res = (IdleTimeout *)raw_data->current;
    if (!res->Validate()) {
        return false;
    }
    raw_data->current += sizeof(IdleTimeout);

    element = res;
    is_present = true;
    return true;
}

IdleTimeout *const ReadableIdleTimeout::Get() const {
    return element;
}

void ReadableIdleTimeout::Log() const {
    ASSERT(element != nullptr);
    element->Log();
}

ElementHeader::ElementType ReadableIdleTimeout::GetElementType() const {
    return ElementHeader::IdleTimeout;
}

bool ReadableIdleTimeout::IsPresent() const {
    return is_present;
}