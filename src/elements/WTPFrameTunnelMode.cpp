#include "WTPFrameTunnelMode.h"
#include "Logging.h"
#include "lassert.h"
#include <cstring>

WTPFrameTunnelMode::WTPFrameTunnelMode(bool l, bool e, bool n)
    : ElementHeader(ElementHeader::WTPFrameTunnelMode,
                    sizeof(WTPFrameTunnelMode) - sizeof(ElementHeader)),
#if VI_CAPWAP_BIG_ENDIAN
      Reservd{ 0 }, N{ n }, E{ e }, L{ l }, U{ 0 } {
#else
      U{ 0 }, L{ l }, E{ e }, N{ n }, Reservd{ 0 } {
#endif
}

bool WTPFrameTunnelMode::Validate() const {
    static_assert(sizeof(WTPFrameTunnelMode) == 5);
    return ElementHeader::GetElementType() == ElementHeader::WTPFrameTunnelMode
        && ElementHeader::GetLength() == (sizeof(WTPFrameTunnelMode) - sizeof(ElementHeader)) //
        && Reservd == 0 && U == 0;
}

uint16_t WTPFrameTunnelMode::GetTotalLength() const {
    return GetLength() + sizeof(ElementHeader);
}

void WTPFrameTunnelMode::Log() const {
    log_i("ME WTPFrameTunnelMode L:{}, E:{}, N:{}", L, E, N);
}

WritableWTPFrameTunnelMode::WritableWTPFrameTunnelMode(bool l, bool e, bool n)
    : element{ l, e, n } {
    static_assert(sizeof(element) == 5);
}

void WritableWTPFrameTunnelMode::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(WTPFrameTunnelMode) <= raw_data->end);
    std::memcpy(raw_data->current, &element, sizeof(element));
    raw_data->current += sizeof(element);
}

uint16_t WritableWTPFrameTunnelMode::GetTotalLength() const {
    return element.GetTotalLength();
}

void WritableWTPFrameTunnelMode::Log() const {
    element.Log();
}

bool ReadableWTPFrameTunnelMode::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(WTPFrameTunnelMode) > raw_data->end) {
        return false;
    }

    auto res = (WTPFrameTunnelMode *)raw_data->current;
    if (!res->Validate()) {
        return false;
    }
    raw_data->current += sizeof(WTPFrameTunnelMode);

    element = res;
    is_present = true;
    return true;
}

const WTPFrameTunnelMode *ReadableWTPFrameTunnelMode::Get() const {
    return element;
}

void ReadableWTPFrameTunnelMode::Log() const {
    ASSERT(element != nullptr);
    element->Log();
}

ElementHeader::ElementType ReadableWTPFrameTunnelMode::GetElementType() const {
    return ElementHeader::WTPFrameTunnelMode;
}

bool ReadableWTPFrameTunnelMode::IsPresent() const {
    return is_present;
}