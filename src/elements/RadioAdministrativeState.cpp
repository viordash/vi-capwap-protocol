#include "RadioAdministrativeState.h"
#include "lassert.h"
#include "logging.h"
#include <algorithm>
#include <arpa/inet.h>
#include <cstring>

RadioAdministrativeState::RadioAdministrativeState(const uint8_t radio_id, const States admin_state)
    : ElementHeader(ElementHeader::RadioAdministrativeState,
                    sizeof(RadioAdministrativeState) - sizeof(ElementHeader)),
      RadioID{ radio_id }, AdminState{ admin_state } {
}
bool RadioAdministrativeState::Validate() const {
    static_assert(sizeof(RadioAdministrativeState) == 6);
    if (ElementHeader::GetElementType() != ElementHeader::RadioAdministrativeState) {
        return false;
    }
    if (ElementHeader::GetLength() != (sizeof(RadioAdministrativeState) - sizeof(ElementHeader))) {
        return false;
    }
    if (RadioID > 31 && RadioID != 0xFF) {
        return false;
    }
    switch (AdminState) {
        case RadioAdministrativeState::States::Reserved:
        case RadioAdministrativeState::States::Enabled:
        case RadioAdministrativeState::States::Disabled:
            break;

        default:
            return false;
    }
    return true;
}
void RadioAdministrativeState::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(RadioAdministrativeState) <= raw_data->end);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
    memcpy(raw_data->current, this, sizeof(RadioAdministrativeState));
#pragma GCC diagnostic pop
    raw_data->current += sizeof(RadioAdministrativeState);
}
RadioAdministrativeState *RadioAdministrativeState::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(RadioAdministrativeState) > raw_data->end) {
        return nullptr;
    }

    auto res = (RadioAdministrativeState *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += sizeof(RadioAdministrativeState);
    return res;
}

WritableRadioAdministrativeStateArray::WritableRadioAdministrativeStateArray() {
    items.reserve(ReadableRadioAdministrativeStateArray::max_count);
}

void WritableRadioAdministrativeStateArray::Add(RadioAdministrativeState radio_state) {
    ASSERT(items.size() + 1 <= ReadableRadioAdministrativeStateArray::max_count);

    auto it_exists = std::find_if(items.begin(),
                                  items.end(),
                                  [&radio_state](const RadioAdministrativeState &item) {
                                      return item.RadioID == radio_state.RadioID;
                                  });
    if (it_exists != items.end()) {
        *it_exists = std::move(radio_state);
        log_i("RadioAdministrativeState: replace RadioID: %u", (*it_exists).RadioID);
    } else {
        items.emplace_back(std::move(radio_state));
    }
}

bool WritableRadioAdministrativeStateArray::Empty() const {
    return items.empty();
}

void WritableRadioAdministrativeStateArray::Clear() {
    items.clear();
}

void WritableRadioAdministrativeStateArray::Serialize(RawData *raw_data) const {
    for (const auto &elem : items) {
        elem.Serialize(raw_data);
    }
}

void WritableRadioAdministrativeStateArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME RadioAdministrativeState #%lu RadioID:%u, AdminState:%u",
              i,
              items[i].RadioID,
              items[i].AdminState);
    }
}

ReadableRadioAdministrativeStateArray::ReadableRadioAdministrativeStateArray() : count{ 0 } {
}

bool ReadableRadioAdministrativeStateArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableRadioAdministrativeStateArray::Deserialize elements count exceeds");
        return false;
    }

    auto ip_address = RadioAdministrativeState::Deserialize(raw_data);
    if (ip_address == nullptr) {
        return false;
    }
    items[count] = ip_address;
    count++;
    return true;
}

nonstd::span<const RadioAdministrativeState *const>
ReadableRadioAdministrativeStateArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableRadioAdministrativeStateArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME RadioAdministrativeState #%lu RadioID:%u, AdminState:%u",
              i,
              items[i]->RadioID,
              items[i]->AdminState);
    }
}