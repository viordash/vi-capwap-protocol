#pragma once

#include "CapwapMessage.h"
#include "elements/CapwapTransportProtocol.h"
#include "elements/ImageData.h"
#include "elements/ImageIdentifier.h"
#include "elements/InitiateDownload.h"
#include "elements/VendorSpecificPayload.h"
#include "span.hpp"
#include <limits>
#include <vector>

struct WritableImageDataRequest : WritableCapwapRequest {

  private:
    const CapwapTransportProtocol *capwap_transport_protocol;
    const WritableImageData *image_data;
    WritableVendorSpecificPayloadArray &vendor_specific_payloads;
    const WritableImageIdentifier *image_identifier;
    const InitiateDownload *initiate_download;

  public:
    WritableImageDataRequest(const WritableImageDataRequest &) = delete;
    WritableImageDataRequest(const CapwapTransportProtocol *capwap_transport_protocol,
                             const WritableImageData *image_data,
                             WritableVendorSpecificPayloadArray &vendor_specific_payloads,
                             const WritableImageIdentifier *image_identifier,
                             const InitiateDownload *initiate_download);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetResponseMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableImageDataRequest : ReadableCapwapRequest {
    CapwapTransportProtocol *capwap_transport_protocol;
    ReadableImageData *image_data;
    ReadableVendorSpecificPayloadArray vendor_specific_payloads;
    ReadableImageIdentifier image_identifier;
    InitiateDownload *initiate_download;

    size_t unknown_elements;

    ReadableImageDataRequest(const ReadableImageDataRequest &) = delete;
    ReadableImageDataRequest();

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;
};
