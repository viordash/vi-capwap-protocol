#include "DirectSequenceControl.h"
#include "Logging.h"
#include "lassert.h"
#include <algorithm>
#include <cstring>

DirectSequenceControl::DirectSequenceControl(uint8_t radio_id,
                                             uint8_t current_channel,
                                             CCAMode current_cca,
                                             uint32_t energy_detect_threshold)
    : ElementHeader(ElementHeader::DirectSequenceControl,
                    sizeof(DirectSequenceControl) - sizeof(ElementHeader)),
      radio_id{ radio_id }, reserved{ 0 }, current_channel{ current_channel },
      current_cca{ current_cca }, energy_detect_threshold{ energy_detect_threshold } {
}

uint8_t DirectSequenceControl::GetRadioID() const {
    return radio_id;
}

uint8_t DirectSequenceControl::GetCurrentChannel() const {
    return current_channel;
}

DirectSequenceControl::CCAMode DirectSequenceControl::GetCurrentCCA() const {
    return current_cca;
}

uint32_t DirectSequenceControl::GetEnergyDetectThreshold() const {
    return energy_detect_threshold.Get();
}

bool DirectSequenceControl::Validate() const {
    static_assert(sizeof(DirectSequenceControl) == 12);
    if (ElementHeader::GetElementType() != ElementHeader::DirectSequenceControl) {
        return false;
    }
    if (ElementHeader::GetLength() != (sizeof(DirectSequenceControl) - sizeof(ElementHeader))) {
        return false;
    }
    if (radio_id > 31) {
        return false;
    }
    switch (current_cca) {
        case CCAMode::EdOnly:
        case CCAMode::CsOnly:
        case CCAMode::EdAndCs:
        case CCAMode::CsWithTimer:
        case CCAMode::HrCsAndEd:
            break;
        default:
            return false;
    }
    return true;
}

WritableDirectSequenceControlArray::WritableDirectSequenceControlArray() {
    static_assert(sizeof(items[0]) == 12);
    items.reserve(ReadableDirectSequenceControlArray::max_count);
}

void WritableDirectSequenceControlArray::Add(DirectSequenceControl ctrl) {
    ASSERT(items.size() + 1 <= ReadableDirectSequenceControlArray::max_count);

    auto it_exists =
        std::find_if(items.begin(), items.end(), [&ctrl](const DirectSequenceControl &item) {
            return item.GetRadioID() == ctrl.GetRadioID();
        });
    if (it_exists != items.end()) {
        *it_exists = std::move(ctrl);
        log_i("DirectSequenceControl: replace RadioID: %u", (*it_exists).GetRadioID());
    } else {
        items.emplace_back(std::move(ctrl));
    }
}

bool WritableDirectSequenceControlArray::Empty() const {
    return items.empty();
}

void WritableDirectSequenceControlArray::Clear() {
    items.clear();
}

void WritableDirectSequenceControlArray::Serialize(RawData *raw_data) const {
    for (const auto &item : items) {
        ASSERT(raw_data->current + sizeof(item) <= raw_data->end);
        std::memcpy(raw_data->current, &item, sizeof(item));
        raw_data->current += sizeof(item);
    }
}

void WritableDirectSequenceControlArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME DirectSequenceControl #%zu RadioID:%u, CurrentChannel:%u, CurrentCCA:%u, "
              "EnergyDetectThreshold:%u",
              i,
              items[i].GetRadioID(),
              items[i].GetCurrentChannel(),
              items[i].GetCurrentCCA(),
              items[i].GetEnergyDetectThreshold());
    }
}

ReadableDirectSequenceControlArray::ReadableDirectSequenceControlArray() : count{ 0 } {
}

ElementHeader::ElementType ReadableDirectSequenceControlArray::GetElementType() const {
    return ElementHeader::DirectSequenceControl;
}

bool ReadableDirectSequenceControlArray::IsPresent() const {
    return count > 0;
}

bool ReadableDirectSequenceControlArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableDirectSequenceControlArray::Deserialize elements count exceeds");
        return false;
    }

    if (raw_data->current + sizeof(DirectSequenceControl) > raw_data->end) {
        return false;
    }

    auto item = (DirectSequenceControl *)raw_data->current;
    if (!item->Validate()) {
        return false;
    }

    raw_data->current += sizeof(DirectSequenceControl);
    items[count] = item;
    count++;
    return true;
}

nonstd::span<const DirectSequenceControl *const> ReadableDirectSequenceControlArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableDirectSequenceControlArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME DirectSequenceControl #%zu RadioID:%u, CurrentChannel:%u, CurrentCCA:%u, "
              "EnergyDetectThreshold:%u",
              i,
              items[i]->GetRadioID(),
              items[i]->GetCurrentChannel(),
              items[i]->GetCurrentCCA(),
              items[i]->GetEnergyDetectThreshold());
    }
}
