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

void TxPowerLevel::Serialize(RawData *raw_data) const {
    size_t total_size = sizeof(ElementHeader) + ElementHeader::GetLength();
    ASSERT(raw_data->current + total_size <= raw_data->end);
#pragma GCC diagnostic push
#if __GNUC__ >= 8
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif
    memcpy(raw_data->current, this, total_size);
#pragma GCC diagnostic pop
    raw_data->current += total_size;
}

TxPowerLevel *TxPowerLevel::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ElementHeader) > raw_data->end) {
        return nullptr;
    }

    auto res = (TxPowerLevel *)raw_data->current;
    size_t total_size = sizeof(ElementHeader) + res->GetLength();
    if (raw_data->current + total_size > raw_data->end) {
        return nullptr;
    }
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += total_size;
    return res;
}

WritableTxPowerLevelArray::Item::Item(uint8_t radio_id, nonstd::span<const int16_t> levels)
    : levels_data(levels), header(radio_id, levels_data.size()) {
}

uint8_t WritableTxPowerLevelArray::Item::GetRadioID() const {
    return header.GetRadioID();
}

WritableTxPowerLevelArray::WritableTxPowerLevelArray() {
    items.reserve(ReadableTxPowerLevelArray::max_count);
}

void WritableTxPowerLevelArray::Add(Item element) {
    ASSERT(items.size() + 1 <= ReadableTxPowerLevelArray::max_count);
    items.emplace_back(std::move(element));
}

bool WritableTxPowerLevelArray::Empty() const {
    return items.empty();
}

void WritableTxPowerLevelArray::Clear() {
    items.clear();
}

void WritableTxPowerLevelArray::Serialize(RawData *raw_data) const {
    for (const auto &elem : items) {
        elem.header.Serialize(raw_data);
        auto *levels_ptr = raw_data->current - elem.levels_data.size() * sizeof(NetworkS16);
        for (size_t i = 0; i < elem.levels_data.size(); i++) {
            NetworkS16 net_value{ elem.levels_data[i] };
            memcpy(levels_ptr + i * sizeof(NetworkS16), &net_value, sizeof(NetworkS16));
        }
    }
}

void WritableTxPowerLevelArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME TxPowerLevel #%zu RadioID:%u, NumLevels:%zu",
              i,
              items[i].GetRadioID(),
              items[i].levels_data.size());
    }
}

ReadableTxPowerLevelArray::ReadableTxPowerLevelArray() : count{ 0 } {
}

bool ReadableTxPowerLevelArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableTxPowerLevelArray::Deserialize elements count exceeds");
        return false;
    }

    auto tpl = TxPowerLevel::Deserialize(raw_data);
    if (tpl == nullptr) {
        return false;
    }
    items[count] = tpl;
    count++;
    return true;
}

nonstd::span<const TxPowerLevel *const> ReadableTxPowerLevelArray::Get() const {
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
