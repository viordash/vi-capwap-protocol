#pragma once

#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <vector>

struct __attribute__((packed)) DecryptionErrorReportPeriod : ElementHeader {
  private:
    uint8_t radio_id;
    NetworkU16 report_interval;

  public:
    DecryptionErrorReportPeriod(const DecryptionErrorReportPeriod &) = default;
    DecryptionErrorReportPeriod(uint8_t radio_id, uint16_t report_interval);

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static DecryptionErrorReportPeriod *Deserialize(RawData *raw_data);

    // Radio ID:   The Radio Identifier refers to an interface index on the WTP, whose value is between one (1) and 31.
    uint8_t RadioID() const;

    // Report Interval:   A 16-bit unsigned integer indicating the time, in seconds.
    uint16_t ReportInterval() const;
};

struct WritableDecryptionErrorReportPeriodArray {
  private:
    std::vector<DecryptionErrorReportPeriod> items;

  public:
    WritableDecryptionErrorReportPeriodArray(const WritableDecryptionErrorReportPeriodArray &) =
        delete;
    WritableDecryptionErrorReportPeriodArray();

    void Add(DecryptionErrorReportPeriod period);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const;
    void Log() const;
};

struct ReadableDecryptionErrorReportPeriodArray {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const DecryptionErrorReportPeriod *, max_count> items;
    size_t count;

  public:
    ReadableDecryptionErrorReportPeriodArray(const ReadableDecryptionErrorReportPeriodArray &) =
        delete;
    ReadableDecryptionErrorReportPeriodArray();

    bool Deserialize(RawData *raw_data);
    nonstd::span<const DecryptionErrorReportPeriod *const> Get() const;
    void Log() const;
};