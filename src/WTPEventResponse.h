#pragma once

#include "CapwapMessage.h"
#include "elements/RadioOperationalState.h"
#include "elements/ResultCode.h"
#include "elements/VendorSpecificPayload.h"
#include "span.hpp"
#include <limits>
#include <vector>

struct WritableWTPEventResponse : WritableCapwapResponse {
  private:
    WritableVendorSpecificPayloadArray &vendor_specific_payloads;

  public:
    WritableWTPEventResponse(const WritableWTPEventResponse &) = delete;
    WritableWTPEventResponse(WritableVendorSpecificPayloadArray &vendor_specific_payloads);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetRequestMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableWTPEventResponse : ReadableCapwapResponse {
    ReadableVendorSpecificPayloadArray vendor_specific_payloads;

    size_t unknown_elements;

    ReadableWTPEventResponse(const ReadableWTPEventResponse &) = delete;
    ReadableWTPEventResponse();

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;
};
