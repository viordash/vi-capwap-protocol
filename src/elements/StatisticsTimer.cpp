#include "StatisticsTimer.h"
#include "Logging.h"
#include "lassert.h"
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

uint16_t StatisticsTimer::GetValue() const {
    return time.Get();
}

void StatisticsTimer::Log() const {
    log_i("ME StatisticsTimer :{} secs", GetValue());
}

WritableStatisticsTimer::WritableStatisticsTimer(uint16_t time) : element{ time } {
    static_assert(sizeof(element) == 6);
}

void WritableStatisticsTimer::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(StatisticsTimer) <= raw_data->end);
    std::memcpy(raw_data->current, &element, sizeof(element));
    raw_data->current += sizeof(element);
}

void WritableStatisticsTimer::Log() const {
    element.Log();
}

bool ReadableStatisticsTimer::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(StatisticsTimer) > raw_data->end) {
        return false;
    }

    auto res = (StatisticsTimer *)raw_data->current;
    if (!res->Validate()) {
        return false;
    }
    raw_data->current += sizeof(StatisticsTimer);

    element = res;
    is_present = true;
    return true;
}

const StatisticsTimer *ReadableStatisticsTimer::Get() const {
    return element;
}

void ReadableStatisticsTimer::Log() const {
    ASSERT(element != nullptr);
    element->Log();
}

ElementHeader::ElementType ReadableStatisticsTimer::GetElementType() const {
    return ElementHeader::StatisticsTimer;
}

bool ReadableStatisticsTimer::IsPresent() const {
    return is_present;
}