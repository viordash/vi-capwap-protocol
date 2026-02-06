

#include "ResetRequest.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"
#include "logging.h"

WritableResetRequest::WritableResetRequest(
    WritableImageIdentifier &image_identifier,
    WritableVendorSpecificPayloadArray &vendor_specific_payloads)

    : image_identifier{ image_identifier }, vendor_specific_payloads{ vendor_specific_payloads } {
}

ControlHeader::MessageType WritableResetRequest::GetMessageType() const {
    return ControlHeader::ResetRequest;
}

ControlHeader::MessageType WritableResetRequest::GetResponseMessageType() const {
    return ControlHeader::ResetResponse;
}

void WritableResetRequest::Serialize(RawData *raw_data) const {
    image_identifier.Serialize(raw_data);
    vendor_specific_payloads.Serialize(raw_data);
}

ReadableResetRequest::ReadableResetRequest() : unknown_elements{} {
}

ControlHeader::MessageType ReadableResetRequest::GetMessageType() const {
    return ControlHeader::ResetRequest;
}

bool ReadableResetRequest::Deserialize(RawData *raw_data) {
    bool valid = false;
    while (raw_data->current + sizeof(ElementHeader) <= raw_data->end) {
        ElementHeader *element = (ElementHeader *)raw_data->current;

        switch (element->GetElementType()) {
            case ElementHeader::ElementType::ImageIdentifier:
                if (!image_identifier.Deserialize(raw_data)) {
                    return false;
                }
                valid = true;
                break;
            case ElementHeader::ElementType::VendorSpecificPayload:
                if (!vendor_specific_payloads.Deserialize(raw_data)) {
                    return false;
                }
                valid = true;
                break;

            default: {
                auto unknownElement = UnrecognizedElement::Deserialize(raw_data);
                if (unknownElement == nullptr) {
                    return false;
                }
                unknown_elements++;
                log_w("ReadableResetRequest::Deserialize unhandled element type: "
                      "0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }
    return valid;
}

void ReadableResetRequest::Log() const {
    log_i("----------------------------------");
    log_i("ME ResetRequest:");

    image_identifier.Log();
    if (vendor_specific_payloads.Get().size() > 0) {
        vendor_specific_payloads.Log();
    }

    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %lu", unknown_elements);
    }
    log_i("----------------------------------");
}