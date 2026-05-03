#include "DeleteWlan.h"
#include "Logging.h"
#include "lassert.h"
#include <algorithm>
#include <cstring>

DeleteWlan::DeleteWlan(uint8_t radio_id, uint8_t wlan_id)
    : ElementHeader(ElementHeader::DeleteWlan, sizeof(DeleteWlan) - sizeof(ElementHeader)),
      RadioID{ radio_id }, WlanID{ wlan_id } {
}

bool DeleteWlan::Validate() const {
    static_assert(sizeof(DeleteWlan) == 6);
    if (ElementHeader::GetElementType() != ElementHeader::DeleteWlan) {
        return false;
    }
    if (ElementHeader::GetLength() != (sizeof(DeleteWlan) - sizeof(ElementHeader))) {
        return false;
    }
    if (RadioID > 31) {
        return false;
    }
    if (WlanID < 1 || WlanID > 16) {
        return false;
    }
    return true;
}

WritableDeleteWlanArray::WritableDeleteWlanArray() {
    static_assert(sizeof(DeleteWlan) == 6);
    items.reserve(ReadableDeleteWlanArray::max_count);
}

void WritableDeleteWlanArray::Add(DeleteWlan wlan) {
    ASSERT(items.size() + 1 <= ReadableDeleteWlanArray::max_count);

    auto it_exists = std::find_if(items.begin(), items.end(), [&wlan](const DeleteWlan &item) {
        return item.RadioID == wlan.RadioID && item.WlanID == wlan.WlanID;
    });
    if (it_exists != items.end()) {
        *it_exists = std::move(wlan);
        log_i("DeleteWlan: replace RadioID: %u, WlanID: %u",
              (*it_exists).RadioID,
              (*it_exists).WlanID);
    } else {
        items.emplace_back(std::move(wlan));
    }
}

bool WritableDeleteWlanArray::Empty() const {
    return items.empty();
}

void WritableDeleteWlanArray::Clear() {
    items.clear();
}

void WritableDeleteWlanArray::Serialize(RawData *raw_data) const {
    for (const auto &item : items) {
        ASSERT(raw_data->current + sizeof(item) <= raw_data->end);
        std::memcpy(raw_data->current, &item, sizeof(item));
        raw_data->current += sizeof(item);
    }
}

void WritableDeleteWlanArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME DeleteWlan #%zu RadioID:%u, WlanID:%u", i, items[i].RadioID, items[i].WlanID);
    }
}

ReadableDeleteWlanArray::ReadableDeleteWlanArray() : count{ 0 } {
}

bool ReadableDeleteWlanArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableDeleteWlanArray::Deserialize elements count exceeds");
        return false;
    }

    if (raw_data->current + sizeof(DeleteWlan) > raw_data->end) {
        return false;
    }

    auto item = (DeleteWlan *)raw_data->current;
    if (!item->Validate()) {
        return false;
    }
    raw_data->current += sizeof(DeleteWlan);

    items[count] = item;
    count++;
    return true;
}

nonstd::span<const DeleteWlan *const> ReadableDeleteWlanArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableDeleteWlanArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME DeleteWlan #%zu RadioID:%u, WlanID:%u", i, items[i]->RadioID, items[i]->WlanID);
    }
}
