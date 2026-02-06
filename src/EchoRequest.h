#pragma once

#include "CapwapMessage.h"
#include "elements/VendorSpecificPayload.h"
#include "span.hpp"
#include <limits>
#include <vector>

struct WritableEchoRequest : WritableCapwapRequest {

  private:
    WritableVendorSpecificPayloadArray &vendor_specific_payloads;

  public:
    WritableEchoRequest(const WritableEchoRequest &) = delete;
    WritableEchoRequest(WritableVendorSpecificPayloadArray &vendor_specific_payloads);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetResponseMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableEchoRequest : ReadableCapwapRequest {
    ReadableVendorSpecificPayloadArray vendor_specific_payloads;

    size_t unknown_elements;

    ReadableEchoRequest(const ReadableEchoRequest &) = delete;
    ReadableEchoRequest();

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;
};
