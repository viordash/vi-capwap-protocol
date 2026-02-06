#pragma once

#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <vector>

struct __attribute__((packed)) RadioAdministrativeState : ElementHeader {
    enum States : uint8_t { Reserved = 0, Enabled = 1, Disabled = 2 };

    // Radio ID: An 8-bit value representing the radio to configure, whose value is between one (1) and 31.
    uint8_t RadioID;

    // Admin State: An 8-bit value representing the administrative state of the radio.
    States AdminState;

    RadioAdministrativeState(const RadioAdministrativeState &) = default;
    RadioAdministrativeState(const uint8_t radio_id, const States admin_state);

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static RadioAdministrativeState *Deserialize(RawData *raw_data);
};

struct WritableRadioAdministrativeStateArray {
  private:
    std::vector<RadioAdministrativeState> items;

  public:
    WritableRadioAdministrativeStateArray(const WritableRadioAdministrativeStateArray &) = delete;
    WritableRadioAdministrativeStateArray();

    void Add(RadioAdministrativeState radio_state);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const;
    void Log() const;
};

struct ReadableRadioAdministrativeStateArray {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const RadioAdministrativeState *, max_count> items;
    size_t count;

  public:
    ReadableRadioAdministrativeStateArray(const ReadableRadioAdministrativeStateArray &) = delete;
    ReadableRadioAdministrativeStateArray();

    bool Deserialize(RawData *raw_data);
    nonstd::span<const RadioAdministrativeState *const> Get() const;
    void Log() const;
};