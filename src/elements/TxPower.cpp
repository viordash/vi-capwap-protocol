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

void TxPower::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(TxPower) <= raw_data->end);
#pragma GCC diagnostic push
#if __GNUC__ >= 8
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif
    memcpy(raw_data->current, this, sizeof(TxPower));
#pragma GCC diagnostic pop
    raw_data->current += sizeof(TxPower);
}

TxPower *TxPower::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(TxPower) > raw_data->end) {
        return nullptr;
    }

    auto res = (TxPower *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += sizeof(TxPower);
    return res;
}

WritableTxPowerArray::WritableTxPowerArray() {
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
    for (const auto &elem : items) {
        elem.Serialize(raw_data);
    }
}

void WritableTxPowerArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME TxPower #%zu RadioID:%u, CurrentTxPower:%u", i, items[i].RadioID, items[i].CurrentTxPower.Get());
    }
}

ReadableTxPowerArray::ReadableTxPowerArray() : count{ 0 } {
}

bool ReadableTxPowerArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableTxPowerArray::Deserialize elements count exceeds");
        return false;
    }

    auto tp = TxPower::Deserialize(raw_data);
    if (tp == nullptr) {
        return false;
    }
    items[count] = tp;
    count++;
    return true;
}

nonstd::span<const TxPower *const> ReadableTxPowerArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableTxPowerArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME TxPower #%zu RadioID:%u, CurrentTxPower:%u", i, items[i]->RadioID, items[i]->CurrentTxPower.Get());
    }
}
