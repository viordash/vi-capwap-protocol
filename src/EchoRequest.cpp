

#include "EchoRequest.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"
#include "logging.h"

WritableEchoRequest::WritableEchoRequest(
    WritableVendorSpecificPayloadArray &vendor_specific_payloads)
    : vendor_specific_payloads{ vendor_specific_payloads } {
}

ControlHeader::MessageType WritableEchoRequest::GetMessageType() const {
    return ControlHeader::EchoRequest;
}

ControlHeader::MessageType WritableEchoRequest::GetResponseMessageType() const {
    return ControlHeader::EchoResponse;
}

void WritableEchoRequest::Serialize(RawData *raw_data) const {
    vendor_specific_payloads.Serialize(raw_data);
}

ReadableEchoRequest::ReadableEchoRequest() : unknown_elements{} {
}

ControlHeader::MessageType ReadableEchoRequest::GetMessageType() const {
    return ControlHeader::EchoRequest;
}

bool ReadableEchoRequest::Deserialize(RawData *raw_data) {
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
                log_w("ReadableEchoRequest::Deserialize unhandled element type: "
                      "0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }
    return true;
}

void ReadableEchoRequest::Log() const {
    log_i("----------------------------------");
    log_i("ME EchoRequest:");

    vendor_specific_payloads.Log();
    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %lu", unknown_elements);
    }
    log_i("----------------------------------");
}