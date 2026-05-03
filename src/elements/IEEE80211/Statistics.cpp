#include "Statistics.h"
#include "Logging.h"
#include "lassert.h"
#include <algorithm>
#include <cstring>

Statistics::Statistics(uint8_t radio_id,
                       uint32_t tx_fragment_count,
                       uint32_t multicast_tx_count,
                       uint32_t failed_count,
                       uint32_t retry_count,
                       uint32_t multiple_retry_count,
                       uint32_t frame_duplicate_count,
                       uint32_t rts_success_count,
                       uint32_t rts_failure_count,
                       uint32_t ack_failure_count,
                       uint32_t rx_fragment_count,
                       uint32_t multicast_rx_count,
                       uint32_t fcs_error_count,
                       uint32_t tx_frame_count,
                       uint32_t decryption_errors,
                       uint32_t discarded_qos_fragment_count,
                       uint32_t associated_station_count,
                       uint32_t qos_cf_polls_received_count,
                       uint32_t qos_cf_polls_unused_count,
                       uint32_t qos_cf_polls_unusable_count)
    : ElementHeader(ElementHeader::Statistics, sizeof(Statistics) - sizeof(ElementHeader)),
      radio_id{ radio_id }, reserved{ 0, 0, 0 }, tx_fragment_count{ tx_fragment_count },
      multicast_tx_count{ multicast_tx_count }, failed_count{ failed_count },
      retry_count{ retry_count }, multiple_retry_count{ multiple_retry_count },
      frame_duplicate_count{ frame_duplicate_count }, rts_success_count{ rts_success_count },
      rts_failure_count{ rts_failure_count }, ack_failure_count{ ack_failure_count },
      rx_fragment_count{ rx_fragment_count }, multicast_rx_count{ multicast_rx_count },
      fcs_error_count{ fcs_error_count }, tx_frame_count{ tx_frame_count },
      decryption_errors{ decryption_errors },
      discarded_qos_fragment_count{ discarded_qos_fragment_count },
      associated_station_count{ associated_station_count },
      qos_cf_polls_received_count{ qos_cf_polls_received_count },
      qos_cf_polls_unused_count{ qos_cf_polls_unused_count },
      qos_cf_polls_unusable_count{ qos_cf_polls_unusable_count } {
}

uint8_t Statistics::GetRadioID() const {
    return radio_id;
}

uint32_t Statistics::GetTxFragmentCount() const {
    return tx_fragment_count.Get();
}

uint32_t Statistics::GetMulticastTxCount() const {
    return multicast_tx_count.Get();
}

uint32_t Statistics::GetFailedCount() const {
    return failed_count.Get();
}

uint32_t Statistics::GetRetryCount() const {
    return retry_count.Get();
}

uint32_t Statistics::GetMultipleRetryCount() const {
    return multiple_retry_count.Get();
}

uint32_t Statistics::GetFrameDuplicateCount() const {
    return frame_duplicate_count.Get();
}

uint32_t Statistics::GetRTSSuccessCount() const {
    return rts_success_count.Get();
}

uint32_t Statistics::GetRTSFailureCount() const {
    return rts_failure_count.Get();
}

uint32_t Statistics::GetACKFailureCount() const {
    return ack_failure_count.Get();
}

uint32_t Statistics::GetRxFragmentCount() const {
    return rx_fragment_count.Get();
}

uint32_t Statistics::GetMulticastRxCount() const {
    return multicast_rx_count.Get();
}

uint32_t Statistics::GetFCSErrorCount() const {
    return fcs_error_count.Get();
}

uint32_t Statistics::GetTxFrameCount() const {
    return tx_frame_count.Get();
}

uint32_t Statistics::GetDecryptionErrors() const {
    return decryption_errors.Get();
}

uint32_t Statistics::GetDiscardedQoSFragmentCount() const {
    return discarded_qos_fragment_count.Get();
}

uint32_t Statistics::GetAssociatedStationCount() const {
    return associated_station_count.Get();
}

uint32_t Statistics::GetQoSCFPollsReceivedCount() const {
    return qos_cf_polls_received_count.Get();
}

uint32_t Statistics::GetQoSCFPollsUnusedCount() const {
    return qos_cf_polls_unused_count.Get();
}

uint32_t Statistics::GetQoSCFPollsUnusableCount() const {
    return qos_cf_polls_unusable_count.Get();
}

bool Statistics::Validate() const {
    static_assert(sizeof(Statistics) == 84); // 4 header + 80 value
    if (ElementHeader::GetElementType() != ElementHeader::Statistics) {
        return false;
    }
    if (ElementHeader::GetLength() != (sizeof(Statistics) - sizeof(ElementHeader))) {
        return false;
    }
    if (radio_id > 31) {
        return false;
    }
    return true;
}

WritableStatisticsArray::WritableStatisticsArray() {
    static_assert(sizeof(items[0]) == 84);
    items.reserve(ReadableStatisticsArray::max_count);
}

void WritableStatisticsArray::Add(Statistics element) {
    ASSERT(items.size() + 1 <= ReadableStatisticsArray::max_count);

    auto it_exists = std::find_if(items.begin(), items.end(), [&element](const Statistics &item) {
        return item.GetRadioID() == element.GetRadioID();
    });
    if (it_exists != items.end()) {
        *it_exists = std::move(element);
        log_i("Statistics: replace RadioID: %u", (*it_exists).GetRadioID());
    } else {
        items.emplace_back(std::move(element));
    }
}

bool WritableStatisticsArray::Empty() const {
    return items.empty();
}

void WritableStatisticsArray::Clear() {
    items.clear();
}

void WritableStatisticsArray::Serialize(RawData *raw_data) const {
    for (const auto &item : items) {
        ASSERT(raw_data->current + sizeof(item) <= raw_data->end);
        std::memcpy(raw_data->current, &item, sizeof(item));
        raw_data->current += sizeof(item);
    }
}

void WritableStatisticsArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME Statistics #%zu RadioID:%u, TxFragmentCount:%u, MulticastTxCount:%u, "
              "FailedCount:%u, RetryCount:%u, AssociatedStationCount:%u",
              i,
              items[i].GetRadioID(),
              items[i].GetTxFragmentCount(),
              items[i].GetMulticastTxCount(),
              items[i].GetFailedCount(),
              items[i].GetRetryCount(),
              items[i].GetAssociatedStationCount());
    }
}

ReadableStatisticsArray::ReadableStatisticsArray() : count{ 0 } {
}

ElementHeader::ElementType ReadableStatisticsArray::GetElementType() const {
    return ElementHeader::Statistics;
}

bool ReadableStatisticsArray::IsPresent() const {
    return count > 0;
}


bool ReadableStatisticsArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableStatisticsArray::Deserialize elements count exceeds");
        return false;
    }

    if (raw_data->current + sizeof(Statistics) > raw_data->end) {
        return false;
    }

    auto item = (Statistics *)raw_data->current;
    if (!item->Validate()) {
        return false;
    }
    raw_data->current += sizeof(Statistics);

    items[count] = item;
    count++;
    return true;
}

nonstd::span<const Statistics *const> ReadableStatisticsArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableStatisticsArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME Statistics #%zu RadioID:%u, TxFragmentCount:%u, MulticastTxCount:%u, "
              "FailedCount:%u, RetryCount:%u, AssociatedStationCount:%u",
              i,
              items[i]->GetRadioID(),
              items[i]->GetTxFragmentCount(),
              items[i]->GetMulticastTxCount(),
              items[i]->GetFailedCount(),
              items[i]->GetRetryCount(),
              items[i]->GetAssociatedStationCount());
    }
}
