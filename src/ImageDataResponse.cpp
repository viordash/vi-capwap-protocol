

#include "ImageDataResponse.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"
#include "logging.h"

WritableImageDataResponse::WritableImageDataResponse(
    const ResultCode::Type result_code,
    WritableVendorSpecificPayloadArray &vendor_specific_payloads,
    const ImageInformation *image_information)
    : result_code{ result_code }, vendor_specific_payloads{ vendor_specific_payloads },
      image_information{ image_information } {
}

ControlHeader::MessageType WritableImageDataResponse::GetMessageType() const {
    return ControlHeader::ImageDataResponse;
}

ControlHeader::MessageType WritableImageDataResponse::GetRequestMessageType() const {
    return ControlHeader::ImageDataRequest;
}

void WritableImageDataResponse::Serialize(RawData *raw_data) const {
    result_code.Serialize(raw_data);
    vendor_specific_payloads.Serialize(raw_data);
    if (image_information != nullptr) {
        image_information->Serialize(raw_data);
    }
}

ReadableImageDataResponse::ReadableImageDataResponse() : unknown_elements{} {
    result_code = nullptr;
    image_information = nullptr;
}

ControlHeader::MessageType ReadableImageDataResponse::GetMessageType() const {
    return ControlHeader::ImageDataResponse;
}

bool ReadableImageDataResponse::Deserialize(RawData *raw_data) {
    while (raw_data->current + sizeof(ElementHeader) <= raw_data->end) {
        ElementHeader *element = (ElementHeader *)raw_data->current;

        switch (element->GetElementType()) {
            case ElementHeader::ElementType::ResultCode:
                result_code = ResultCode::Deserialize(raw_data);
                if (result_code == nullptr) {
                    return false;
                }
                break;

            case ElementHeader::ElementType::VendorSpecificPayload:
                if (!vendor_specific_payloads.Deserialize(raw_data)) {
                    return false;
                }
                break;

            case ElementHeader::ElementType::ImageInformation:
                image_information = ImageInformation::Deserialize(raw_data);
                if (image_information == nullptr) {
                    return false;
                }
                break;

            default: {
                auto unknownElement = UnrecognizedElement::Deserialize(raw_data);
                if (unknownElement == nullptr) {
                    return false;
                }
                unknown_elements++;
                log_w("ReadableImageDataResponse::Deserialize unhandled element type: 0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }
    return result_code != nullptr;
}

void ReadableImageDataResponse::Log() const {
    log_i("----------------------------------");
    log_i("ME ImageDataResponse:");

    ASSERT(result_code != nullptr);
    result_code->Log();

    vendor_specific_payloads.Log();
    if (image_information != nullptr) {
        image_information->Log();
    }

    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %lu", unknown_elements);
    }
    log_i("----------------------------------");
}