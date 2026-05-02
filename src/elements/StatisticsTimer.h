#pragma once

#include "IElement.h"
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

    uint16_t GetValue() const;
    void Log() const;
};

struct WritableStatisticsTimer : IWritableElement {
  protected:
    StatisticsTimer element;

  public:
    WritableStatisticsTimer(uint16_t time);

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableStatisticsTimer : IReadableElement {
  protected:
    StatisticsTimer *element = nullptr;
    bool is_present = false;

  public:
    bool Deserialize(RawData *raw_data) override final;
    void Log() const override final;
    const StatisticsTimer *const Get() const;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};