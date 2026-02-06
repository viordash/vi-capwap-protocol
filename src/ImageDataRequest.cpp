

#include "ImageDataRequest.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"
#include "logging.h"

WritableImageDataRequest::WritableImageDataRequest(
    const CapwapTransportProtocol *capwap_transport_protocol,
    const WritableImageData *image_data,
    WritableVendorSpecificPayloadArray &vendor_specific_payloads,
    const WritableImageIdentifier *image_identifier,
    const InitiateDownload *initiate_download)
    : capwap_transport_protocol{ capwap_transport_protocol }, image_data{ image_data },
      vendor_specific_payloads{ vendor_specific_payloads }, image_identifier{ image_identifier },
      initiate_download{ initiate_download } {
}

ControlHeader::MessageType WritableImageDataRequest::GetMessageType() const {
    return ControlHeader::ImageDataRequest;
}

ControlHeader::MessageType WritableImageDataRequest::GetResponseMessageType() const {
    return ControlHeader::ImageDataResponse;
}

void WritableImageDataRequest::Serialize(RawData *raw_data) const {
    if (capwap_transport_protocol != nullptr) {
        capwap_transport_protocol->Serialize(raw_data);
    }
    if (image_data != nullptr) {
        image_data->Serialize(raw_data);
    }
    vendor_specific_payloads.Serialize(raw_data);
    if (image_identifier != nullptr) {
        image_identifier->Serialize(raw_data);
    }
    if (initiate_download != nullptr) {
        initiate_download->Serialize(raw_data);
    }
}

ReadableImageDataRequest::ReadableImageDataRequest() : unknown_elements{} {
    capwap_transport_protocol = nullptr;
    image_data = nullptr;
    initiate_download = nullptr;
}

ControlHeader::MessageType ReadableImageDataRequest::GetMessageType() const {
    return ControlHeader::ImageDataRequest;
}

bool ReadableImageDataRequest::Deserialize(RawData *raw_data) {

    while (raw_data->current + sizeof(ElementHeader) <= raw_data->end) {
        ElementHeader *element = (ElementHeader *)raw_data->current;

        switch (element->GetElementType()) {

            case ElementHeader::ElementType::CAPWAPTransportProtocol:
                capwap_transport_protocol = CapwapTransportProtocol::Deserialize(raw_data);
                if (capwap_transport_protocol == nullptr) {
                    return false;
                }
                break;

            case ElementHeader::ElementType::ImageData:
                image_data = ReadableImageData::Deserialize(raw_data);
                if (image_data == nullptr) {
                    return false;
                }
                break;

            case ElementHeader::ElementType::VendorSpecificPayload:
                if (!vendor_specific_payloads.Deserialize(raw_data)) {
                    return false;
                }
                break;

            case ElementHeader::ElementType::ImageIdentifier:
                if (!image_identifier.Deserialize(raw_data)) {
                    return false;
                }
                break;

            case ElementHeader::ElementType::InitiateDownload:
                initiate_download = InitiateDownload::Deserialize(raw_data);
                if (initiate_download == nullptr) {
                    return false;
                }
                break;

            default: {
                auto unknownElement = UnrecognizedElement::Deserialize(raw_data);
                if (unknownElement == nullptr) {
                    return false;
                }
                unknown_elements++;
                log_w("ReadableImageDataRequest::Deserialize unhandled element type: 0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }
    return image_data != nullptr
        || (image_identifier.GetData().size() > 0 && initiate_download != nullptr);
}

void ReadableImageDataRequest::Log() const {
    log_i("----------------------------------");
    log_i("ME ImageDataRequest:");

    if (capwap_transport_protocol != nullptr) {
        capwap_transport_protocol->Log();
    }
    if (image_data != nullptr) {
        image_data->Log();
    }
    image_identifier.Log();
    if (initiate_download != nullptr) {
        initiate_download->Log();
    }

    vendor_specific_payloads.Log();
    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %lu", unknown_elements);
    }
    log_i("----------------------------------");
}