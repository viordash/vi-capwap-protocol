#include "UpdateWlan.h"
#include "Logging.h"
#include "lassert.h"
#include <algorithm>
#include <cstring>

UpdateWlan::UpdateWlan(uint8_t radio_id,
                       uint8_t wlan_id,
                       uint16_t capability,
                       uint8_t key_index,
                       KeyStatus key_status,
                       uint16_t key_length)
    : ElementHeader(ElementHeader::UpdateWlan,
                    sizeof(UpdateWlan) - sizeof(ElementHeader) + key_length),
      radio_id{ radio_id }, wlan_id{ wlan_id }, capability{ capability }, key_index{ key_index },
      key_status{ key_status }, key_length{ key_length } {
}

uint8_t UpdateWlan::GetRadioID() const {
    return radio_id;
}

uint8_t UpdateWlan::GetWlanID() const {
    return wlan_id;
}

uint16_t UpdateWlan::GetCapability() const {
    return capability.Get();
}

uint8_t UpdateWlan::GetKeyIndex() const {
    return key_index;
}

UpdateWlan::KeyStatus UpdateWlan::GetKeyStatus() const {
    return key_status;
}

uint16_t UpdateWlan::GetKeyLength() const {
    return key_length.Get();
}

bool UpdateWlan::Validate() const {
    if (ElementHeader::GetElementType() != ElementHeader::UpdateWlan) {
        return false;
    }
    auto length = ElementHeader::GetLength();
    if (length < 8) {
        return false;
    }
    if (radio_id > 31) {
        return false;
    }
    if (wlan_id < 1 || wlan_id > 16) {
        return false;
    }
    switch (key_status) {
        case KeyStatus::PerStation:
        case KeyStatus::SharedWEP:
        case KeyStatus::BeginRekeying:
        case KeyStatus::CompletedRekeying:
            break;

        default:
            return false;
    }

    if (key_length.Get() > max_key_length) {
        return false;
    }
    if (length != 8 + key_length.Get()) {
        return false;
    }
    return true;
}

WritableUpdateWlanArray::WritableUpdateWlanArray() {
    static_assert(sizeof(Item::header) == 12);
    items.reserve(ReadableUpdateWlanArray::max_count);
}

void WritableUpdateWlanArray::Add(Item element) {
    ASSERT(items.size() + 1 <= ReadableUpdateWlanArray::max_count);

    auto it_exists = std::find_if(items.begin(), items.end(), [&element](const Item &item) {
        return item.header.GetRadioID() == element.header.GetRadioID()
            && item.header.GetWlanID() == element.header.GetWlanID();
    });

    if (it_exists != items.end()) {
        *it_exists = std::move(element);
        log_i("UpdateWlan: replace RadioID: %u, WlanID: %u",
              (*it_exists).header.GetRadioID(),
              (*it_exists).header.GetWlanID());
    } else {
        items.emplace_back(std::move(element));
    }
}

bool WritableUpdateWlanArray::Empty() const {
    return items.empty();
}

void WritableUpdateWlanArray::Clear() {
    items.clear();
}

void WritableUpdateWlanArray::Serialize(RawData *raw_data) const {
    for (const auto &item : items) {
        ASSERT(raw_data->current + sizeof(item.header) <= raw_data->end);
        std::memcpy(raw_data->current, &item.header, sizeof(item.header));
        raw_data->current += sizeof(item.header);
        uint16_t data_size =
            item.header.GetLength() - (sizeof(item.header) - sizeof(ElementHeader));
        std::memcpy(raw_data->current, item.data.data(), data_size);
        raw_data->current += data_size;
    }
}

void WritableUpdateWlanArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME UpdateWlan #%zu RadioID:%u, WlanID:%u, Capability:0x%04X, KeyIndex:%u, "
              "KeyStatus:%u, KeyLen:%zu",
              i,
              items[i].header.GetRadioID(),
              items[i].header.GetWlanID(),
              items[i].header.GetCapability(),
              items[i].header.GetKeyIndex(),
              (unsigned)items[i].header.GetKeyStatus(),
              items[i].data.size());
    }
}

ReadableUpdateWlanArray::ReadableUpdateWlanArray() : count{ 0 } {
}

bool ReadableUpdateWlanArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableUpdateWlanArray::Deserialize elements count exceeds");
        return false;
    }

    if (raw_data->current + sizeof(UpdateWlan) > raw_data->end) {
        return false;
    }

    auto item = (ReadableUpdateWlanArray::Item *)raw_data->current;
    if (!item->Validate()) {
        return false;
    }

    uint8_t *last = raw_data->current + sizeof(ElementHeader) + item->GetLength();
    if (last > raw_data->end) {
        return false;
    }

    raw_data->current = last;
    items[count] = item;
    count++;
    return true;
}

nonstd::span<const ReadableUpdateWlanArray::Item *const> ReadableUpdateWlanArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableUpdateWlanArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME UpdateWlan #%zu RadioID:%u, WlanID:%u, Capability:0x%04X, KeyIndex:%u, "
              "KeyStatus:%u, KeyLen:%u",
              i,
              items[i]->GetRadioID(),
              items[i]->GetWlanID(),
              items[i]->GetCapability(),
              items[i]->GetKeyIndex(),
              (unsigned)items[i]->GetKeyStatus(),
              items[i]->GetKeyLength());
    }
}

ElementHeader::ElementType ReadableUpdateWlanArray::GetElementType() const {
    return ElementHeader::UpdateWlan;
}

bool ReadableUpdateWlanArray::IsPresent() const {
    return count > 0;
}
