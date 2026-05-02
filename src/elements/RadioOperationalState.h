#pragma once

#include "IElement.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <vector>

struct __attribute__((packed)) RadioOperationalState : ElementHeader {
    enum States : uint8_t { Reserved = 0, Enabled = 1, Disabled = 2 };
    enum Causes : uint8_t {
        Normal = 0,
        RadioFailure = 1,
        SoftwareFailure = 2,
        AdministrativelySet = 3
    };

    // The Radio Identifier refers to an interface index on the WTP, whose value is between one (1) and 31.
    // A value of 0xFF is invalid, as it is not possible to change the WTP's operational state.
    uint8_t RadioID;

    // State:   An 8-bit Boolean value representing the state of the radio
    States State;

    // Cause:   When a radio is inoperable, the cause field contains the reason the radio is out of service
    Causes Cause;

    RadioOperationalState(const RadioOperationalState &) = default;
    RadioOperationalState(const uint8_t radio_id, const States state, const Causes cause);
    bool Validate() const;
};

struct WritableRadioOperationalStateArray : IWritableElement {
  private:
    std::vector<RadioOperationalState> items;

  public:
    WritableRadioOperationalStateArray(const WritableRadioOperationalStateArray &) = delete;
    WritableRadioOperationalStateArray();

    void Add(RadioOperationalState radio_state);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableRadioOperationalStateArray : IReadableElement {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const RadioOperationalState *, max_count> items;
    size_t count;

  public:
    ReadableRadioOperationalStateArray(const ReadableRadioOperationalStateArray &) = delete;
    ReadableRadioOperationalStateArray();

    bool Deserialize(RawData *raw_data) override final;
    nonstd::span<const RadioOperationalState *const> Get() const;
    void Log() const override final;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};