#pragma once

#include "CapwapMessage.h"
#include "elements/RadioOperationalState.h"
#include "elements/ResultCode.h"
#include "elements/VendorSpecificPayload.h"
#include "span.hpp"
#include <limits>
#include <unordered_map>

struct WritableConfigurationUpdateResponse : WritableCapwapResponse {

  private:
    const WritableResultCode result_code;

    nonstd::span<IWritableConfigurationUpdateResponseOptionalElement *const> optional_elements;

  public:
    WritableConfigurationUpdateResponse(const WritableConfigurationUpdateResponse &) = delete;
    WritableConfigurationUpdateResponse(
        const ResultCode::Type result_code,
        nonstd::span<IWritableConfigurationUpdateResponseOptionalElement *const> optional_elements);
    WritableConfigurationUpdateResponse(
        const ResultCode::Type result_code,
        std::initializer_list<IWritableConfigurationUpdateResponseOptionalElement *const> optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetRequestMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableConfigurationUpdateResponse : ReadableCapwapResponse {
    protected:
    std::unordered_map<ElementHeader::ElementType, IReadableConfigurationUpdateResponseOptionalElement *const>
        key_optional_elements;

    std::unordered_map<ElementHeader::ElementType, IReadableConfigurationUpdateResponseOptionalElement *const>
    MapOptionalsElements(
        nonstd::span<IReadableConfigurationUpdateResponseOptionalElement *const> optional_elements);

  public:
    ReadableResultCode result_code;

    size_t unknown_elements;

    ReadableConfigurationUpdateResponse(const ReadableConfigurationUpdateResponse &) = delete;
    ReadableConfigurationUpdateResponse(
        nonstd::span<IReadableConfigurationUpdateResponseOptionalElement *const> optional_elements);
    ReadableConfigurationUpdateResponse(
        std::initializer_list<IReadableConfigurationUpdateResponseOptionalElement *> optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;
};
