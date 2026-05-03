#pragma once

#include "CapwapMessage.h"
#include "IElement.h"
#include "elements/ImageIdentifier.h"
#include "elements/VendorSpecificPayload.h"
#include "span.hpp"
#include <limits>
#include <optional>
#include <unordered_map>

struct WritableResetRequest : WritableCapwapRequest {

  private:
    WritableImageIdentifier &image_identifier;

    nonstd::span<IWritableResetRequestOptionalElement *const> optional_elements;

  public:
    WritableResetRequest(const WritableResetRequest &) = delete;
    WritableResetRequest(
        WritableImageIdentifier &image_identifier,
        nonstd::span<IWritableResetRequestOptionalElement *const> optional_elements);
    WritableResetRequest(
        WritableImageIdentifier &image_identifier,
        std::initializer_list<IWritableResetRequestOptionalElement *const> optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetResponseMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableResetRequest : ReadableCapwapRequest {
  protected:
    std::unordered_map<ElementHeader::ElementType, IReadableResetRequestOptionalElement *const>
        key_optional_elements;

    std::unordered_map<ElementHeader::ElementType, IReadableResetRequestOptionalElement *const>
    MapOptionalsElements(
        nonstd::span<IReadableResetRequestOptionalElement *const> optional_elements);

  public:
    ReadableImageIdentifier image_identifier;

    size_t unknown_elements;

    ReadableResetRequest(const ReadableResetRequest &) = delete;
    ReadableResetRequest(
        nonstd::span<IReadableResetRequestOptionalElement *const> optional_elements);
    ReadableResetRequest(
        std::initializer_list<IReadableResetRequestOptionalElement *> optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;
};
