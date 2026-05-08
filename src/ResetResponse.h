#pragma once

#include "CapwapMessage.h"
#include "IElement.h"
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
