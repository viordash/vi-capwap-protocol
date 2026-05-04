#pragma once

#include "CapwapMessage.h"
#include "elements/CapwapTransportProtocol.h"
#include "elements/ImageData.h"
#include "elements/ImageIdentifier.h"
#include "elements/InitiateDownload.h"
#include "elements/VendorSpecificPayload.h"
#include "span.hpp"
#include <limits>
#include <unordered_map>

struct WritableImageDataRequest : WritableCapwapRequest {

  private:
    nonstd::span<IWritableImageDataRequestOptionalElement *const> optional_elements;

  public:
    WritableImageDataRequest(const WritableImageDataRequest &) = delete;
    WritableImageDataRequest(
        nonstd::span<IWritableImageDataRequestOptionalElement *const> optional_elements);
    WritableImageDataRequest(
        std::initializer_list<IWritableImageDataRequestOptionalElement *const> optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetResponseMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableImageDataRequest : ReadableCapwapRequest {
  protected:
    std::unordered_map<ElementHeader::ElementType, IReadableImageDataRequestOptionalElement *const>
        key_optional_elements;

    std::unordered_map<ElementHeader::ElementType, IReadableImageDataRequestOptionalElement *const>
    MapOptionalsElements(
        nonstd::span<IReadableImageDataRequestOptionalElement *const> optional_elements);

  public:
    size_t unknown_elements;
    ReadableImageDataRequest(const ReadableImageDataRequest &) = delete;
    ReadableImageDataRequest(
        nonstd::span<IReadableImageDataRequestOptionalElement *const> optional_elements);

    ReadableImageDataRequest(
        std::initializer_list<IReadableImageDataRequestOptionalElement *> optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;
};
