#pragma once

#include "CapwapMessage.h"
#include "elements/ImageIdentifier.h"
#include "elements/VendorSpecificPayload.h"
#include "span.hpp"
#include <limits>
#include <optional>
#include <vector>

struct WritableResetRequest : WritableCapwapRequest {

  private:
    WritableImageIdentifier &image_identifier;
    WritableVendorSpecificPayloadArray &vendor_specific_payloads;

  public:
    WritableResetRequest(const WritableResetRequest &) = delete;
    WritableResetRequest(WritableImageIdentifier &image_identifier,
                         WritableVendorSpecificPayloadArray &vendor_specific_payloads);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetResponseMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableResetRequest : ReadableCapwapRequest {
    ReadableImageIdentifier image_identifier;
    ReadableVendorSpecificPayloadArray vendor_specific_payloads;

    size_t unknown_elements;

    ReadableResetRequest(const ReadableResetRequest &) = delete;
    ReadableResetRequest();

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;
};
