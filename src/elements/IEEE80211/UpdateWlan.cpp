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
    : ElementHeader(ElementHeader::UpdateWlan, 8 + key_length), radio_id{ radio_id },
      wlan_id{ wlan_id }, capability{ capability }, key_index{ key_index },
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

void UpdateWlan::Serialize(RawData *raw_data) const {
    size_t total_size = sizeof(ElementHeader) + ElementHeader::GetLength();
    ASSERT(raw_data->current + total_size <= raw_data->end);
#pragma GCC diagnostic push
#if __GNUC__ >= 8
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif
    memcpy(raw_data->current, this, total_size);
#pragma GCC diagnostic pop
    raw_data->current += total_size;
}

UpdateWlan *UpdateWlan::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ElementHeader) > raw_data->end) {
        return nullptr;
    }

    auto res = (UpdateWlan *)raw_data->current;
    size_t total_size = sizeof(ElementHeader) + res->GetLength();
    if (raw_data->current + total_size > raw_data->end) {
        return nullptr;
    }
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += total_size;
    return res;
}

WritableUpdateWlanArray::Item::Item(uint8_t radio_id,
                                    uint8_t wlan_id,
                                    uint16_t capability,
                                    uint8_t key_index,
                                    UpdateWlan::KeyStatus key_status,
                                    std::vector<uint8_t> &&key)
    : key_data(std::move(key)), radio_id(radio_id), wlan_id(wlan_id), capability(capability),
      key_index(key_index), key_status(key_status) {
}

uint8_t WritableUpdateWlanArray::Item::GetRadioID() const {
    return radio_id;
}

uint8_t WritableUpdateWlanArray::Item::GetWlanID() const {
    return wlan_id;
}

WritableUpdateWlanArray::WritableUpdateWlanArray() {
    items.reserve(ReadableUpdateWlanArray::max_count);
}

void WritableUpdateWlanArray::Add(Item element) {
    ASSERT(items.size() + 1 <= ReadableUpdateWlanArray::max_count);
    items.emplace_back(std::move(element));
}

bool WritableUpdateWlanArray::Empty() const {
    return items.empty();
}

void WritableUpdateWlanArray::Clear() {
    items.clear();
}

void WritableUpdateWlanArray::Serialize(RawData *raw_data) const {
    for (const auto &elem : items) {
        UpdateWlan header(elem.radio_id,
                          elem.wlan_id,
                          elem.capability,
                          elem.key_index,
                          elem.key_status,
                          elem.key_data.size());
        header.Serialize(raw_data);
        // Overwrite key data in the serialized buffer
        if (!elem.key_data.empty()) {
            memcpy(raw_data->current - elem.key_data.size(),
                   elem.key_data.data(),
                   elem.key_data.size());
        }
    }
}

void WritableUpdateWlanArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME UpdateWlan #%zu RadioID:%u, WlanID:%u, Capability:0x%04X, KeyIndex:%u, "
              "KeyStatus:%u, KeyLen:%zu",
              i,
              items[i].radio_id,
              items[i].wlan_id,
              items[i].capability,
              items[i].key_index,
              items[i].key_status,
              items[i].key_data.size());
    }
}

ReadableUpdateWlanArray::ReadableUpdateWlanArray() : count{ 0 } {
}

bool ReadableUpdateWlanArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableUpdateWlanArray::Deserialize elements count exceeds");
        return false;
    }

    auto uw = UpdateWlan::Deserialize(raw_data);
    if (uw == nullptr) {
        return false;
    }
    items[count] = uw;
    count++;
    return true;
}

nonstd::span<const UpdateWlan *const> ReadableUpdateWlanArray::Get() const {
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
              items[i]->GetKeyStatus(),
              items[i]->GetKeyLength());
    }
}
