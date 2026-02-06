#include "DeleteStation.h"
#include "ClearHeader.h"
#include "lassert.h"
#include "logging.h"
#include <string.h>

DeleteStation::DeleteStation(uint8_t radio_id, uint8_t mac_length)
    : ElementHeader(ElementHeader::DeleteStation,
                    (sizeof(DeleteStation) + mac_length) - sizeof(ElementHeader)),
      RadioID{ radio_id }, MACAddress{ mac_length } {
}
bool DeleteStation::Validate() const {
    static_assert(sizeof(DeleteStation) == 6);

    if (ElementHeader::GetElementType() != ElementHeader::DeleteStation) {
        return false;
    }

    switch (MACAddress.Length) {
        case RadioMACAddress::mac_EUI48_size:
        case RadioMACAddress::mac_EUI64_size:
            break;
        default:
            return false;
    }
    auto mac_len = GetLength() - (sizeof(DeleteStation) - sizeof(ElementHeader));

    if (mac_len != MACAddress.Length) {
        log_e("DeleteStation: wrong element length");
        return false;
    }

    if (!MACAddress.Validate()) {
        return false;
    }

    return true;
}

WritableDeleteStationArray::WritableDeleteStationArray() {
    items.reserve(ReadableDeleteStationArray::max_count);
}

void WritableDeleteStationArray::Add(uint8_t radio_id, MacAddress mac_address) {
    ASSERT(items.size() + 1 <= ReadableDeleteStationArray::max_count);

    auto it_exists =
        std::find_if(items.begin(),
                     items.end(),
                     [&radio_id, &mac_address](const WritableDeleteStationArray::Item &item) {
                         return item.header.RadioID == radio_id && item.Mac == mac_address;
                     });
    if (it_exists != items.end()) {
        *it_exists = WritableDeleteStationArray::Item{ radio_id, std::move(mac_address) };
        log_i("DeleteStation: replace RadioID: %u, MacAddress:", radio_id);
        MacAddress::Log(0, (*it_exists).Mac.Length, (*it_exists).Mac.Address);
    } else {
        items.emplace_back(radio_id, std::move(mac_address));
    }
}
bool WritableDeleteStationArray::Empty() const {
    return items.empty();
}
void WritableDeleteStationArray::Clear() {
    items.clear();
}

void WritableDeleteStationArray::Serialize(RawData *raw_data) const {
    ASSERT(items.size() <= ReadableDeleteStationArray::max_count);

    for (const auto &elem : items) {
        ASSERT(raw_data->current + sizeof(DeleteStation) + elem.header.GetLength()
               <= raw_data->end);
        DeleteStation *dst = (DeleteStation *)raw_data->current;
        *dst = elem.header;
        raw_data->current += sizeof(DeleteStation);

        memcpy(raw_data->current, elem.Mac.Address, elem.header.MACAddress.Length);
        raw_data->current +=
            elem.header.GetLength() - (sizeof(DeleteStation) - sizeof(ElementHeader));
    }
}
uint16_t WritableDeleteStationArray::GetTotalLength() const {
    uint16_t len = 0;
    for (const auto &elem : items) {
        len += elem.header.GetLength() + sizeof(ElementHeader);
    }
    return len;
}

void WritableDeleteStationArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME DeleteStation #%lu Radio ID:%u", i, items[i].header.RadioID);
        MacAddress::Log(i, items[i].Mac.Length, items[i].Mac.Address);
    }
}

ReadableDeleteStationArray::ReadableDeleteStationArray() : count{ 0 } {
}

bool ReadableDeleteStationArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableDeleteStationArray::Deserialize elements count exceeds");
        return false;
    }

    if (raw_data->current + sizeof(DeleteStation) > raw_data->end) {
        return false;
    }

    auto item = (DeleteStation *)raw_data->current;
    if (!item->Validate()) {
        return false;
    }
    raw_data->current += item->GetLength() + sizeof(ElementHeader);

    items[count] = item;
    count++;
    return true;
}

nonstd::span<const DeleteStation *const> ReadableDeleteStationArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableDeleteStationArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME DeleteStation #%lu Radio ID:%u", i, items[i]->RadioID);
        MacAddress::Log(i, items[i]->MACAddress.Length, items[i]->MACAddress.MACAddresses);
    }
}