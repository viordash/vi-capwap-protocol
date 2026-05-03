#include "MICCountermeasures.h"
#include "Logging.h"
#include "lassert.h"
#include <algorithm>
#include <cstring>

MICCountermeasures::MICCountermeasures(uint8_t radio_id,
                                       uint8_t wlan_id,
                                       const uint8_t *mac_address)
    : ElementHeader(ElementHeader::MICCountermeasures,
                    sizeof(MICCountermeasures) - sizeof(ElementHeader)),
      RadioID{ radio_id }, WlanID{ wlan_id } {
    memcpy(MACAddress, mac_address, mac_address_size);
}

bool MICCountermeasures::Validate() const {
    static_assert(sizeof(MICCountermeasures) == 12);
    if (ElementHeader::GetElementType() != ElementHeader::MICCountermeasures) {
        return false;
    }
    if (ElementHeader::GetLength() != (sizeof(MICCountermeasures) - sizeof(ElementHeader))) {
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

WritableMICCountermeasuresArray::WritableMICCountermeasuresArray() {
    static_assert(sizeof(items[0]) == 12);
    items.reserve(ReadableMICCountermeasuresArray::max_count);
}

void WritableMICCountermeasuresArray::Add(MICCountermeasures cm) {
    ASSERT(items.size() + 1 <= ReadableMICCountermeasuresArray::max_count);

    items.emplace_back(std::move(cm));
}

bool WritableMICCountermeasuresArray::Empty() const {
    return items.empty();
}

void WritableMICCountermeasuresArray::Clear() {
    items.clear();
}

void WritableMICCountermeasuresArray::Serialize(RawData *raw_data) const {
    for (const auto &item : items) {
        ASSERT(raw_data->current + sizeof(item) <= raw_data->end);
        std::memcpy(raw_data->current, &item, sizeof(item));
        raw_data->current += sizeof(item);
    }
}

void WritableMICCountermeasuresArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME MICCountermeasures #%zu RadioID:%u, WlanID:%u, "
              "MAC:%02X:%02X:%02X:%02X:%02X:%02X",
              i,
              items[i].RadioID,
              items[i].WlanID,
              items[i].MACAddress[0],
              items[i].MACAddress[1],
              items[i].MACAddress[2],
              items[i].MACAddress[3],
              items[i].MACAddress[4],
              items[i].MACAddress[5]);
    }
}

ReadableMICCountermeasuresArray::ReadableMICCountermeasuresArray() : count{ 0 } {
}

ElementHeader::ElementType ReadableMICCountermeasuresArray::GetElementType() const {
    return ElementHeader::MICCountermeasures;
}

bool ReadableMICCountermeasuresArray::IsPresent() const {
    return count > 0;
}


bool ReadableMICCountermeasuresArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableMICCountermeasuresArray::Deserialize elements count exceeds");
        return false;
    }
    if (raw_data->current + sizeof(MICCountermeasures) > raw_data->end) {
        return false;
    }

    auto item = (MICCountermeasures *)raw_data->current;
    if (!item->Validate()) {
        return false;
    }
    raw_data->current += sizeof(MICCountermeasures);

    items[count] = item;
    count++;
    return true;
}

nonstd::span<const MICCountermeasures *const> ReadableMICCountermeasuresArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableMICCountermeasuresArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME MICCountermeasures #%zu RadioID:%u, WlanID:%u, "
              "MAC:%02X:%02X:%02X:%02X:%02X:%02X",
              i,
              items[i]->RadioID,
              items[i]->WlanID,
              items[i]->MACAddress[0],
              items[i]->MACAddress[1],
              items[i]->MACAddress[2],
              items[i]->MACAddress[3],
              items[i]->MACAddress[4],
              items[i]->MACAddress[5]);
    }
}
