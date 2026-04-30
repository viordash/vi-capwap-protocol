#pragma once

#include "IOptionalElement.h"
#include "elements/ElementHeader.h"
#include <cstdint>

struct __attribute__((packed)) MaximumMessageLength : ElementHeader {
  private:
    NetworkU16 length;

  public:
    MaximumMessageLength(const MaximumMessageLength &) = delete;
    MaximumMessageLength(uint16_t length);

    bool Validate() const;
    uint16_t GetValue() const;
    void Log() const;
};

struct WritableMaximumMessageLength : IWritableJoinRequestOptionalElement {
  protected:
    MaximumMessageLength element;

  public:
    WritableMaximumMessageLength(uint16_t length);

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableMaximumMessageLength : IReadableJoinRequestOptionalElement {
  protected:
    MaximumMessageLength *element = nullptr;
    bool is_present = false;

  public:
    bool Deserialize(RawData *raw_data) override final;
    void Log() const override final;
    const MaximumMessageLength *const Get() const;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};