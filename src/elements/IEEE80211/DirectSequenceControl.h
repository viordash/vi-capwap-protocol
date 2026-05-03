#pragma once

#include "Helpers.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <vector>

struct __attribute__((packed)) DirectSequenceControl : ElementHeader {
  public:
    // Current CCA Mode values from RFC 5416
    enum CCAMode : uint8_t {
        EdOnly = 1,      // energy detect only
        CsOnly = 2,      // carrier sense only
        EdAndCs = 4,     // carrier sense and energy detect
        CsWithTimer = 8, // carrier sense with timer
        HrCsAndEd = 16   // high rate carrier sense and energy detect
    };

  protected:
    // Radio ID: An 8-bit value representing the radio, whose value is between 0 and 31.
    uint8_t radio_id;

    // Reserved: Must be set to zero.
    uint8_t reserved;

    // Current Channel: Current operating frequency channel of the DSSS PHY.
    uint8_t current_channel;

    // Current CCA: Current Clear Channel Assessment method in operation.
    CCAMode current_cca;

    // Energy Detect Threshold: Current Energy Detect Threshold used by the DSSS PHY.
    NetworkU32 energy_detect_threshold;

  public:
    DirectSequenceControl(const DirectSequenceControl &) = default;
    DirectSequenceControl(uint8_t radio_id,
                          uint8_t current_channel,
                          CCAMode current_cca,
                          uint32_t energy_detect_threshold);

    uint8_t GetRadioID() const;
    uint8_t GetCurrentChannel() const;
    CCAMode GetCurrentCCA() const;
    uint32_t GetEnergyDetectThreshold() const;

    bool Validate() const;
};

struct WritableDirectSequenceControlArray {
  private:
    std::vector<DirectSequenceControl> items;

  public:
    WritableDirectSequenceControlArray(const WritableDirectSequenceControlArray &) = delete;
    WritableDirectSequenceControlArray();

    void Add(DirectSequenceControl ctrl);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const;
    void Log() const;
};

struct ReadableDirectSequenceControlArray {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const DirectSequenceControl *, max_count> items;
    size_t count;

  public:
    ReadableDirectSequenceControlArray(const ReadableDirectSequenceControlArray &) = delete;
    ReadableDirectSequenceControlArray();

    bool Deserialize(RawData *raw_data);
    nonstd::span<const DirectSequenceControl *const> Get() const;
    void Log() const;
};
