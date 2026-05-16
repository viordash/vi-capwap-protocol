#include "Station.h"
#include "Logging.h"
#include "lassert.h"
#include <algorithm>
#include <cstring>

Station::Station(uint8_t radio_id,
                 uint16_t association_id,
                 uint8_t flags,
                 const uint8_t *mac_address,
                 uint16_t capabilities,
                 uint8_t wlan_id,
                 uint16_t supported_rates_length)
    : ElementHeader(ElementHeader::Station,
                    (sizeof(Station) - sizeof(ElementHeader)) + supported_rates_length),
      radio_id{ radio_id }, association_id{ association_id }, flags{ flags },
      capabilities{ capabilities }, wlan_id{ wlan_id } {
    std::memcpy(this->mac_address, mac_address, mac_address_size);
}

uint8_t Station::GetRadioID() const {
    return radio_id;
}

uint16_t Station::GetAssociationID() const {
    return association_id.Get();
}

uint8_t Station::GetFlags() const {
    return flags;
}

const uint8_t *Station::GetMACAddress() const {
    return mac_address;
}

uint16_t Station::GetCapabilities() const {
    return capabilities.Get();
}

uint8_t Station::GetWlanID() const {
    return wlan_id;
}

uint16_t Station::GetSupportedRatesLength() const {
    return GetLength() - (sizeof(Station) - sizeof(ElementHeader));
}

bool Station::Validate() const {
    static_assert(sizeof(Station) == 17); // 4 header + 13 value
    if (ElementHeader::GetElementType() != ElementHeader::Station) {
        return false;
    }
    if (GetLength() < (sizeof(Station) - sizeof(ElementHeader))) {
        return false;
    }
    if (GetSupportedRatesLength() > max_supported_rates_length) {
        log_e("Station: supported rates length exceeds limit: {}",
              (unsigned)max_supported_rates_length);
        return false;
    }
    if (radio_id > 31) {
        return false;
    }
    if (wlan_id < min_wlan_id || wlan_id > max_wlan_id) {
        return false;
    }
    return true;
}

WritableStationArray::WritableStationArray() {
    static_assert(sizeof(Item::header) == 17);
    items.reserve(ReadableStationArray::max_count);
}

void WritableStationArray::Add(WritableStationArray::Item element) {
    ASSERT(items.size() + 1 <= ReadableStationArray::max_count);

    auto it_exists = std::find_if(items.begin(), items.end(), [&element](const Item &item) {
        return item.header.GetRadioID() == element.header.GetRadioID()
            && memcmp(item.header.GetMACAddress(),
                      element.header.GetMACAddress(),
                      Station::mac_address_size)
                   == 0;
    });

    if (it_exists != items.end()) {
        *it_exists = std::move(element);
        log_i("Station: replace RadioID: {}, MAC: {:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}",
              (*it_exists).header.GetRadioID(),
              (*it_exists).header.GetMACAddress()[0],
              (*it_exists).header.GetMACAddress()[1],
              (*it_exists).header.GetMACAddress()[2],
              (*it_exists).header.GetMACAddress()[3],
              (*it_exists).header.GetMACAddress()[4],
              (*it_exists).header.GetMACAddress()[5]);
    } else {
        items.emplace_back(std::move(element));
    }
}

bool WritableStationArray::Empty() const {
    return items.empty();
}

void WritableStationArray::Clear() {
    items.clear();
}

void WritableStationArray::Serialize(RawData *raw_data) const {
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

void WritableStationArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME Station #{} RadioID:{}, AssocID:{}, MAC:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}, "
              "WlanID:{}, RatesLen:{}",
              i,
              items[i].header.GetRadioID(),
              items[i].header.GetAssociationID(),
              items[i].header.GetMACAddress()[0],
              items[i].header.GetMACAddress()[1],
              items[i].header.GetMACAddress()[2],
              items[i].header.GetMACAddress()[3],
              items[i].header.GetMACAddress()[4],
              items[i].header.GetMACAddress()[5],
              items[i].header.GetWlanID(),
              items[i].data.size());
    }
}

ReadableStationArray::ReadableStationArray() : count{ 0 } {
}

bool ReadableStationArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableStationArray::Deserialize elements count exceeds");
        return false;
    }

    if (raw_data->current + sizeof(Station) > raw_data->end) {
        return false;
    }

    auto item = (ReadableStationArray::Item *)raw_data->current;
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

nonstd::span<const ReadableStationArray::Item *const> ReadableStationArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableStationArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME Station #{} RadioID:{}, AssocID:{}, MAC:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}, "
              "WlanID:{}, RatesLen:{}",
              i,
              items[i]->GetRadioID(),
              items[i]->GetAssociationID(),
              items[i]->GetMACAddress()[0],
              items[i]->GetMACAddress()[1],
              items[i]->GetMACAddress()[2],
              items[i]->GetMACAddress()[3],
              items[i]->GetMACAddress()[4],
              items[i]->GetMACAddress()[5],
              items[i]->GetWlanID(),
              items[i]->GetSupportedRatesLength());
    }
}

ElementHeader::ElementType ReadableStationArray::GetElementType() const {
    return ElementHeader::Station;
}

bool ReadableStationArray::IsPresent() const {
    return count > 0;
}
