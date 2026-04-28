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
    memcpy(this->mac_address, mac_address, mac_address_size);
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
        log_e("Station: supported rates length exceeds limit: %u",
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

void Station::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(Station) <= raw_data->end);
    Station *dst = (Station *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(Station);
}

Station *Station::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(Station) > raw_data->end) {
        return nullptr;
    }

    auto res = (Station *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }

    uint8_t *last = raw_data->current + sizeof(ElementHeader) + res->GetLength();
    if (last > raw_data->end) {
        return nullptr;
    }

    raw_data->current = last;
    return res;
}

WritableStationArray::Item::Item(uint8_t radio_id,
                                 uint16_t association_id,
                                 uint8_t flags,
                                 const uint8_t *mac_address,
                                 uint16_t capabilities,
                                 uint8_t wlan_id,
                                 nonstd::span<const uint8_t> supported_rates)
    : supported_rates_data{ supported_rates }, header{ radio_id,
                                                       association_id,
                                                       flags,
                                                       mac_address,
                                                       capabilities,
                                                       wlan_id,
                                                       (uint16_t)supported_rates_data.size() } {
}

const uint8_t *WritableStationArray::Item::GetMACAddress() const {
    return header.GetMACAddress();
}

uint8_t WritableStationArray::Item::GetRadioID() const {
    return header.GetRadioID();
}

WritableStationArray::WritableStationArray() {
    items.reserve(ReadableStationArray::max_count);
}

void WritableStationArray::Add(WritableStationArray::Item element) {
    ASSERT(items.size() + 1 <= ReadableStationArray::max_count);

    items.emplace_back(std::move(element));
}

bool WritableStationArray::Empty() const {
    return items.empty();
}

void WritableStationArray::Clear() {
    items.clear();
}

void WritableStationArray::Serialize(RawData *raw_data) const {
    for (const auto &elem : items) {
        elem.header.Serialize(raw_data);
        uint16_t rates_size = elem.header.GetLength() - (sizeof(Station) - sizeof(ElementHeader));
        memcpy(raw_data->current, elem.supported_rates_data.data(), rates_size);
        raw_data->current += rates_size;
    }
}

void WritableStationArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME Station #%zu RadioID:%u, AssocID:%u, MAC:%02X:%02X:%02X:%02X:%02X:%02X, "
              "WlanID:%u, RatesLen:%zu",
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
              items[i].supported_rates_data.size());
    }
}

ReadableStationArray::ReadableStationArray() : count{ 0 } {
}

bool ReadableStationArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableStationArray::Deserialize elements count exceeds");
        return false;
    }

    auto station = Station::Deserialize(raw_data);
    if (station == nullptr) {
        return false;
    }
    items[count] = station;
    count++;
    return true;
}

nonstd::span<const Station *const> ReadableStationArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableStationArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME Station #%zu RadioID:%u, AssocID:%u, MAC:%02X:%02X:%02X:%02X:%02X:%02X, "
              "WlanID:%u, RatesLen:%u",
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
