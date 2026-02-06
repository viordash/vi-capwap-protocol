
#include "WTPRadioStatistics.h"
#include "lassert.h"
#include "logging.h"
#include <string.h>

WTPRadioStatistics::WTPRadioStatistics(uint8_t radio_id,
                                       LastFailureType last_failure_type,
                                       uint16_t reset_count,
                                       uint16_t sw_failure_count,
                                       uint16_t hw_failure_count,
                                       uint16_t other_failure_count,
                                       uint16_t unknown_failure_count,
                                       uint16_t config_update_count,
                                       uint16_t channel_change_count,
                                       uint16_t band_change_count,
                                       int16_t current_noise_floor)
    : ElementHeader(ElementHeader::WTPRadioStatistics,
                    sizeof(WTPRadioStatistics) - sizeof(ElementHeader)),
      radio_id{ radio_id }, last_failure_type{ last_failure_type },

      reset_count{ reset_count }, sw_failure_count{ sw_failure_count },
      hw_failure_count{ hw_failure_count }, other_failure_count{ other_failure_count },
      unknown_failure_count{ unknown_failure_count }, config_update_count{ config_update_count },
      channel_change_count{ channel_change_count }, band_change_count{ band_change_count },
      current_noise_floor{ current_noise_floor } {
}
bool WTPRadioStatistics::Validate() const {
    static_assert(sizeof(WTPRadioStatistics) == 24);

    if (GetElementType() != ElementHeader::WTPRadioStatistics) {
        return false;
    }
    if (GetLength() != (sizeof(WTPRadioStatistics) - sizeof(ElementHeader))) {
        return false;
    }

    switch (last_failure_type) {
        case LastFailureType::SoftwareFailure:
        case LastFailureType::HardwareFailure:
        case LastFailureType::OtherFailure:
        case LastFailureType::Unknown:
            return true;
        default:
            return false;
    };
}
void WTPRadioStatistics::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(WTPRadioStatistics) <= raw_data->end);
    WTPRadioStatistics *dst = (WTPRadioStatistics *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(WTPRadioStatistics);
}
WTPRadioStatistics *WTPRadioStatistics::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(WTPRadioStatistics) > raw_data->end) {
        return nullptr;
    }

    auto res = (WTPRadioStatistics *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += sizeof(WTPRadioStatistics);
    return res;
}

uint8_t WTPRadioStatistics::GetRadioID() const {
    return radio_id;
}
WTPRadioStatistics::LastFailureType WTPRadioStatistics::GetLastFailureType() const {
    return last_failure_type;
}
uint16_t WTPRadioStatistics::GetResetCount() const {
    return reset_count.Get();
}
uint16_t WTPRadioStatistics::GetSWFailureCount() const {
    return sw_failure_count.Get();
}
uint16_t WTPRadioStatistics::GetHWFailureCount() const {
    return hw_failure_count.Get();
}
uint16_t WTPRadioStatistics::GetOtherFailureCount() const {
    return other_failure_count.Get();
}
uint16_t WTPRadioStatistics::GetUnknownFailureCount() const {
    return unknown_failure_count.Get();
}
uint16_t WTPRadioStatistics::GetConfigUpdateCount() const {
    return config_update_count.Get();
}
uint16_t WTPRadioStatistics::GetChannelChangeCount() const {
    return channel_change_count.Get();
}
uint16_t WTPRadioStatistics::GetBandChangeCount() const {
    return band_change_count.Get();
}
int16_t WTPRadioStatistics::GetCurrentNoiseFloor() const {
    return current_noise_floor.Get();
}
uint16_t WTPRadioStatistics::GetTotalLength() const {
    return GetLength() + sizeof(ElementHeader);
}

bool WTPRadioStatistics::operator==(const WTPRadioStatistics &other) const {
    return radio_id == other.radio_id && last_failure_type == other.last_failure_type
        && last_failure_type == other.last_failure_type && reset_count == other.reset_count
        && sw_failure_count == other.sw_failure_count && hw_failure_count == other.hw_failure_count
        && other_failure_count == other.other_failure_count
        && unknown_failure_count == other.unknown_failure_count
        && config_update_count == other.config_update_count
        && channel_change_count == other.channel_change_count
        && band_change_count == other.band_change_count
        && current_noise_floor == other.current_noise_floor;
}

WritableWTPRadioStatisticsArray::WritableWTPRadioStatisticsArray() {
    items.reserve(ReadableWTPRadioStatisticsArray::max_count);
}

void WritableWTPRadioStatisticsArray::Add(WTPRadioStatistics statistics) {
    ASSERT(items.size() + 1 <= ReadableWTPRadioStatisticsArray::max_count);

    auto it_exists =
        std::find_if(items.begin(), items.end(), [&statistics](const WTPRadioStatistics &item) {
            return item == statistics;
        });
    if (it_exists != items.end()) {
        *it_exists = std::move(statistics);
        log_i("WTPRadioStatistics: replace Radio ID:%u, Last Failure Type:%u, Reset Count:%u, SW "
              "Failure "
              "Count:%u, HW Failure Count:%u, "
              "Other Failure Count:%u, Unknown Failure "
              "Count:%u, Config Update Count:%u, Channel Change Count:%u, Band Change Count:%u, "
              "Current Noise Floor:%d",
              (*it_exists).GetRadioID(),
              (unsigned)(*it_exists).GetLastFailureType(),
              (*it_exists).GetResetCount(),
              (*it_exists).GetSWFailureCount(),
              (*it_exists).GetHWFailureCount(),
              (*it_exists).GetOtherFailureCount(),
              (*it_exists).GetUnknownFailureCount(),
              (*it_exists).GetConfigUpdateCount(),
              (*it_exists).GetChannelChangeCount(),
              (*it_exists).GetBandChangeCount(),
              (*it_exists).GetCurrentNoiseFloor());
    } else {
        items.emplace_back(std::move(statistics));
    }
}

bool WritableWTPRadioStatisticsArray::Empty() const {
    return items.empty();
}

void WritableWTPRadioStatisticsArray::Clear() {
    items.clear();
}

void WritableWTPRadioStatisticsArray::Serialize(RawData *raw_data) const {
    ASSERT(items.size() <= ReadableWTPRadioStatisticsArray::max_count);
    for (const auto &elem : items) {
        elem.Serialize(raw_data);
    }
}

uint16_t WritableWTPRadioStatisticsArray::GetTotalLength() const {
    uint16_t size = 0;
    for (const auto &elem : items) {
        size += elem.GetTotalLength();
    }
    return size;
}

void WritableWTPRadioStatisticsArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME WTPRadioStatistics #%lu Radio ID:%u, Last Failure Type:%u, Reset Count:%u, SW "
              "Failure "
              "Count:%u, HW Failure Count:%u, "
              "Other Failure Count:%u, Unknown Failure "
              "Count:%u, Config Update Count:%u, Channel Change Count:%u, Band Change Count:%u, "
              "Current Noise Floor:%d",
              i,
              items[i].GetRadioID(),
              (unsigned)items[i].GetLastFailureType(),
              items[i].GetResetCount(),
              items[i].GetSWFailureCount(),
              items[i].GetHWFailureCount(),
              items[i].GetOtherFailureCount(),
              items[i].GetUnknownFailureCount(),
              items[i].GetConfigUpdateCount(),
              items[i].GetChannelChangeCount(),
              items[i].GetBandChangeCount(),
              items[i].GetCurrentNoiseFloor());
    }
}

ReadableWTPRadioStatisticsArray::ReadableWTPRadioStatisticsArray() : count{ 0 } {
}

bool ReadableWTPRadioStatisticsArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableWTPRadioStatisticsArray::Deserialize elements count exceeds");
        return false;
    }

    auto radio_info = WTPRadioStatistics::Deserialize(raw_data);
    if (radio_info == nullptr) {
        return false;
    }
    items[count] = radio_info;
    count++;
    return true;
}

nonstd::span<const WTPRadioStatistics *const> ReadableWTPRadioStatisticsArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableWTPRadioStatisticsArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME WTPRadioStatistics #%lu Radio ID:%u, Last Failure Type:%u, Reset Count:%u, SW "
              "Failure "
              "Count:%u, HW Failure Count:%u, "
              "Other Failure Count:%u, Unknown Failure "
              "Count:%u, Config Update Count:%u, Channel Change Count:%u, Band Change Count:%u, "
              "Current Noise Floor:%d",
              i,
              items[i]->GetRadioID(),
              (unsigned)items[i]->GetLastFailureType(),
              items[i]->GetResetCount(),
              items[i]->GetSWFailureCount(),
              items[i]->GetHWFailureCount(),
              items[i]->GetOtherFailureCount(),
              items[i]->GetUnknownFailureCount(),
              items[i]->GetConfigUpdateCount(),
              items[i]->GetChannelChangeCount(),
              items[i]->GetBandChangeCount(),
              items[i]->GetCurrentNoiseFloor());
    }
}