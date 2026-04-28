#include "StationSessionKey.h"
#include "Logging.h"
#include "lassert.h"
#include <algorithm>
#include <cstring>

StationSessionKey::StationSessionKey(const uint8_t *mac_address,
                                     uint16_t flags,
                                     const uint8_t *pairwise_tsc,
                                     const uint8_t *pairwise_rsc,
                                     uint16_t key_length)
    : ElementHeader(ElementHeader::StationSessionKey,
                    (sizeof(StationSessionKey) - sizeof(ElementHeader)) + key_length),
      flags{ flags } {
    memcpy(this->mac_address, mac_address, mac_address_size);
    memcpy(this->pairwise_tsc, pairwise_tsc, tsc_size);
    memcpy(this->pairwise_rsc, pairwise_rsc, rsc_size);
}

const uint8_t *StationSessionKey::GetMACAddress() const {
    return mac_address;
}

uint16_t StationSessionKey::GetFlags() const {
    return flags.Get();
}

bool StationSessionKey::GetAKMOnlyFlag() const {
    return (flags.Get() & FLAG_AKM_ONLY) != 0;
}

bool StationSessionKey::GetACCryptoFlag() const {
    return (flags.Get() & FLAG_AC_CRYPTO) != 0;
}

const uint8_t *StationSessionKey::GetPairwiseTSC() const {
    return pairwise_tsc;
}

const uint8_t *StationSessionKey::GetPairwiseRSC() const {
    return pairwise_rsc;
}

uint16_t StationSessionKey::GetKeyLength() const {
    return GetLength() - (sizeof(StationSessionKey) - sizeof(ElementHeader));
}

bool StationSessionKey::Validate() const {
    static_assert(sizeof(StationSessionKey) == 24); // 4 header + 20 value
    if (ElementHeader::GetElementType() != ElementHeader::StationSessionKey) {
        return false;
    }
    if (GetLength() < (sizeof(StationSessionKey) - sizeof(ElementHeader))) {
        return false;
    }
    if (GetKeyLength() > max_key_length) {
        log_e("StationSessionKey: key length exceeds limit: %u", (unsigned)max_key_length);
        return false;
    }
    // Check reserved bits in flags (bits 13-0 must be zero)
    if ((flags.Get() & 0x3FFF) != 0) {
        return false;
    }
    return true;
}

void StationSessionKey::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(StationSessionKey) <= raw_data->end);
    StationSessionKey *dst = (StationSessionKey *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(StationSessionKey);
}

StationSessionKey *StationSessionKey::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(StationSessionKey) > raw_data->end) {
        return nullptr;
    }

    auto res = (StationSessionKey *)raw_data->current;
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

WritableStationSessionKeyArray::Item::Item(const uint8_t *mac_address,
                                           uint16_t flags,
                                           const uint8_t *pairwise_tsc,
                                           const uint8_t *pairwise_rsc,
                                           nonstd::span<const uint8_t> key)
    : key_data{ key },
      header{ mac_address, flags, pairwise_tsc, pairwise_rsc, (uint16_t)key_data.size() } {
}

const uint8_t *WritableStationSessionKeyArray::Item::GetMACAddress() const {
    return header.GetMACAddress();
}

WritableStationSessionKeyArray::WritableStationSessionKeyArray() {
    items.reserve(ReadableStationSessionKeyArray::max_count);
}

void WritableStationSessionKeyArray::Add(WritableStationSessionKeyArray::Item element) {
    ASSERT(items.size() + 1 <= ReadableStationSessionKeyArray::max_count);

    items.emplace_back(std::move(element));
}

bool WritableStationSessionKeyArray::Empty() const {
    return items.empty();
}

void WritableStationSessionKeyArray::Clear() {
    items.clear();
}

void WritableStationSessionKeyArray::Serialize(RawData *raw_data) const {
    for (const auto &elem : items) {
        elem.header.Serialize(raw_data);
        uint16_t key_size =
            elem.header.GetLength() - (sizeof(StationSessionKey) - sizeof(ElementHeader));
        memcpy(raw_data->current, elem.key_data.data(), key_size);
        raw_data->current += key_size;
    }
}

void WritableStationSessionKeyArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME StationSessionKey #%zu MAC:%02X:%02X:%02X:%02X:%02X:%02X, Flags:0x%04X "
              "(A:%u, C:%u), KeyLen:%zu",
              i,
              items[i].header.GetMACAddress()[0],
              items[i].header.GetMACAddress()[1],
              items[i].header.GetMACAddress()[2],
              items[i].header.GetMACAddress()[3],
              items[i].header.GetMACAddress()[4],
              items[i].header.GetMACAddress()[5],
              items[i].header.GetFlags(),
              items[i].header.GetAKMOnlyFlag() ? 1 : 0,
              items[i].header.GetACCryptoFlag() ? 1 : 0,
              items[i].key_data.size());
    }
}

ReadableStationSessionKeyArray::ReadableStationSessionKeyArray() : count{ 0 } {
}

bool ReadableStationSessionKeyArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableStationSessionKeyArray::Deserialize elements count exceeds");
        return false;
    }

    auto key = StationSessionKey::Deserialize(raw_data);
    if (key == nullptr) {
        return false;
    }
    items[count] = key;
    count++;
    return true;
}

nonstd::span<const StationSessionKey *const> ReadableStationSessionKeyArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableStationSessionKeyArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME StationSessionKey #%zu MAC:%02X:%02X:%02X:%02X:%02X:%02X, Flags:0x%04X "
              "(A:%u, C:%u), KeyLen:%u",
              i,
              items[i]->GetMACAddress()[0],
              items[i]->GetMACAddress()[1],
              items[i]->GetMACAddress()[2],
              items[i]->GetMACAddress()[3],
              items[i]->GetMACAddress()[4],
              items[i]->GetMACAddress()[5],
              items[i]->GetFlags(),
              items[i]->GetAKMOnlyFlag() ? 1 : 0,
              items[i]->GetACCryptoFlag() ? 1 : 0,
              items[i]->GetKeyLength());
    }
}
