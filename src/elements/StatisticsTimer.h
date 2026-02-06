#pragma once

#include "elements/ElementHeader.h"
#include <cstdint>

struct __attribute__((packed)) StatisticsTimer : ElementHeader {
  private:
    // Statistics Timer: A 16-bit unsigned integer indicating the time, in seconds
    NetworkU16 time;

  public:
    StatisticsTimer(const StatisticsTimer &) = default;
    StatisticsTimer(StatisticsTimer &&) = default;
    StatisticsTimer(uint16_t time);

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static StatisticsTimer *Deserialize(RawData *raw_data);

    uint16_t GetValue() const;
    uint16_t GetTotalLength() const;
    void Log() const;
};