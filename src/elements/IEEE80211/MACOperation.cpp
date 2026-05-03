#include "MACOperation.h"
#include "Logging.h"
#include "lassert.h"
#include <algorithm>
#include <cstring>

MACOperation::MACOperation(uint8_t radio_id,
                           uint16_t rts_threshold,
                           uint8_t short_retry,
                           uint8_t long_retry,
                           uint16_t fragmentation_threshold,
                           uint32_t tx_msdu_lifetime,
                           uint32_t rx_msdu_lifetime)
    : ElementHeader(ElementHeader::MACOperation, sizeof(MACOperation) - sizeof(ElementHeader)),
      radio_id{ radio_id }, reserved{ 0 }, rts_threshold{ rts_threshold },
      short_retry{ short_retry }, long_retry{ long_retry },
      fragmentation_threshold{ fragmentation_threshold }, tx_msdu_lifetime{ tx_msdu_lifetime },
      rx_msdu_lifetime{ rx_msdu_lifetime } {
}

uint8_t MACOperation::GetRadioID() const {
    return radio_id;
}

uint16_t MACOperation::GetRTSThreshold() const {
    return rts_threshold.Get();
}

uint8_t MACOperation::GetShortRetry() const {
    return short_retry;
}

uint8_t MACOperation::GetLongRetry() const {
    return long_retry;
}

uint16_t MACOperation::GetFragmentationThreshold() const {
    return fragmentation_threshold.Get();
}

uint32_t MACOperation::GetTxMSDULifetime() const {
    return tx_msdu_lifetime.Get();
}

uint32_t MACOperation::GetRxMSDULifetime() const {
    return rx_msdu_lifetime.Get();
}

bool MACOperation::Validate() const {
    static_assert(sizeof(MACOperation) == 20);
    if (ElementHeader::GetElementType() != ElementHeader::MACOperation) {
        return false;
    }
    if (ElementHeader::GetLength() != (sizeof(MACOperation) - sizeof(ElementHeader))) {
        return false;
    }
    if (radio_id > 31) {
        return false;
    }
    return true;
}

WritableMACOperationArray::WritableMACOperationArray() {
    static_assert(sizeof(items[0]) == 20);
    items.reserve(ReadableMACOperationArray::max_count);
}

void WritableMACOperationArray::Add(MACOperation op) {
    ASSERT(items.size() + 1 <= ReadableMACOperationArray::max_count);

    auto it_exists = std::find_if(items.begin(), items.end(), [&op](const MACOperation &item) {
        return item.GetRadioID() == op.GetRadioID();
    });
    if (it_exists != items.end()) {
        *it_exists = std::move(op);
        log_i("MACOperation: replace RadioID: %u", (*it_exists).GetRadioID());
    } else {
        items.emplace_back(std::move(op));
    }
}

bool WritableMACOperationArray::Empty() const {
    return items.empty();
}

void WritableMACOperationArray::Clear() {
    items.clear();
}

void WritableMACOperationArray::Serialize(RawData *raw_data) const {
    for (const auto &item : items) {
        ASSERT(raw_data->current + sizeof(item) <= raw_data->end);
        std::memcpy(raw_data->current, &item, sizeof(item));
        raw_data->current += sizeof(item);
    }
}

void WritableMACOperationArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME MACOperation #%zu RadioID:%u, RTSThreshold:%u, ShortRetry:%u, LongRetry:%u, "
              "FragThreshold:%u, TxMSDULifetime:%u, RxMSDULifetime:%u",
              i,
              items[i].GetRadioID(),
              items[i].GetRTSThreshold(),
              items[i].GetShortRetry(),
              items[i].GetLongRetry(),
              items[i].GetFragmentationThreshold(),
              items[i].GetTxMSDULifetime(),
              items[i].GetRxMSDULifetime());
    }
}

ReadableMACOperationArray::ReadableMACOperationArray() : count{ 0 } {
}

bool ReadableMACOperationArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableMACOperationArray::Deserialize elements count exceeds");
        return false;
    }
    if (raw_data->current + sizeof(MACOperation) > raw_data->end) {
        return false;
    }

    auto item = (MACOperation *)raw_data->current;
    if (!item->Validate()) {
        return false;
    }
    raw_data->current += sizeof(MACOperation);

    items[count] = item;
    count++;
    return true;
}

nonstd::span<const MACOperation *const> ReadableMACOperationArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableMACOperationArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME MACOperation #%zu RadioID:%u, RTSThreshold:%u, ShortRetry:%u, LongRetry:%u, "
              "FragThreshold:%u, TxMSDULifetime:%u, RxMSDULifetime:%u",
              i,
              items[i]->GetRadioID(),
              items[i]->GetRTSThreshold(),
              items[i]->GetShortRetry(),
              items[i]->GetLongRetry(),
              items[i]->GetFragmentationThreshold(),
              items[i]->GetTxMSDULifetime(),
              items[i]->GetRxMSDULifetime());
    }
}
