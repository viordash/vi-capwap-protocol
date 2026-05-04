#pragma once

#include "IElement.h"
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

struct WritableCapwapTransportProtocol : IWritableConfigurationStatusRequestOptionalElement,
                                         IWritableJoinRequestOptionalElement,
                                         IWritableJoinResponseOptionalElement,
                                         IWritableImageDataRequestOptionalElement {
  protected:
    CapwapTransportProtocol element;

  public:
    WritableCapwapTransportProtocol(CapwapTransportProtocol::Type type);

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableCapwapTransportProtocol : IReadableConfigurationStatusRequestOptionalElement,
                                         IReadableJoinRequestOptionalElement,
                                         IReadableJoinResponseOptionalElement,
                                         IReadableImageDataRequestOptionalElement {
  protected:
    CapwapTransportProtocol *element = nullptr;
    bool is_present = false;

  public:
    bool Deserialize(RawData *raw_data) override final;
    void Log() const override final;
    CapwapTransportProtocol *const Get() const;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};