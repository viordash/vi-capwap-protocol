#pragma once

#include "IOptionalElement.h"
#include "elements/ElementHeader.h"
#include "lassert.h"
#include <cstdint>

struct __attribute__((packed)) CapwapTransportProtocol : ElementHeader {
    enum Type : uint8_t { UDPLite = 1, UDP = 2 };

    Type type;

    CapwapTransportProtocol(const CapwapTransportProtocol &) = delete;
    CapwapTransportProtocol(Type type);

    bool Validate() const;
    void Log() const;
};

struct WritableCapwapTransportProtocol : IWritableConfigurationStatusRequestOptionalElement {
  protected:
    CapwapTransportProtocol element;

  public:
    WritableCapwapTransportProtocol(CapwapTransportProtocol::Type type);

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableCapwapTransportProtocol : IReadableConfigurationStatusRequestOptionalElement {
  protected:
    CapwapTransportProtocol *element = nullptr;
    bool is_present = false;

  public:
    bool Deserialize(RawData *raw_data) override final;
    void Log() const override final;
    const CapwapTransportProtocol *const Get() const;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};