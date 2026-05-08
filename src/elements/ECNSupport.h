#pragma once

#include "IElement.h"
#include "elements/ElementHeader.h"
#include <cstdint>

struct __attribute__((packed)) ECNSupport : ElementHeader {
    enum Type : uint8_t { LimitedECN = 0, FullAndLimitedECN = 1 };

    Type type;

    ECNSupport(const ECNSupport &) = default;
    ECNSupport(ECNSupport &&) = default;
    ECNSupport(Type type);

    bool Validate() const;
    void Log() const;
};

struct WritableECNSupport : IWritableElement {
  protected:
    ECNSupport element;

  public:
    WritableECNSupport(ECNSupport::Type type);

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableECNSupport : IReadableElement {
  protected:
    ECNSupport *element = nullptr;
    bool is_present = false;

  public:
    bool Deserialize(RawData *raw_data) override final;
    void Log() const override final;
    const ECNSupport *Get() const;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};