#include "CAPWAPTimers.h"
#include "lassert.h"
#include "logging.h"
#include <string.h>

CAPWAPTimers::CAPWAPTimers(uint8_t discovery, uint8_t echo_interval)
    : ElementHeader(ElementHeader::CAPWAPTimers, sizeof(CAPWAPTimers) - sizeof(ElementHeader)),
      Discovery{ discovery }, EchoInterval{ echo_interval } {
}
bool CAPWAPTimers::Validate() const {
    static_assert(sizeof(CAPWAPTimers) == 6);
    return ElementHeader::GetElementType() == ElementHeader::CAPWAPTimers
        && ElementHeader::GetLength() == (sizeof(CAPWAPTimers) - sizeof(ElementHeader));
}
void CAPWAPTimers::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(CAPWAPTimers) <= raw_data->end);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
    memcpy(raw_data->current, this, sizeof(CAPWAPTimers));
#pragma GCC diagnostic pop
    raw_data->current += sizeof(CAPWAPTimers);
}
CAPWAPTimers *CAPWAPTimers::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(CAPWAPTimers) > raw_data->end) {
        return nullptr;
    }

    auto res = (CAPWAPTimers *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += sizeof(CAPWAPTimers);
    return res;
}
uint16_t CAPWAPTimers::GetTotalLength() const {
    return GetLength() + sizeof(ElementHeader);
}

void CAPWAPTimers::Log() const {
    log_i("ME CAPWAPTimers Discovery:%u, Echo interval:%u", Discovery, EchoInterval);
}