#include "TxPowerLevel.h"
#include "Logging.h"
#include "lassert.h"
#include <algorithm>
#include <cstring>

TxPowerLevel::TxPowerLevel(uint8_t radio_id, uint8_t num_levels)
    : ElementHeader(ElementHeader::TxPowerLevel, 2 + num_levels * sizeof(NetworkS16)),
      radio_id{ radio_id }, num_levels{ num_levels } {
}

uint8_t TxPowerLevel::GetRadioID() const {
    return radio_id;
}

uint8_t TxPowerLevel::GetNumLevels() const {
    return num_levels;
}

bool TxPowerLevel::Validate() const {
    if (ElementHeader::GetElementType() != ElementHeader::TxPowerLevel) {
        return false;
    }
    auto length = ElementHeader::GetLength();
    if (length < 4) { // RadioID + NumLevels + at least one PowerLevel
        return false;
    }
    if (radio_id > 31) {
        return false;
    }
    if (num_levels == 0 || num_levels > max_levels) {
        return false;
    }
    if (length != 2 + num_levels * sizeof(NetworkS16)) {
        return false;
    }
    return true;
}

WritableTxPowerLevelArray::WritableTxPowerLevelArray() {
    static_assert(sizeof(Item::header) == 6);
    items.reserve(ReadableTxPowerLevelArray::max_count);
}

void WritableTxPowerLevelArray::Add(Item element) {
    ASSERT(items.size() + 1 <= ReadableTxPowerLevelArray::max_count);
    ASSERT(element.data.size() <= TxPowerLevel::max_levels);

    auto it_exists = std::find_if(items.begin(), items.end(), [&element](const Item &item) {
        return item.header.GetRadioID() == element.header.GetRadioID();
    });

    if (it_exists != items.end()) {
        *it_exists = std::move(element);
        log_i("TxPowerLevel: replace RadioID: %u", (*it_exists).header.GetRadioID());
    } else {
        items.emplace_back(std::move(element));
    }
}

bool WritableTxPowerLevelArray::Empty() const {
    return items.empty();
}

void WritableTxPowerLevelArray::Clear() {
    items.clear();
}

void WritableTxPowerLevelArray::Serialize(RawData *raw_data) const {
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

void WritableTxPowerLevelArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME TxPowerLevel #%zu RadioID:%u, NumLevels:%zu",
              i,
              items[i].header.GetRadioID(),
              items[i].data.size());
    }
}

ReadableTxPowerLevelArray::ReadableTxPowerLevelArray() : count{ 0 } {
}

bool ReadableTxPowerLevelArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableTxPowerLevelArray::Deserialize elements count exceeds");
        return false;
    }

    if (raw_data->current + sizeof(TxPowerLevel) > raw_data->end) {
        return false;
    }

    auto item = (ReadableTxPowerLevelArray::Item *)raw_data->current;
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

nonstd::span<const ReadableTxPowerLevelArray::Item *const> ReadableTxPowerLevelArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableTxPowerLevelArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME TxPowerLevel #%zu RadioID:%u, NumLevels:%u",
              i,
              items[i]->GetRadioID(),
              items[i]->GetNumLevels());
    }
}

ElementHeader::ElementType ReadableTxPowerLevelArray::GetElementType() const {
    return ElementHeader::TxPowerLevel;
}

bool ReadableTxPowerLevelArray::IsPresent() const {
    return count > 0;
}
