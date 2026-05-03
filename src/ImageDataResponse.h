#pragma once

#include "CapwapMessage.h"
#include "IElement.h"
#include "elements/ImageIdentifier.h"
#include "elements/ImageInformation.h"
#include "elements/InitiateDownload.h"
#include "elements/ResultCode.h"
#include "elements/VendorSpecificPayload.h"
#include "span.hpp"
#include <limits>
#include <unordered_map>

struct WritableImageDataResponse : WritableCapwapResponse {

  private:
    const WritableResultCode result_code;

    nonstd::span<IWritableImageDataResponseOptionalElement *const> optional_elements;

  public:
    WritableImageDataResponse(const WritableImageDataResponse &) = delete;
    WritableImageDataResponse(
        const ResultCode::Type result_code,
        nonstd::span<IWritableImageDataResponseOptionalElement *const> optional_elements);
    WritableImageDataResponse(
        const ResultCode::Type result_code,
        std::initializer_list<IWritableImageDataResponseOptionalElement *const> optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetRequestMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableImageDataResponse : ReadableCapwapResponse {
  protected:
    std::unordered_map<ElementHeader::ElementType, IReadableImageDataResponseOptionalElement *const>
        key_optional_elements;

    std::unordered_map<ElementHeader::ElementType, IReadableImageDataResponseOptionalElement *const>
    MapOptionalsElements(
        nonstd::span<IReadableImageDataResponseOptionalElement *const> optional_elements);

  public:
    ReadableResultCode result_code;

    size_t unknown_elements;

    ReadableImageDataResponse(const ReadableImageDataResponse &) = delete;
    ReadableImageDataResponse(
        nonstd::span<IReadableImageDataResponseOptionalElement *const> optional_elements);
    ReadableImageDataResponse(
        std::initializer_list<IReadableImageDataResponseOptionalElement *> optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;
};
