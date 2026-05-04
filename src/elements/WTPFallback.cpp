#include "WTPFallback.h"
#include "Logging.h"
#include "lassert.h"
#include <cstring>

WTPFallback::WTPFallback(Mode mode)
    : ElementHeader(ElementHeader::WTPFallback, sizeof(WTPFallback) - sizeof(ElementHeader)),
      mode{ mode } {
}

bool WTPFallback::Validate() const {
    static_assert(sizeof(WTPFallback) == 5);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
    return ElementHeader::GetElementType() == ElementHeader::WTPFallback
        && ElementHeader::GetLength() == (sizeof(WTPFallback) - sizeof(ElementHeader)) //
        && mode >= Reserved && mode <= Disabled;
#pragma GCC diagnostic pop
}

void WTPFallback::Log() const {
    log_i("ME WTPFallback Mode:%u", (unsigned)mode);
}

WritableWTPFallback::WritableWTPFallback(WTPFallback::Mode mode) : element{ mode } {
    static_assert(sizeof(element) == 5);
}

void WritableWTPFallback::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(WTPFallback) <= raw_data->end);
    std::memcpy(raw_data->current, &element, sizeof(element));
    raw_data->current += sizeof(element);
}

void WritableWTPFallback::Log() const {
    element.Log();
}

bool ReadableWTPFallback::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(WTPFallback) > raw_data->end) {
        return false;
    }

    auto res = (WTPFallback *)raw_data->current;
    if (!res->Validate()) {
        return false;
    }
    raw_data->current += sizeof(WTPFallback);

    element = res;
    is_present = true;
    return true;
}

const WTPFallback *ReadableWTPFallback::Get() const {
    return element;
}

void ReadableWTPFallback::Log() const {
    ASSERT(element != nullptr);
    element->Log();
}

ElementHeader::ElementType ReadableWTPFallback::GetElementType() const {
    return ElementHeader::WTPFallback;
}

bool ReadableWTPFallback::IsPresent() const {
    return is_present;
}