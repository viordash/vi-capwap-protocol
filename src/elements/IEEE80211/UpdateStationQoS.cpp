#include "UpdateStationQoS.h"
#include "Logging.h"
#include "lassert.h"
#include <algorithm>
#include <cstring>

UpdateStationQoS::UpdateStationQoS(uint8_t radio_id,
                                   const uint8_t *mac_address,
                                   uint8_t priority_8021p,
                                   uint8_t dscp_tag)
    : ElementHeader(ElementHeader::UpdateStationQoS,
                    sizeof(UpdateStationQoS) - sizeof(ElementHeader)),
      RadioID{ radio_id } {
    memcpy(MACAddress, mac_address, mac_address_size);
    // QoS Sub-Element layout (16 bits, network byte order):
    // Bits 15-11: Reserved (5 bits)
    // Bits 10-8: 802.1p Tag (3 bits)
    // Bits 7-6: Reserved (2 bits)
    // Bits 5-0: DSCP Tag (6 bits)
    uint16_t qos_value = ((priority_8021p & 0x07) << 8) | (dscp_tag & 0x3F);
    QoSSubElement = NetworkU16{ qos_value };
}

uint8_t UpdateStationQoS::Get8021pTag() const {
    return (QoSSubElement.Get() >> 8) & 0x07;
}

uint8_t UpdateStationQoS::GetDscpTag() const {
    return QoSSubElement.Get() & 0x3F;
}

void UpdateStationQoS::Set8021pTag(uint8_t tag) {
    uint16_t value = QoSSubElement.Get();
    value = (value & 0xF8FF) | ((tag & 0x07) << 8);
    QoSSubElement = NetworkU16{ value };
}

void UpdateStationQoS::SetDscpTag(uint8_t tag) {
    uint16_t value = QoSSubElement.Get();
    value = (value & 0xFFC0) | (tag & 0x3F);
    QoSSubElement = NetworkU16{ value };
}

bool UpdateStationQoS::Validate() const {
    static_assert(sizeof(UpdateStationQoS) == 13);
    if (ElementHeader::GetElementType() != ElementHeader::UpdateStationQoS) {
        return false;
    }
    if (ElementHeader::GetLength() != (sizeof(UpdateStationQoS) - sizeof(ElementHeader))) {
        return false;
    }
    if (RadioID > 31) {
        return false;
    }
    return true;
}


WritableUpdateStationQoSArray::WritableUpdateStationQoSArray() {
    static_assert(sizeof(items[0]) == 13);
    items.reserve(ReadableUpdateStationQoSArray::max_count);
}

void WritableUpdateStationQoSArray::Add(UpdateStationQoS element) {
    ASSERT(items.size() + 1 <= ReadableUpdateStationQoSArray::max_count);
    items.emplace_back(std::move(element));
}

bool WritableUpdateStationQoSArray::Empty() const {
    return items.empty();
}

void WritableUpdateStationQoSArray::Clear() {
    items.clear();
}

void WritableUpdateStationQoSArray::Serialize(RawData *raw_data) const {
    for (const auto &item : items) {
        ASSERT(raw_data->current + sizeof(item) <= raw_data->end);
        std::memcpy(raw_data->current, &item, sizeof(item));
        raw_data->current += sizeof(item);
    }
}

void WritableUpdateStationQoSArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME UpdateStationQoS #%zu RadioID:%u, MAC:%02X:%02X:%02X:%02X:%02X:%02X, 8021p:%u, "
              "DSCP:%u",
              i,
              items[i].RadioID,
              items[i].MACAddress[0],
              items[i].MACAddress[1],
              items[i].MACAddress[2],
              items[i].MACAddress[3],
              items[i].MACAddress[4],
              items[i].MACAddress[5],
              items[i].Get8021pTag(),
              items[i].GetDscpTag());
    }
}

ReadableUpdateStationQoSArray::ReadableUpdateStationQoSArray() : count{ 0 } {
}

ElementHeader::ElementType ReadableUpdateStationQoSArray::GetElementType() const {
    return ElementHeader::UpdateStationQoS;
}

bool ReadableUpdateStationQoSArray::IsPresent() const {
    return count > 0;
}


bool ReadableUpdateStationQoSArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableUpdateStationQoSArray::Deserialize elements count exceeds");
        return false;
    }

    if (raw_data->current + sizeof(UpdateStationQoS) > raw_data->end) {
        return false;
    }

    auto item = (UpdateStationQoS *)raw_data->current;
    if (!item->Validate()) {
        return false;
    }
    raw_data->current += sizeof(UpdateStationQoS);

    items[count] = item;
    count++;
    return true;
}

nonstd::span<const UpdateStationQoS *const> ReadableUpdateStationQoSArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableUpdateStationQoSArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME UpdateStationQoS #%zu RadioID:%u, MAC:%02X:%02X:%02X:%02X:%02X:%02X, 8021p:%u, "
              "DSCP:%u",
              i,
              items[i]->RadioID,
              items[i]->MACAddress[0],
              items[i]->MACAddress[1],
              items[i]->MACAddress[2],
              items[i]->MACAddress[3],
              items[i]->MACAddress[4],
              items[i]->MACAddress[5],
              items[i]->Get8021pTag(),
              items[i]->GetDscpTag());
    }
}
