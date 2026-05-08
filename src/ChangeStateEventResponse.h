#pragma once

#include "CapwapMessage.h"
#include "IElement.h"
#include "elements/VendorSpecificPayload.h"
#include "span.hpp"
#include <limits>
#include <unordered_map>

struct WritableChangeStateEventResponse : WritableCapwapResponse {
  private:
    nonstd::span<IWritableChangeStateEventResponseOptionalElement *const> optional_elements;

  public:
    WritableChangeStateEventResponse(const WritableChangeStateEventResponse &) = delete;
    WritableChangeStateEventResponse();
    WritableChangeStateEventResponse(
        nonstd::span<IWritableChangeStateEventResponseOptionalElement *const> optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetRequestMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableChangeStateEventResponse : ReadableCapwapResponse {
  protected:
    std::unordered_map<ElementHeader::ElementType,
                       IReadableChangeStateEventResponseOptionalElement *const>
        key_optional_elements;

    std::unordered_map<ElementHeader::ElementType,
                       IReadableChangeStateEventResponseOptionalElement *const>
    MapOptionalsElements(
        nonstd::span<IReadableChangeStateEventResponseOptionalElement *const> optional_elements);

  public:
    size_t unknown_elements;

    ReadableChangeStateEventResponse(const ReadableChangeStateEventResponse &) = delete;
    ReadableChangeStateEventResponse();
    ReadableChangeStateEventResponse(
        nonstd::span<IReadableChangeStateEventResponseOptionalElement *const> optional_elements);

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
