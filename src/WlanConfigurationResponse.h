#pragma once

#include "CapwapMessage.h"
#include "IElement.h"
#include "elements/ResultCode.h"
#include "elements/VendorSpecificPayload.h"
#include "span.hpp"
#include <limits>
#include <unordered_map>

struct WritableWlanConfigurationResponse : WritableCapwapResponse {
  protected:
    const WritableResultCode result_code;
    nonstd::span<IWritableWlanConfigurationResponseOptionalElement *const> optional_elements;

  public:
    WritableWlanConfigurationResponse(const WritableWlanConfigurationResponse &) = delete;
    WritableWlanConfigurationResponse(const ResultCode::Type result_code);
    WritableWlanConfigurationResponse(
        const ResultCode::Type result_code,
        nonstd::span<IWritableWlanConfigurationResponseOptionalElement *const> optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetRequestMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableWlanConfigurationResponse : ReadableCapwapResponse {
  protected:
    std::unordered_map<ElementHeader::ElementType,
                       IReadableWlanConfigurationResponseOptionalElement *const>
        key_optional_elements;

    std::unordered_map<ElementHeader::ElementType,
                       IReadableWlanConfigurationResponseOptionalElement *const>
    MapOptionalsElements(
        nonstd::span<IReadableWlanConfigurationResponseOptionalElement *const> optional_elements);

  public:
    ReadableResultCode result_code;
    size_t unknown_elements;

    ReadableWlanConfigurationResponse(const ReadableWlanConfigurationResponse &) = delete;
    ReadableWlanConfigurationResponse();
    ReadableWlanConfigurationResponse(
        nonstd::span<IReadableWlanConfigurationResponseOptionalElement *const> optional_elements);

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
