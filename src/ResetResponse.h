#pragma once

#include "IElement.h"
#include "CapwapMessage.h"
#include "elements/ResultCode.h"
#include "elements/VendorSpecificPayload.h"
#include "span.hpp"
#include <limits>
#include <unordered_map>

struct WritableResetResponse : WritableCapwapResponse {

  private:
    nonstd::span<IWritableResetResponseOptionalElement *const> optional_elements;

  public:
    WritableResetResponse(const WritableResetResponse &) = delete;
    WritableResetResponse(
        nonstd::span<IWritableResetResponseOptionalElement *const> optional_elements);
    WritableResetResponse(
        std::initializer_list<IWritableResetResponseOptionalElement *const> optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetRequestMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableResetResponse : ReadableCapwapResponse {
  protected:
    std::unordered_map<ElementHeader::ElementType, IReadableResetResponseOptionalElement *const>
        key_optional_elements;

    std::unordered_map<ElementHeader::ElementType, IReadableResetResponseOptionalElement *const>
    MapOptionalsElements(
        nonstd::span<IReadableResetResponseOptionalElement *const> optional_elements);

  public:

    size_t unknown_elements;

    ReadableResetResponse(const ReadableResetResponse &) = delete;
    ReadableResetResponse(
        nonstd::span<IReadableResetResponseOptionalElement *const> optional_elements);
    ReadableResetResponse(
        std::initializer_list<IReadableResetResponseOptionalElement *> optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;
};
