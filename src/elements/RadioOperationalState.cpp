#include "RadioOperationalState.h"
#include "Logging.h"
#include "lassert.h"
#include <algorithm>
#include <arpa/inet.h>
#include <cstring>

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
        ASSERT(raw_data->current + sizeof(RadioOperationalState) <= raw_data->end);
#pragma GCC diagnostic push
#if __GNUC__ >= 8
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif
        memcpy(raw_data->current, &elem, sizeof(RadioOperationalState));
#pragma GCC diagnostic pop
        raw_data->current += sizeof(RadioOperationalState);
    }
}

void WritableRadioOperationalStateArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME RadioOperationalState #%zu RadioID:%u, State:%u, Cause:%u",
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

    if (raw_data->current + sizeof(RadioOperationalState) > raw_data->end) {
        return false;
    }

    auto element = (RadioOperationalState *)raw_data->current;
    if (!element->Validate()) {
        return false;
    }
    raw_data->current += sizeof(RadioOperationalState);

    items[count] = element;
    count++;
    return true;
}

nonstd::span<const RadioOperationalState *const> ReadableRadioOperationalStateArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableRadioOperationalStateArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME RadioOperationalState #%zu RadioID:%u, State:%u, Cause:%u",
              i,
              items[i]->RadioID,
              items[i]->State,
              items[i]->Cause);
    }
}

ElementHeader::ElementType ReadableRadioOperationalStateArray::GetElementType() const {
    return ElementHeader::RadioOperationalState;
}

bool ReadableRadioOperationalStateArray::IsPresent() const {
    return count > 0;
}