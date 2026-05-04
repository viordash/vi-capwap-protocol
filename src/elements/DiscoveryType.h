#pragma once

#include "IElement.h"
#include "elements/ElementHeader.h"
#include <cstdint>

struct __attribute__((packed)) DiscoveryType : ElementHeader {
    enum Type : uint8_t { Unknown = 0, StaticConfiguration = 1, DHCP = 2, DNS = 3, ACReferral = 4 };

    Type type;

    DiscoveryType(const DiscoveryType &) = default;
    DiscoveryType(DiscoveryType &&) = default;
    DiscoveryType(Type type);

    bool Validate() const;
    void Log() const;
};

struct WritableDiscoveryType : IWritableElement {
  protected:
    DiscoveryType element;

  public:
    WritableDiscoveryType(DiscoveryType::Type type);

    void Serialize(RawData *raw_data) const override final;
    uint16_t GetTotalLength() const;
    void Log() const override final;
};

struct ReadableDiscoveryType : IReadableElement {
  protected:
    DiscoveryType *element = nullptr;
    bool is_present = false;

  public:
    bool Deserialize(RawData *raw_data) override final;
    void Log() const override final;
    DiscoveryType *const Get() const;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};