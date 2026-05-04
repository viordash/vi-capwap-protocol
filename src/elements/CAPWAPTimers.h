#pragma once

#include "IElement.h"
#include "elements/ElementHeader.h"
#include <cstdint>

struct __attribute__((packed)) CAPWAPTimers : ElementHeader {
    uint8_t Discovery;
    uint8_t EchoInterval;

    CAPWAPTimers(const CAPWAPTimers &) = default;
    CAPWAPTimers(CAPWAPTimers &&) = default;
    CAPWAPTimers(uint8_t discovery, uint8_t echo_interval);

    bool Validate() const;
    void Log() const;
};

struct WritableCAPWAPTimers : IWritableConfigurationUpdateRequestOptionalElement {
  protected:
    CAPWAPTimers element;

  public:
    WritableCAPWAPTimers(uint8_t discovery, uint8_t echo_interval);

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableCAPWAPTimers : IReadableConfigurationUpdateRequestOptionalElement {
  protected:
    CAPWAPTimers *element = nullptr;
    bool is_present = false;

  public:
    bool Deserialize(RawData *raw_data) override final;
    void Log() const override final;
    const CAPWAPTimers *Get() const;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};