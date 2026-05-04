#pragma once
#include "IElement.h"
#include "elements/ElementHeader.h"
#include <cstdint>

struct __attribute__((packed)) MTUDiscoveryPadding : ElementHeader {
    MTUDiscoveryPadding(const MTUDiscoveryPadding &) = delete;
    MTUDiscoveryPadding(uint16_t size);

    bool Validate() const;
};

struct WritableMTUDiscoveryPadding : IWritableElement {
  protected:
    MTUDiscoveryPadding element;

  public:
    WritableMTUDiscoveryPadding(const WritableMTUDiscoveryPadding &) = delete;
    WritableMTUDiscoveryPadding(uint16_t size);

    void Serialize(RawData *raw_data) const override final;
    uint16_t GetLength() const;
    uint16_t GetTotalLength() const;
    void Log() const override final;
};

struct ReadableMTUDiscoveryPadding : IReadableElement {
  public:
    struct Element : MTUDiscoveryPadding {
        uint8_t padding[];
        Element(const Element &) = delete;
    };

  protected:
    Element *element = nullptr;
    bool is_present = false;

  public:
    bool Deserialize(RawData *raw_data) override final;
    void Log() const override final;
    const ReadableMTUDiscoveryPadding::Element *const Get() const;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};
