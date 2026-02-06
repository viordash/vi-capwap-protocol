#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <vector>

struct __attribute__((packed)) WTPRadioStatistics : ElementHeader {
  public:
    enum LastFailureType : uint8_t {
        SoftwareFailure = 1,
        HardwareFailure = 2,
        OtherFailure = 3,
        Unknown = 255
    };

  private:
    //Radio ID: The radio ID of the radio to which the statistics apply, whose value is between one (1) and 31.
    uint8_t radio_id;

    // Last Failure Type: The last WTP failure
    LastFailureType last_failure_type;

    // Reset Count: The number of times that the radio has been reset.
    NetworkU16 reset_count;

    // SW Failure Count: The number of times that the radio has failed due to software - related reasons.uint8_t sw_failure_count_0;
    NetworkU16 sw_failure_count;

    // HW Failure Count: The number of times that the radio has failed due to hardware-related reasons.
    NetworkU16 hw_failure_count;

    // Other Failure Count: The number of times that the radio has failed due to known reasons, other than software or hardware failure.
    NetworkU16 other_failure_count;

    // Unknown Failure Count: The number of times that the radio has failed for unknown reasons.
    NetworkU16 unknown_failure_count;

    // Config Update Count: The number of times that the radio configuration has been updated.
    NetworkU16 config_update_count;

    // Channel Change Count: The number of times that the radio channel has been changed.
    NetworkU16 channel_change_count;

    // Band Change Count: The number of times that the radio has changed frequency bands.
    NetworkU16 band_change_count;

    // Current Noise Floor: A signed integer that indicates the noise floor of the radio receiver in units of dBm.
    NetworkS16 current_noise_floor;

  public:
    WTPRadioStatistics(const WTPRadioStatistics &) = default;
    WTPRadioStatistics(uint8_t radio_id,
                       LastFailureType last_failure_type,
                       uint16_t reset_count,
                       uint16_t sw_failure_count,
                       uint16_t hw_failure_count,
                       uint16_t other_failure_count,
                       uint16_t unknown_failure_count,
                       uint16_t config_update_count,
                       uint16_t channel_change_count,
                       uint16_t band_change_count,
                       int16_t current_noise_floor);

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static WTPRadioStatistics *Deserialize(RawData *raw_data);

    uint8_t GetRadioID() const;
    LastFailureType GetLastFailureType() const;
    uint16_t GetResetCount() const;
    uint16_t GetSWFailureCount() const;
    uint16_t GetHWFailureCount() const;
    uint16_t GetOtherFailureCount() const;
    uint16_t GetUnknownFailureCount() const;
    uint16_t GetConfigUpdateCount() const;
    uint16_t GetChannelChangeCount() const;
    uint16_t GetBandChangeCount() const;
    int16_t GetCurrentNoiseFloor() const;

    uint16_t GetTotalLength() const;

    bool operator==(const WTPRadioStatistics &other) const;
};

struct WritableWTPRadioStatisticsArray {
  private:
    std::vector<WTPRadioStatistics> items;

  public:
    WritableWTPRadioStatisticsArray(const WritableWTPRadioStatisticsArray &) = delete;
    WritableWTPRadioStatisticsArray();

    void Add(WTPRadioStatistics statistics);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const;
    uint16_t GetTotalLength() const;
    void Log() const;
};

struct ReadableWTPRadioStatisticsArray {
  public:
    static const size_t max_count = 32; //Radio ID

  protected:
    std::array<const WTPRadioStatistics *, max_count> items;
    size_t count;

  public:
    ReadableWTPRadioStatisticsArray(const ReadableWTPRadioStatisticsArray &) = delete;
    ReadableWTPRadioStatisticsArray();

    bool Deserialize(RawData *raw_data);
    nonstd::span<const WTPRadioStatistics *const> Get() const;
    void Log() const;
};
