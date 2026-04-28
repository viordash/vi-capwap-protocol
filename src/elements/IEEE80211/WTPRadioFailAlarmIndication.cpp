#include "WTPRadioFailAlarmIndication.h"
#include "Logging.h"
#include "lassert.h"
#include <cstring>

WTPRadioFailAlarmIndication::WTPRadioFailAlarmIndication(uint8_t radio_id,
                                                         AlarmType type,
                                                         AlarmStatus status)
    : ElementHeader(ElementHeader::WTPRadioFailAlarmIndication,
                    sizeof(WTPRadioFailAlarmIndication) - sizeof(ElementHeader)),
      RadioID{ radio_id }, Type{ type }, Status{ status }, Pad{ 0 } {
}

bool WTPRadioFailAlarmIndication::Validate() const {
    static_assert(sizeof(WTPRadioFailAlarmIndication) == 8);
    if (ElementHeader::GetElementType() != ElementHeader::WTPRadioFailAlarmIndication) {
        return false;
    }
    if (ElementHeader::GetLength()
        != (sizeof(WTPRadioFailAlarmIndication) - sizeof(ElementHeader))) {
        return false;
    }
    if (RadioID > 31) {
        return false;
    }

    switch (Type) {
        case AlarmType::Receiver:
        case AlarmType::Transmitter:
            break;
        default:
            return false;
    }

    switch (Status) {
        case AlarmStatus::Cleared:
        case AlarmStatus::Minor:
        case AlarmStatus::Major:
        case AlarmStatus::Critical:
            break;
        default:
            return false;
    }
    return true;
}

void WTPRadioFailAlarmIndication::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(WTPRadioFailAlarmIndication) <= raw_data->end);
#pragma GCC diagnostic push
#if __GNUC__ >= 8
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif
    memcpy(raw_data->current, this, sizeof(WTPRadioFailAlarmIndication));
#pragma GCC diagnostic pop
    raw_data->current += sizeof(WTPRadioFailAlarmIndication);
}

WTPRadioFailAlarmIndication *WTPRadioFailAlarmIndication::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(WTPRadioFailAlarmIndication) > raw_data->end) {
        return nullptr;
    }

    auto res = (WTPRadioFailAlarmIndication *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += sizeof(WTPRadioFailAlarmIndication);
    return res;
}

WritableWTPRadioFailAlarmIndicationArray::WritableWTPRadioFailAlarmIndicationArray() {
    items.reserve(ReadableWTPRadioFailAlarmIndicationArray::max_count);
}

void WritableWTPRadioFailAlarmIndicationArray::Add(WTPRadioFailAlarmIndication element) {
    ASSERT(items.size() + 1 <= ReadableWTPRadioFailAlarmIndicationArray::max_count);
    items.emplace_back(std::move(element));
}

bool WritableWTPRadioFailAlarmIndicationArray::Empty() const {
    return items.empty();
}

void WritableWTPRadioFailAlarmIndicationArray::Clear() {
    items.clear();
}

void WritableWTPRadioFailAlarmIndicationArray::Serialize(RawData *raw_data) const {
    for (const auto &elem : items) {
        elem.Serialize(raw_data);
    }
}

void WritableWTPRadioFailAlarmIndicationArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME WTPRadioFailAlarmIndication #%zu RadioID:%u, Type:%u, Status:%u",
              i,
              items[i].RadioID,
              items[i].Type,
              items[i].Status);
    }
}

ReadableWTPRadioFailAlarmIndicationArray::ReadableWTPRadioFailAlarmIndicationArray() : count{ 0 } {
}

bool ReadableWTPRadioFailAlarmIndicationArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableWTPRadioFailAlarmIndicationArray::Deserialize elements count exceeds");
        return false;
    }

    auto rfai = WTPRadioFailAlarmIndication::Deserialize(raw_data);
    if (rfai == nullptr) {
        return false;
    }
    items[count] = rfai;
    count++;
    return true;
}

nonstd::span<const WTPRadioFailAlarmIndication *const>
ReadableWTPRadioFailAlarmIndicationArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableWTPRadioFailAlarmIndicationArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME WTPRadioFailAlarmIndication #%zu RadioID:%u, Type:%u, Status:%u",
              i,
              items[i]->RadioID,
              items[i]->Type,
              items[i]->Status);
    }
}
