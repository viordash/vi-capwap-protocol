#pragma once

#include "IElement.h"
#include "elements/ElementHeader.h"
#include <cstdint>

struct __attribute__((packed)) WTPFallback : ElementHeader {
    enum Mode : uint8_t { Reserved = 0, Enabled = 1, Disabled = 1 };

    Mode mode;

    WTPFallback(const WTPFallback &) = default;
    WTPFallback(Mode mode);

    bool Validate() const;
    void Log() const;
};

struct WritableWTPFallback : IWritableElement {
  protected:
    WTPFallback element;

  public:
    WritableWTPFallback(WTPFallback::Mode mode);

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableWTPFallback : IReadableElement {
  protected:
    WTPFallback *element = nullptr;
    bool is_present = false;

  public:
    bool Deserialize(RawData *raw_data) override final;
    void Log() const override final;
    const WTPFallback *const Get() const;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};