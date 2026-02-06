#pragma once

#include "CapwapMessage.h"
#include "elements/RadioOperationalState.h"
#include "elements/ResultCode.h"
#include "elements/ReturnedMessageElement.h"
#include "elements/VendorSpecificPayload.h"
#include "span.hpp"
#include <limits>
#include <vector>

struct WritableChangeStateEventRequest : WritableCapwapRequest {

  private:
    WritableRadioOperationalStateArray &radio_operational_states;
    ResultCode &result_code;

    WritableReturnedMessageElementArray &returned_message_elements;
    WritableVendorSpecificPayloadArray &vendor_specific_payloads;

  public:
    static const uint16_t no_probe_mtu_size = std::numeric_limits<uint16_t>::min();

    WritableChangeStateEventRequest(const WritableChangeStateEventRequest &) = delete;
    WritableChangeStateEventRequest(WritableRadioOperationalStateArray &radio_operational_states,
                                    ResultCode &result_code,
                                    WritableReturnedMessageElementArray &returned_message_elements,
                                    WritableVendorSpecificPayloadArray &vendor_specific_payloads);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetResponseMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
    ResultCode::Type GetResultCode();
    void Clear();
    bool Validate();
};

struct ReadableChangeStateEventRequest : ReadableCapwapRequest {
    ReadableRadioOperationalStateArray radio_operational_states;
    ResultCode *result_code;

    ReadableReturnedMessageElementArray returned_message_elements;
    ReadableVendorSpecificPayloadArray vendor_specific_payloads;

    size_t unknown_elements;

    ReadableChangeStateEventRequest(const ReadableChangeStateEventRequest &) = delete;
    ReadableChangeStateEventRequest();

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;
};
