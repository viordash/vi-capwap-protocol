#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "IOptionalElement.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <string_view>

struct __attribute__((packed)) ACTimestampHeader : ElementHeader {
  protected:
    NetworkU32 timestamp;

  public:
    ACTimestampHeader(const ACTimestampHeader &) = default;
    ACTimestampHeader(ACTimestampHeader &&) = default;
    ACTimestampHeader(uint32_t timestamp);

    uint32_t GetTimestamp() const;
    bool Validate() const;
    void Log() const;
};

struct WritableACTimestamp : IWritableConfigurationStatusRequestOptionalElement {
  protected:
    ACTimestampHeader element;

  public:
    WritableACTimestamp(uint32_t timestamp);

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableACTimestamp : IReadableConfigurationStatusRequestOptionalElement {
  protected:
    ACTimestampHeader *element = nullptr;
    bool is_present = false;

  public:
    bool Deserialize(RawData *raw_data) override final;
    void Log() const override final;
    const ACTimestampHeader *const Get() const;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};