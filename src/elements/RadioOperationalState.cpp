#include "RadioOperationalState.h"
#include "lassert.h"
#include "logging.h"
#include <algorithm>
#include <arpa/inet.h>

RadioOperationalState::RadioOperationalState(const uint8_t radio_id,
                                             const States state,
                                             const Causes cause)
    : ElementHeader(ElementHeader::RadioOperationalState,
                    sizeof(RadioOperationalState) - sizeof(ElementHeader)),
      RadioID{ radio_id }, State{ state }, Cause{ cause } {
}
bool RadioOperationalState::Validate() const {
    static_assert(sizeof(RadioOperationalState) == 7);
    if (ElementHeader::GetElementType() != ElementHeader::RadioOperationalState) {
        return false;
    }
    if (ElementHeader::GetLength() != (sizeof(RadioOperationalState) - sizeof(ElementHeader))) {
        return false;
    }
    if (RadioID > 31) {
        return false;
    }
    switch (State) {
        case RadioOperationalState::States::Reserved:
        case RadioOperationalState::States::Enabled:
        case RadioOperationalState::States::Disabled:
            break;

        default:
            return false;
    }
    switch (Cause) {
        case RadioOperationalState::Causes::Normal:
        case RadioOperationalState::Causes::RadioFailure:
        case RadioOperationalState::Causes::SoftwareFailure:
        case RadioOperationalState::Causes::AdministrativelySet:
            break;

        default:
            return false;
    }

    return true;
}
void RadioOperationalState::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(RadioOperationalState) <= raw_data->end);
    RadioOperationalState *dst = (RadioOperationalState *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(RadioOperationalState);
}
RadioOperationalState *RadioOperationalState::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(RadioOperationalState) > raw_data->end) {
        return nullptr;
    }

    auto res = (RadioOperationalState *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += sizeof(RadioOperationalState);
    return res;
}

WritableRadioOperationalStateArray::WritableRadioOperationalStateArray() {
    items.reserve(ReadableRadioOperationalStateArray::max_count);
}

void WritableRadioOperationalStateArray::Add(RadioOperationalState radio_state) {
    ASSERT(items.size() + 1 <= ReadableRadioOperationalStateArray::max_count);

    auto it_exists =
        std::find_if(items.begin(), items.end(), [&radio_state](const RadioOperationalState &item) {
            return item.RadioID == radio_state.RadioID;
        });
    if (it_exists != items.end()) {
        *it_exists = std::move(radio_state);
        log_i("RadioOperationalState: replace RadioID: %u", (*it_exists).RadioID);
    } else {
        items.emplace_back(std::move(radio_state));
    }
}

bool WritableRadioOperationalStateArray::Empty() const {
    return items.empty();
}

void WritableRadioOperationalStateArray::Clear() {
    items.clear();
}

void WritableRadioOperationalStateArray::Serialize(RawData *raw_data) const {
    for (const auto &elem : items) {
        elem.Serialize(raw_data);
    }
}

void WritableRadioOperationalStateArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME RadioOperationalState #%lu RadioID:%u, State:%u, Cause:%u",
              i,
              items[i].RadioID,
              items[i].State,
              items[i].Cause);
    }
}

ReadableRadioOperationalStateArray::ReadableRadioOperationalStateArray() : count{ 0 } {
}

bool ReadableRadioOperationalStateArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableRadioOperationalStateArray::Deserialize elements count exceeds");
        return false;
    }

    auto ip_address = RadioOperationalState::Deserialize(raw_data);
    if (ip_address == nullptr) {
        return false;
    }
    items[count] = ip_address;
    count++;
    return true;
}

nonstd::span<const RadioOperationalState *const> ReadableRadioOperationalStateArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableRadioOperationalStateArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME RadioOperationalState #%lu RadioID:%u, State:%u, Cause:%u",
              i,
              items[i]->RadioID,
              items[i]->State,
              items[i]->Cause);
    }
}