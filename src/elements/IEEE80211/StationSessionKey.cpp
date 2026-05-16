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
    std::memcpy(this->mac_address, mac_address, mac_address_size);
    std::memcpy(this->pairwise_tsc, pairwise_tsc, tsc_size);
    std::memcpy(this->pairwise_rsc, pairwise_rsc, rsc_size);
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
        log_e("StationSessionKey: key length exceeds limit: {}", (unsigned)max_key_length);
        return false;
    }
    // Check reserved bits in flags (bits 13-0 must be zero)
    if ((flags.Get() & 0x3FFF) != 0) {
        return false;
    }
    return true;
}

WritableStationSessionKeyArray::WritableStationSessionKeyArray() {
    static_assert(sizeof(Item::header) == 24);
    items.reserve(ReadableStationSessionKeyArray::max_count);
}

void WritableStationSessionKeyArray::Add(WritableStationSessionKeyArray::Item element) {
    ASSERT(items.size() + 1 <= ReadableStationSessionKeyArray::max_count);

    auto it_exists = std::find_if(items.begin(), items.end(), [&element](const Item &item) {
        return memcmp(item.header.GetMACAddress(),
                      element.header.GetMACAddress(),
                      StationSessionKey::mac_address_size)
            == 0;
    });

    if (it_exists != items.end()) {
        *it_exists = std::move(element);
        log_i("StationSessionKey: replace MAC: {:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}",
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

bool WritableStationSessionKeyArray::Empty() const {
    return items.empty();
}

void WritableStationSessionKeyArray::Clear() {
    items.clear();
}

void WritableStationSessionKeyArray::Serialize(RawData *raw_data) const {
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

void WritableStationSessionKeyArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME StationSessionKey #{} MAC:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}, Flags:0x{:04X} "
              "(A:{}, C:{}), KeyLen:{}",
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
              items[i].data.size());
    }
}

ReadableStationSessionKeyArray::ReadableStationSessionKeyArray() : count{ 0 } {
}

bool ReadableStationSessionKeyArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableStationSessionKeyArray::Deserialize elements count exceeds");
        return false;
    }

    if (raw_data->current + sizeof(StationSessionKey) > raw_data->end) {
        return false;
    }

    auto item = (ReadableStationSessionKeyArray::Item *)raw_data->current;
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

nonstd::span<const ReadableStationSessionKeyArray::Item *const>
ReadableStationSessionKeyArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableStationSessionKeyArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME StationSessionKey #{} MAC:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}, Flags:0x{:04X} "
              "(A:{}, C:{}), KeyLen:{}",
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

ElementHeader::ElementType ReadableStationSessionKeyArray::GetElementType() const {
    return ElementHeader::StationSessionKey;
}

bool ReadableStationSessionKeyArray::IsPresent() const {
    return count > 0;
}
