#include "WTPFrameTunnelMode.h"
#include "lassert.h"
#include "logging.h"

WTPFrameTunnelMode::WTPFrameTunnelMode(bool l, bool e, bool n)
    : ElementHeader(ElementHeader::WTPFrameTunnelMode,
                    sizeof(WTPFrameTunnelMode) - sizeof(ElementHeader)),
      U{ 0 }, L{ l }, E{ e }, N{ n }, Reservd{ 0 } {
}
bool WTPFrameTunnelMode::Validate() const {
    static_assert(sizeof(WTPFrameTunnelMode) == 5);
    return ElementHeader::GetElementType() == ElementHeader::WTPFrameTunnelMode
        && ElementHeader::GetLength() == (sizeof(WTPFrameTunnelMode) - sizeof(ElementHeader)) //
        && Reservd == 0 && U == 0;
}
void WTPFrameTunnelMode::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(WTPFrameTunnelMode) <= raw_data->end);
    WTPFrameTunnelMode *dst = (WTPFrameTunnelMode *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(WTPFrameTunnelMode);
}
WTPFrameTunnelMode *WTPFrameTunnelMode::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(WTPFrameTunnelMode) > raw_data->end) {
        return nullptr;
    }

    auto res = (WTPFrameTunnelMode *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += sizeof(WTPFrameTunnelMode);
    return res;
}
uint16_t WTPFrameTunnelMode::GetTotalLength() const {
    return GetLength() + sizeof(ElementHeader);
}

void WTPFrameTunnelMode::Log() const {
    log_i("ME WTPFrameTunnelMode L:%u, E:%u, N:%u", L, E, N);
}