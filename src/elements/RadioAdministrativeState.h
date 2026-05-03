#pragma once

#include "IElement.h"
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
};

struct WritableRadioAdministrativeStateArray : IWritableConfigurationUpdateRequestOptionalElement {
  private:
    std::vector<RadioAdministrativeState> items;

  public:
    WritableRadioAdministrativeStateArray(const WritableRadioAdministrativeStateArray &) = delete;
    WritableRadioAdministrativeStateArray();

    void Add(RadioAdministrativeState radio_state);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableRadioAdministrativeStateArray : IReadableConfigurationUpdateRequestOptionalElement {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const RadioAdministrativeState *, max_count> items;
    size_t count;

  public:
    ReadableRadioAdministrativeStateArray(const ReadableRadioAdministrativeStateArray &) = delete;
    ReadableRadioAdministrativeStateArray();

    bool Deserialize(RawData *raw_data) override final;
    nonstd::span<const RadioAdministrativeState *const> Get() const;
    void Log() const override final;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};