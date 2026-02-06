#pragma once

#include "CapwapMessage.h"
#include "elements/ImageIdentifier.h"
#include "elements/ImageInformation.h"
#include "elements/InitiateDownload.h"
#include "elements/ResultCode.h"
#include "elements/VendorSpecificPayload.h"
#include "span.hpp"
#include <limits>
#include <vector>

struct WritableImageDataResponse : WritableCapwapResponse {

  private:
    const ResultCode result_code;
    WritableVendorSpecificPayloadArray &vendor_specific_payloads;
    const ImageInformation *image_information;

  public:
    WritableImageDataResponse(const WritableImageDataResponse &) = delete;
    WritableImageDataResponse(const ResultCode::Type result_code,
                              WritableVendorSpecificPayloadArray &vendor_specific_payloads,
                              const ImageInformation *image_information);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetRequestMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableImageDataResponse : ReadableCapwapResponse {
    ResultCode *result_code;
    ReadableVendorSpecificPayloadArray vendor_specific_payloads;
    ImageInformation *image_information;

    size_t unknown_elements;

    ReadableImageDataResponse(const ReadableImageDataResponse &) = delete;
    ReadableImageDataResponse();

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;
};
