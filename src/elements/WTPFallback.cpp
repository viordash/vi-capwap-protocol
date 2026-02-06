#include "WTPFallback.h"
#include "lassert.h"
#include "logging.h"
#include <cstring>

WTPFallback::WTPFallback(Mode mode)
    : ElementHeader(ElementHeader::WTPFallback, sizeof(WTPFallback) - sizeof(ElementHeader)),
      mode{ mode } {
}
bool WTPFallback::Validate() const {
    static_assert(sizeof(WTPFallback) == 5);
    return ElementHeader::GetElementType() == ElementHeader::WTPFallback
        && ElementHeader::GetLength() == (sizeof(WTPFallback) - sizeof(ElementHeader)) //
        && mode >= Reserved && mode <= Disabled;
}
void WTPFallback::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(WTPFallback) <= raw_data->end);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
    memcpy(raw_data->current, this, sizeof(WTPFallback));
#pragma GCC diagnostic pop
    raw_data->current += sizeof(WTPFallback);
}
WTPFallback *WTPFallback::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(WTPFallback) > raw_data->end) {
        return nullptr;
    }

    auto res = (WTPFallback *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += sizeof(WTPFallback);
    return res;
}
uint16_t WTPFallback::GetTotalLength() const {
    return GetLength() + sizeof(ElementHeader);
}

void WTPFallback::Log() const {
    log_i("ME WTPFallback Mode:%u", (unsigned)mode);
}