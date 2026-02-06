#pragma once

#include "CapwapMessage.h"
#include "elements/RadioOperationalState.h"
#include "elements/ResultCode.h"
#include "elements/VendorSpecificPayload.h"
#include "span.hpp"
#include <limits>
#include <vector>

struct WritableConfigurationUpdateResponse : WritableCapwapResponse {

  private:
    const ResultCode result_code;

    WritableRadioOperationalStateArray &radio_operational_states;
    WritableVendorSpecificPayloadArray &vendor_specific_payloads;

  public:
    WritableConfigurationUpdateResponse(const WritableConfigurationUpdateResponse &) = delete;
    WritableConfigurationUpdateResponse(
        const ResultCode::Type result_code,
        WritableRadioOperationalStateArray &radio_operational_states,
        WritableVendorSpecificPayloadArray &vendor_specific_payloads);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetRequestMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableConfigurationUpdateResponse : ReadableCapwapResponse {
    ResultCode *result_code;

    ReadableRadioOperationalStateArray radio_operational_states;
    ReadableVendorSpecificPayloadArray vendor_specific_payloads;

    size_t unknown_elements;

    ReadableConfigurationUpdateResponse(const ReadableConfigurationUpdateResponse &) = delete;
    ReadableConfigurationUpdateResponse();

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;
};
