#include "StatisticsTimer.h"
#include "lassert.h"
#include "logging.h"
#include <cstring>

StatisticsTimer::StatisticsTimer(uint16_t time)
    : ElementHeader(ElementHeader::StatisticsTimer,
                    sizeof(StatisticsTimer) - sizeof(ElementHeader)),
      time{ time } {
}
bool StatisticsTimer::Validate() const {
    static_assert(sizeof(StatisticsTimer) == 6);
    return ElementHeader::GetElementType() == ElementHeader::StatisticsTimer
        && ElementHeader::GetLength() == (sizeof(StatisticsTimer) - sizeof(ElementHeader));
}
void StatisticsTimer::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(StatisticsTimer) <= raw_data->end);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
    memcpy(raw_data->current, this, sizeof(StatisticsTimer));
#pragma GCC diagnostic pop
    raw_data->current += sizeof(StatisticsTimer);
}
StatisticsTimer *StatisticsTimer::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(StatisticsTimer) > raw_data->end) {
        return nullptr;
    }

    auto res = (StatisticsTimer *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += sizeof(StatisticsTimer);
    return res;
}

uint16_t StatisticsTimer::GetValue() const {
    return time.Get();
}

uint16_t StatisticsTimer::GetTotalLength() const {
    return GetLength() + sizeof(ElementHeader);
}

void StatisticsTimer::Log() const {
    log_i("ME StatisticsTimer :%u secs", GetValue());
}