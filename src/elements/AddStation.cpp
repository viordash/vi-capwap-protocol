#include "AddStation.h"
#include "ClearHeader.h"
#include "Logging.h"
#include "lassert.h"
#include <cstring>
#include <string.h>

AddStation::AddStation(uint8_t radio_id, uint8_t mac_length, uint16_t vlan_name_length)
    : ElementHeader(ElementHeader::AddStation,
                    (sizeof(AddStation) + mac_length + vlan_name_length) - sizeof(ElementHeader)),
      RadioID{ radio_id }, MACAddress{ mac_length } {
}

bool AddStation::Validate() const {
    static_assert(sizeof(AddStation) == 6);

    if (ElementHeader::GetElementType() != ElementHeader::AddStation) {
        return false;
    }

    switch (MACAddress.Length) {
        case RadioMACAddress::mac_EUI48_size:
        case RadioMACAddress::mac_EUI64_size:
            break;
        default:
            return false;
    }

    auto body_length = GetLength() - (sizeof(AddStation) - sizeof(ElementHeader));

    if (body_length < MACAddress.Length) {
        log_e("AddStation: wrong element length");
        return false;
    }

    if (!MACAddress.Validate()) {
        return false;
    }

    auto vlan_name_length = body_length - MACAddress.Length;
    if (vlan_name_length > max_vlan_name_length) {
        log_e("AddStation: VLAN name length exceeds limit: {}", (unsigned)vlan_name_length);
        return false;
    }

    return true;
}

uint16_t AddStation::GetVlanNameLength() const {
    auto body_length = GetLength() - (sizeof(AddStation) - sizeof(ElementHeader));
    return body_length - MACAddress.Length;
}

WritableAddStationArray::Item::Item(uint8_t radio_id,
                                    MacAddress &&mac_address,
                                    std::string vlan_name)
    : Mac{ std::move(mac_address) }, vlan_name{ std::move(vlan_name) },
      header{ radio_id, (uint8_t)Mac.Length, (uint16_t)this->vlan_name.size() } {
}

WritableAddStationArray::WritableAddStationArray() {
    static_assert(sizeof(Item::header) == 6);
    items.reserve(ReadableAddStationArray::max_count);
}

void WritableAddStationArray::Add(uint8_t radio_id, MacAddress mac_address, std::string vlan_name) {
    ASSERT(items.size() + 1 <= ReadableAddStationArray::max_count);

    auto it_exists =
        std::find_if(items.begin(),
                     items.end(),
                     [&radio_id, &mac_address](const WritableAddStationArray::Item &item) {
                         return item.header.RadioID == radio_id && item.Mac == mac_address;
                     });
    if (it_exists != items.end()) {
        *it_exists =
            WritableAddStationArray::Item{ radio_id, std::move(mac_address), std::move(vlan_name) };
        log_i("AddStation: replace RadioID: {}, MacAddress:", radio_id);
        MacAddress::Log(0, (*it_exists).Mac.Length, (*it_exists).Mac.Address);
    } else {
        items.emplace_back(radio_id, std::move(mac_address), std::move(vlan_name));
    }
}

bool WritableAddStationArray::Empty() const {
    return items.empty();
}

void WritableAddStationArray::Clear() {
    items.clear();
}

void WritableAddStationArray::Serialize(RawData *raw_data) const {
    ASSERT(items.size() <= ReadableAddStationArray::max_count);

    for (const auto &item : items) {
        ASSERT(raw_data->current + sizeof(item.header) + item.header.GetLength() <= raw_data->end);
        std::memcpy(raw_data->current, &item.header, sizeof(item.header));
        raw_data->current += sizeof(item.header);

        uint16_t mac_size = item.Mac.Length;
        std::memcpy(raw_data->current, item.Mac.Address, mac_size);
        raw_data->current += mac_size;

        if (!item.vlan_name.empty()) {
            std::memcpy(raw_data->current, item.vlan_name.data(), item.vlan_name.size());
            raw_data->current += item.vlan_name.size();
        }
    }
}

void WritableAddStationArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME AddStation #{} Radio ID:{}", i, items[i].header.RadioID);
        MacAddress::Log(i, items[i].Mac.Length, items[i].Mac.Address);
        if (!items[i].vlan_name.empty()) {
            log_i("  VLAN Name: '{}'", items[i].vlan_name);
        }
    }
}

ReadableAddStationArray::ReadableAddStationArray() : count{ 0 } {
}

bool ReadableAddStationArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableAddStationArray::Deserialize elements count exceeds");
        return false;
    }

    if (raw_data->current + sizeof(AddStation) > raw_data->end) {
        return false;
    }

    auto item = (const AddStation *)raw_data->current;
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

nonstd::span<const AddStation *const> ReadableAddStationArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableAddStationArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME AddStation #{} Radio ID:{}", i, items[i]->RadioID);
        MacAddress::Log(i, items[i]->MACAddress.Length, items[i]->MACAddress.MACAddresses);
        auto vlan_len = items[i]->GetVlanNameLength();
        if (vlan_len > 0) {
            log_i("  VLAN Name: '{}'",
                  std::string_view((const char *)(items[i]->MACAddress.MACAddresses
                                                  + items[i]->MACAddress.Length),
                                   vlan_len));
        }
    }
}

ElementHeader::ElementType ReadableAddStationArray::GetElementType() const {
    return ElementHeader::AddStation;
}

bool ReadableAddStationArray::IsPresent() const {
    return count > 0;
}
