#include "CAPWAPTimers.h"
#include "Logging.h"
#include "lassert.h"
#include <cstring>
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

void CAPWAPTimers::Log() const {
    log_i("ME CAPWAPTimers Discovery:{}, Echo interval:{}", Discovery, EchoInterval);
}

WritableCAPWAPTimers::WritableCAPWAPTimers(uint8_t discovery, uint8_t echo_interval)
    : element{ discovery, echo_interval } {
    static_assert(sizeof(element) == 6);
}

void WritableCAPWAPTimers::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(CAPWAPTimers) <= raw_data->end);
    std::memcpy(raw_data->current, &element, sizeof(element));
    raw_data->current += sizeof(element);
}

void WritableCAPWAPTimers::Log() const {
    element.Log();
}

bool ReadableCAPWAPTimers::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(CAPWAPTimers) > raw_data->end) {
        return false;
    }

    auto res = (CAPWAPTimers *)raw_data->current;
    if (!res->Validate()) {
        return false;
    }
    raw_data->current += sizeof(CAPWAPTimers);

    element = res;
    is_present = true;
    return true;
}

const CAPWAPTimers *ReadableCAPWAPTimers::Get() const {
    return element;
}

void ReadableCAPWAPTimers::Log() const {
    ASSERT(element != nullptr);
    element->Log();
}

ElementHeader::ElementType ReadableCAPWAPTimers::GetElementType() const {
    return ElementHeader::CAPWAPTimers;
}

bool ReadableCAPWAPTimers::IsPresent() const {
    return is_present;
}