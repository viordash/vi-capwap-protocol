#include "StationQoSProfile.h"
#include "Logging.h"
#include "lassert.h"
#include <algorithm>
#include <cstring>

StationQoSProfile::StationQoSProfile(const uint8_t *mac_address, uint8_t priority_8021p)
    : ElementHeader(ElementHeader::StationQoSProfile,
                    sizeof(StationQoSProfile) - sizeof(ElementHeader)),
      reserved{ 0 }, priority_8021p{ priority_8021p } {
    memcpy(this->mac_address, mac_address, mac_address_size);
}

const uint8_t *StationQoSProfile::GetMACAddress() const {
    return mac_address;
}

uint8_t StationQoSProfile::Get8021pPriority() const {
    return priority_8021p;
}

bool StationQoSProfile::Validate() const {
    static_assert(sizeof(StationQoSProfile) == 12); // 4 header + 8 value
    if (ElementHeader::GetElementType() != ElementHeader::StationQoSProfile) {
        return false;
    }
    if (ElementHeader::GetLength() != (sizeof(StationQoSProfile) - sizeof(ElementHeader))) {
        return false;
    }
    if (priority_8021p > max_8021p_priority) {
        return false;
    }
    return true;
}

WritableStationQoSProfileArray::WritableStationQoSProfileArray() {
    static_assert(sizeof(items[0]) == 12);
    items.reserve(ReadableStationQoSProfileArray::max_count);
}

void WritableStationQoSProfileArray::Add(StationQoSProfile element) {
    ASSERT(items.size() + 1 <= ReadableStationQoSProfileArray::max_count);

    items.emplace_back(std::move(element));
}

bool WritableStationQoSProfileArray::Empty() const {
    return items.empty();
}

void WritableStationQoSProfileArray::Clear() {
    items.clear();
}

void WritableStationQoSProfileArray::Serialize(RawData *raw_data) const {
    for (const auto &item : items) {
        ASSERT(raw_data->current + sizeof(item) <= raw_data->end);
        std::memcpy(raw_data->current, &item, sizeof(item));
        raw_data->current += sizeof(item);
    }
}

void WritableStationQoSProfileArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME StationQoSProfile #%zu MAC:%02X:%02X:%02X:%02X:%02X:%02X, 8021p:%u",
              i,
              items[i].GetMACAddress()[0],
              items[i].GetMACAddress()[1],
              items[i].GetMACAddress()[2],
              items[i].GetMACAddress()[3],
              items[i].GetMACAddress()[4],
              items[i].GetMACAddress()[5],
              items[i].Get8021pPriority());
    }
}

ReadableStationQoSProfileArray::ReadableStationQoSProfileArray() : count{ 0 } {
}

bool ReadableStationQoSProfileArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableStationQoSProfileArray::Deserialize elements count exceeds");
        return false;
    }

    if (raw_data->current + sizeof(StationQoSProfile) > raw_data->end) {
        return false;
    }

    auto item = (StationQoSProfile *)raw_data->current;
    if (!item->Validate()) {
        return false;
    }
    raw_data->current += sizeof(StationQoSProfile);

    items[count] = item;
    count++;
    return true;
}

nonstd::span<const StationQoSProfile *const> ReadableStationQoSProfileArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableStationQoSProfileArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME StationQoSProfile #%zu MAC:%02X:%02X:%02X:%02X:%02X:%02X, 8021p:%u",
              i,
              items[i]->GetMACAddress()[0],
              items[i]->GetMACAddress()[1],
              items[i]->GetMACAddress()[2],
              items[i]->GetMACAddress()[3],
              items[i]->GetMACAddress()[4],
              items[i]->GetMACAddress()[5],
              items[i]->Get8021pPriority());
    }
}
