#pragma once
#include "IOptionalElement.h"
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <string_view>

struct __attribute__((packed)) IdleTimeout : ElementHeader {
  protected:
    NetworkU32 timeout;

  public:
    IdleTimeout(const IdleTimeout &) = default;
    IdleTimeout(IdleTimeout &&) = default;
    IdleTimeout(uint32_t timeout);

    uint32_t GetTimeout() const;
    bool Validate() const;
    void Log() const;
};

struct WritableIdleTimeout : IWritableOptionalElement {
  protected:
    IdleTimeout element;

  public:
    WritableIdleTimeout(uint32_t timeout);

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableIdleTimeout : IReadableOptionalElement {
  protected:
    IdleTimeout *element = nullptr;
    bool is_present = false;

  public:
    bool Deserialize(RawData *raw_data) override final;
    void Log() const override final;
    const IdleTimeout *const Get() const;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};