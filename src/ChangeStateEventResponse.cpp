

#include "ChangeStateEventResponse.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"
#include "logging.h"

WritableChangeStateEventResponse::WritableChangeStateEventResponse(
    WritableVendorSpecificPayloadArray &vendor_specific_payloads)
    : vendor_specific_payloads{ vendor_specific_payloads } {
}

ControlHeader::MessageType WritableChangeStateEventResponse::GetMessageType() const {
    return ControlHeader::ChangeStateEventResponse;
}

ControlHeader::MessageType WritableChangeStateEventResponse::GetRequestMessageType() const {
    return ControlHeader::ChangeStateEventRequest;
}

void WritableChangeStateEventResponse::Serialize(RawData *raw_data) const {
    vendor_specific_payloads.Serialize(raw_data);
}

ReadableChangeStateEventResponse::ReadableChangeStateEventResponse() : unknown_elements{} {
}

ControlHeader::MessageType ReadableChangeStateEventResponse::GetMessageType() const {
    return ControlHeader::ChangeStateEventResponse;
}

bool ReadableChangeStateEventResponse::Deserialize(RawData *raw_data) {
    while (raw_data->current + sizeof(ElementHeader) <= raw_data->end) {
        ElementHeader *element = (ElementHeader *)raw_data->current;

        switch (element->GetElementType()) {
            case ElementHeader::ElementType::VendorSpecificPayload:
                if (!vendor_specific_payloads.Deserialize(raw_data)) {
                    return false;
                }
                break;

            default: {
                auto unknownElement = UnrecognizedElement::Deserialize(raw_data);
                if (unknownElement == nullptr) {
                    return false;
                }
                unknown_elements++;
                log_w("ReadableChangeStateEventResponse::Deserialize unhandled element type: "
                      "0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }
    return true;
}

void ReadableChangeStateEventResponse::Log() const {
    log_i("----------------------------------");
    log_i("ME ChangeStateEventResponse:");

    vendor_specific_payloads.Log();
    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %lu", unknown_elements);
    }
    log_i("----------------------------------");
}