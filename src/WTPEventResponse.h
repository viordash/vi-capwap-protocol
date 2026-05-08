#pragma once

#include "CapwapMessage.h"
#include "IElement.h"
#include "elements/RadioOperationalState.h"
#include "elements/ResultCode.h"
#include "elements/VendorSpecificPayload.h"
#include "span.hpp"
#include <limits>
#include <unordered_map>

struct WritableWTPEventResponse : WritableCapwapResponse {
  private:
    nonstd::span<IWritableWTPEventResponseOptionalElement *const> optional_elements;

  public:
    WritableWTPEventResponse(const WritableWTPEventResponse &) = delete;
    WritableWTPEventResponse(
        nonstd::span<IWritableWTPEventResponseOptionalElement *const> optional_elements);


    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetRequestMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableWTPEventResponse : ReadableCapwapResponse {
  protected:
    std::unordered_map<ElementHeader::ElementType, IReadableWTPEventResponseOptionalElement *const>
        key_optional_elements;

    std::unordered_map<ElementHeader::ElementType, IReadableWTPEventResponseOptionalElement *const>
    MapOptionalsElements(
        nonstd::span<IReadableWTPEventResponseOptionalElement *const> optional_elements);

  public:
    size_t unknown_elements;

    ReadableWTPEventResponse(const ReadableWTPEventResponse &) = delete;
    ReadableWTPEventResponse(
        nonstd::span<IReadableWTPEventResponseOptionalElement *const> optional_elements);


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
