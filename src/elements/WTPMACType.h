#pragma once

#include "IElement.h"
#include "elements/ElementHeader.h"
#include <cstdint>

struct __attribute__((packed)) WTPMACType : ElementHeader {
    enum Type : uint8_t { Local_MAC = 0, Split_MAC = 1, Both = 2 };

    Type type;

    WTPMACType(const WTPMACType &) = default;
    WTPMACType(Type type);

    bool Validate() const;
    void Log() const;
};

struct WritableWTPMACType : IWritableElement {
  protected:
    WTPMACType element;

  public:
    WritableWTPMACType(WTPMACType::Type type);

    void Serialize(RawData *raw_data) const override final;
    uint16_t GetTotalLength() const;
    void Log() const override final;
};

struct ReadableWTPMACType : IReadableElement {
  protected:
    WTPMACType *element = nullptr;
    bool is_present = false;

  public:
    bool Deserialize(RawData *raw_data) override final;
    void Log() const override final;
    const WTPMACType *const Get() const;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};