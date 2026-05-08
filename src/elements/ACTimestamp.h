#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "IElement.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <string_view>

struct __attribute__((packed)) ACTimestamp : ElementHeader {
  protected:
    NetworkU32 timestamp;

  public:
    ACTimestamp(const ACTimestamp &) = default;
    ACTimestamp(ACTimestamp &&) = default;
    ACTimestamp(uint32_t timestamp);

    uint32_t GetTimestamp() const;
    bool Validate() const;
    void Log() const;
};

struct WritableACTimestamp : IWritableConfigurationUpdateRequestOptionalElement {
  protected:
    ACTimestamp element;

  public:
    WritableACTimestamp(uint32_t timestamp);

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableACTimestamp : IReadableConfigurationUpdateRequestOptionalElement {
  protected:
    ACTimestamp *element = nullptr;
    bool is_present = false;

  public:
    bool Deserialize(RawData *raw_data) override final;
    void Log() const override final;
    const ACTimestamp *Get() const;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};