#include "WTPRadioFailAlarmIndication.h"
#include "Logging.h"
#include "lassert.h"
#include <algorithm>
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

WritableWTPRadioFailAlarmIndicationArray::WritableWTPRadioFailAlarmIndicationArray() {
    static_assert(sizeof(items[0]) == 8);
    items.reserve(ReadableWTPRadioFailAlarmIndicationArray::max_count);
}

void WritableWTPRadioFailAlarmIndicationArray::Add(WTPRadioFailAlarmIndication element) {
    ASSERT(items.size() + 1 <= ReadableWTPRadioFailAlarmIndicationArray::max_count);

    auto it_exists = std::find_if(items.begin(),
                                  items.end(),
                                  [&element](const WTPRadioFailAlarmIndication &item) {
                                      return item.RadioID == element.RadioID;
                                  });

    if (it_exists != items.end()) {
        *it_exists = std::move(element);
        log_i("WTPRadioFailAlarmIndication: replace RadioID: %u", (*it_exists).RadioID);
    } else {
        items.emplace_back(std::move(element));
    }
}

bool WritableWTPRadioFailAlarmIndicationArray::Empty() const {
    return items.empty();
}

void WritableWTPRadioFailAlarmIndicationArray::Clear() {
    items.clear();
}

void WritableWTPRadioFailAlarmIndicationArray::Serialize(RawData *raw_data) const {
    for (const auto &item : items) {
        ASSERT(raw_data->current + sizeof(item) <= raw_data->end);
        std::memcpy(raw_data->current, &item, sizeof(item));
        raw_data->current += sizeof(item);
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

ElementHeader::ElementType WritableWTPRadioFailAlarmIndicationArray::GetElementType() const {
    return ElementHeader::WTPRadioFailAlarmIndication;
}

ReadableWTPRadioFailAlarmIndicationArray::ReadableWTPRadioFailAlarmIndicationArray() : count{ 0 } {
}

ElementHeader::ElementType ReadableWTPRadioFailAlarmIndicationArray::GetElementType() const {
    return ElementHeader::WTPRadioFailAlarmIndication;
}

bool ReadableWTPRadioFailAlarmIndicationArray::IsPresent() const {
    return count > 0;
}

bool ReadableWTPRadioFailAlarmIndicationArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableWTPRadioFailAlarmIndicationArray::Deserialize elements count exceeds");
        return false;
    }

    if (raw_data->current + sizeof(WTPRadioFailAlarmIndication) > raw_data->end) {
        return false;
    }

    auto item = (WTPRadioFailAlarmIndication *)raw_data->current;
    if (!item->Validate()) {
        return false;
    }

    raw_data->current += sizeof(WTPRadioFailAlarmIndication);
    items[count] = item;
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
