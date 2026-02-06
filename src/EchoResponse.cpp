

#include "EchoResponse.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"
#include "logging.h"

WritableEchoResponse::WritableEchoResponse(
    WritableVendorSpecificPayloadArray &vendor_specific_payloads)
    : vendor_specific_payloads{ vendor_specific_payloads } {
}

ControlHeader::MessageType WritableEchoResponse::GetMessageType() const {
    return ControlHeader::EchoResponse;
}

ControlHeader::MessageType WritableEchoResponse::GetRequestMessageType() const {
    return ControlHeader::EchoRequest;
}

void WritableEchoResponse::Serialize(RawData *raw_data) const {
    vendor_specific_payloads.Serialize(raw_data);
}

ReadableEchoResponse::ReadableEchoResponse() : unknown_elements{} {
}

ControlHeader::MessageType ReadableEchoResponse::GetMessageType() const {
    return ControlHeader::EchoResponse;
}

bool ReadableEchoResponse::Deserialize(RawData *raw_data) {
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
                log_w("ReadableEchoResponse::Deserialize unhandled element type: "
                      "0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }
    return true;
}

void ReadableEchoResponse::Log() const {
    log_i("----------------------------------");
    log_i("ME EchoResponse:");

    vendor_specific_payloads.Log();
    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %lu", unknown_elements);
    }
    log_i("----------------------------------");
}