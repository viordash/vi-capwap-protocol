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
    WritableResetRequest(WritableImageIdentifier &image_identifier);
    WritableResetRequest(
        WritableImageIdentifier &image_identifier,
        nonstd::span<IWritableResetRequestOptionalElement *const> optional_elements);

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
    ReadableResetRequest();
    ReadableResetRequest(
        nonstd::span<IReadableResetRequestOptionalElement *const> optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;

    template <typename T> T *GetOptionalElement(ElementHeader::ElementType element_type) {
        auto it = key_optional_elements.find(element_type);
        if (it != key_optional_elements.end()) {
            return static_cast<T *>(it->second);
        }
        return nullptr;
    }
};
