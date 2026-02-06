#pragma once

#include "CapwapMessage.h"
#include "elements/VendorSpecificPayload.h"
#include "span.hpp"
#include <limits>
#include <vector>

struct WritableChangeStateEventResponse : WritableCapwapResponse {
  private:
    WritableVendorSpecificPayloadArray &vendor_specific_payloads;

  public:
    WritableChangeStateEventResponse(const WritableChangeStateEventResponse &) = delete;
    WritableChangeStateEventResponse(WritableVendorSpecificPayloadArray &vendor_specific_payloads);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetRequestMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableChangeStateEventResponse : ReadableCapwapResponse {
    ReadableVendorSpecificPayloadArray vendor_specific_payloads;

    size_t unknown_elements;

    ReadableChangeStateEventResponse(const ReadableChangeStateEventResponse &) = delete;
    ReadableChangeStateEventResponse();

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;
};
