#pragma once

#include "IElement.h"
#include "CapwapMessage.h"
#include "elements/VendorSpecificPayload.h"
#include "span.hpp"
#include <limits>
#include <unordered_map>

struct WritableChangeStateEventResponse : WritableCapwapResponse {
  private:
    nonstd::span<IWritableChangeStateEventResponseOptionalElement  *const> optional_elements;

  public:
    WritableChangeStateEventResponse(const WritableChangeStateEventResponse &) = delete;
    WritableChangeStateEventResponse(
        nonstd::span<IWritableChangeStateEventResponseOptionalElement  *const> optional_elements);
    WritableChangeStateEventResponse(
        std::initializer_list<IWritableChangeStateEventResponseOptionalElement  *const> optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetRequestMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableChangeStateEventResponse : ReadableCapwapResponse {
  protected:
    std::unordered_map<ElementHeader::ElementType, IReadableChangeStateEventResponseOptionalElement  *const>
        key_optional_elements;

    std::unordered_map<ElementHeader::ElementType, IReadableChangeStateEventResponseOptionalElement  *const>
    MapOptionalsElements(
        nonstd::span<IReadableChangeStateEventResponseOptionalElement  *const> optional_elements);

  public:

    size_t unknown_elements;

    ReadableChangeStateEventResponse(const ReadableChangeStateEventResponse &) = delete;
    ReadableChangeStateEventResponse(
        nonstd::span<IReadableChangeStateEventResponseOptionalElement  *const> optional_elements);
    ReadableChangeStateEventResponse(
        std::initializer_list<IReadableChangeStateEventResponseOptionalElement  *> optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;
};
