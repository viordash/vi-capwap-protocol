#pragma once

#include "CapwapMessage.h"
#include "elements/ResultCode.h"
#include "elements/VendorSpecificPayload.h"
#include "span.hpp"
#include <limits>
#include <vector>

struct WritableResetResponse : WritableCapwapResponse {

  private:
    const ResultCode result_code;

    WritableVendorSpecificPayloadArray &vendor_specific_payloads;

  public:
    WritableResetResponse(const WritableResetResponse &) = delete;
    WritableResetResponse(const ResultCode::Type result_code,
                          WritableVendorSpecificPayloadArray &vendor_specific_payloads);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetRequestMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableResetResponse : ReadableCapwapResponse {
    ResultCode *result_code;

    ReadableVendorSpecificPayloadArray vendor_specific_payloads;

    size_t unknown_elements;

    ReadableResetResponse(const ReadableResetResponse &) = delete;
    ReadableResetResponse();

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;
};
