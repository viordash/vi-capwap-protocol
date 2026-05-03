#include "TxPower.h"
#include "Logging.h"
#include "lassert.h"
#include <cstring>

TxPower::TxPower(uint8_t radio_id, uint16_t current_tx_power)
    : ElementHeader(ElementHeader::TxPower, sizeof(TxPower) - sizeof(ElementHeader)),
      RadioID{ radio_id }, Reserved{ 0 }, CurrentTxPower{ current_tx_power } {
}

bool TxPower::Validate() const {
    static_assert(sizeof(TxPower) == 8);
    if (ElementHeader::GetElementType() != ElementHeader::TxPower) {
        return false;
    }
    if (ElementHeader::GetLength() != (sizeof(TxPower) - sizeof(ElementHeader))) {
        return false;
    }
    if (RadioID > 31) {
        return false;
    }
    return true;
}


WritableTxPowerArray::WritableTxPowerArray() {
    static_assert(sizeof(items[0]) == 8);
    items.reserve(ReadableTxPowerArray::max_count);
}

void WritableTxPowerArray::Add(TxPower element) {
    ASSERT(items.size() + 1 <= ReadableTxPowerArray::max_count);
    items.emplace_back(std::move(element));
}

bool WritableTxPowerArray::Empty() const {
    return items.empty();
}

void WritableTxPowerArray::Clear() {
    items.clear();
}

void WritableTxPowerArray::Serialize(RawData *raw_data) const {
    for (const auto &item : items) {
        ASSERT(raw_data->current + sizeof(item) <= raw_data->end);
        std::memcpy(raw_data->current, &item, sizeof(item));
        raw_data->current += sizeof(item);
    }
}

void WritableTxPowerArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME TxPower #%zu RadioID:%u, CurrentTxPower:%u",
              i,
              items[i].RadioID,
              items[i].CurrentTxPower.Get());
    }
}

ReadableTxPowerArray::ReadableTxPowerArray() : count{ 0 } {
}

ElementHeader::ElementType ReadableTxPowerArray::GetElementType() const {
    return ElementHeader::TxPower;
}

bool ReadableTxPowerArray::IsPresent() const {
    return count > 0;
}


bool ReadableTxPowerArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableTxPowerArray::Deserialize elements count exceeds");
        return false;
    }

    if (raw_data->current + sizeof(TxPower) > raw_data->end) {
        return false;
    }

    auto item = (TxPower *)raw_data->current;
    if (!item->Validate()) {
        return false;
    }
    raw_data->current += sizeof(TxPower);

    items[count] = item;
    count++;
    return true;
}

nonstd::span<const TxPower *const> ReadableTxPowerArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableTxPowerArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME TxPower #%zu RadioID:%u, CurrentTxPower:%u",
              i,
              items[i]->RadioID,
              items[i]->CurrentTxPower.Get());
    }
}
