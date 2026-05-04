#include "AssignedWtpBssid.h"
#include "Logging.h"
#include "lassert.h"
#include <algorithm>
#include <arpa/inet.h>
#include <cstring>

AssignedWtpBssid::AssignedWtpBssid(uint8_t radio_id, uint8_t wlan_id, const uint8_t *bssid)
    : ElementHeader(ElementHeader::AssignedWtpBssid,
                    sizeof(AssignedWtpBssid) - sizeof(ElementHeader)),
      RadioID{ radio_id }, WlanID{ wlan_id } {
    memcpy(BSSID, bssid, bssid_size);
}

bool AssignedWtpBssid::Validate() const {
    static_assert(sizeof(AssignedWtpBssid) == 12);
    if (ElementHeader::GetElementType() != ElementHeader::AssignedWtpBssid) {
        return false;
    }
    if (ElementHeader::GetLength() != (sizeof(AssignedWtpBssid) - sizeof(ElementHeader))) {
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

WritableAssignedWtpBssidArray::WritableAssignedWtpBssidArray() {
    static_assert(sizeof(items[0]) == 12);
    items.reserve(ReadableAssignedWtpBssidArray::max_count);
}

void WritableAssignedWtpBssidArray::Add(AssignedWtpBssid bssid) {
    ASSERT(items.size() + 1 <= ReadableAssignedWtpBssidArray::max_count);

    auto it_exists =
        std::find_if(items.begin(), items.end(), [&bssid](const AssignedWtpBssid &item) {
            return item.RadioID == bssid.RadioID && item.WlanID == bssid.WlanID;
        });
    if (it_exists != items.end()) {
        *it_exists = std::move(bssid);
        log_i("AssignedWtpBssid: replace RadioID: %u, WlanID: %u",
              (*it_exists).RadioID,
              (*it_exists).WlanID);
    } else {
        items.emplace_back(std::move(bssid));
    }
}

bool WritableAssignedWtpBssidArray::Empty() const {
    return items.empty();
}

void WritableAssignedWtpBssidArray::Clear() {
    items.clear();
}

void WritableAssignedWtpBssidArray::Serialize(RawData *raw_data) const {
    for (const auto &item : items) {
        ASSERT(raw_data->current + sizeof(item) <= raw_data->end);
        std::memcpy(raw_data->current, &item, sizeof(item));
        raw_data->current += sizeof(item);
    }
}

void WritableAssignedWtpBssidArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME AssignedWtpBssid #%zu RadioID:%u, WlanID:%u, "
              "BSSID:%02X:%02X:%02X:%02X:%02X:%02X",
              i,
              items[i].RadioID,
              items[i].WlanID,
              items[i].BSSID[0],
              items[i].BSSID[1],
              items[i].BSSID[2],
              items[i].BSSID[3],
              items[i].BSSID[4],
              items[i].BSSID[5]);
    }
}

ReadableAssignedWtpBssidArray::ReadableAssignedWtpBssidArray() : count{ 0 } {
}

ElementHeader::ElementType ReadableAssignedWtpBssidArray::GetElementType() const {
    return ElementHeader::AssignedWtpBssid;
}

bool ReadableAssignedWtpBssidArray::IsPresent() const {
    return count > 0;
}

bool ReadableAssignedWtpBssidArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableAssignedWtpBssidArray::Deserialize elements count exceeds");
        return false;
    }

    if (raw_data->current + sizeof(AssignedWtpBssid) > raw_data->end) {
        return false;
    }

    auto item = (AssignedWtpBssid *)raw_data->current;
    if (!item->Validate()) {
        return false;
    }

    raw_data->current += sizeof(AssignedWtpBssid);
    items[count] = item;
    count++;
    return true;
}

nonstd::span<const AssignedWtpBssid *const> ReadableAssignedWtpBssidArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableAssignedWtpBssidArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME AssignedWtpBssid #%zu RadioID:%u, WlanID:%u, "
              "BSSID:%02X:%02X:%02X:%02X:%02X:%02X",
              i,
              items[i]->RadioID,
              items[i]->WlanID,
              items[i]->BSSID[0],
              items[i]->BSSID[1],
              items[i]->BSSID[2],
              items[i]->BSSID[3],
              items[i]->BSSID[4],
              items[i]->BSSID[5]);
    }
}
