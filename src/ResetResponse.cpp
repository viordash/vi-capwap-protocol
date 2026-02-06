

#include "ResetResponse.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"
#include "logging.h"

WritableResetResponse::WritableResetResponse(
    const ResultCode::Type result_code,
    WritableVendorSpecificPayloadArray &vendor_specific_payloads)
    : result_code{ result_code }, vendor_specific_payloads{ vendor_specific_payloads } {
}

ControlHeader::MessageType WritableResetResponse::GetMessageType() const {
    return ControlHeader::ResetResponse;
}

ControlHeader::MessageType WritableResetResponse::GetRequestMessageType() const {
    return ControlHeader::ResetRequest;
}

void WritableResetResponse::Serialize(RawData *raw_data) const {
    result_code.Serialize(raw_data);
    vendor_specific_payloads.Serialize(raw_data);
}

ReadableResetResponse::ReadableResetResponse() : unknown_elements{} {
    result_code = nullptr;
}

ControlHeader::MessageType ReadableResetResponse::GetMessageType() const {
    return ControlHeader::ResetResponse;
}

bool ReadableResetResponse::Deserialize(RawData *raw_data) {
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

            default: {
                auto unknownElement = UnrecognizedElement::Deserialize(raw_data);
                if (unknownElement == nullptr) {
                    return false;
                }
                unknown_elements++;
                log_w("ReadableResetResponse::Deserialize unhandled element type: "
                      "0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }
    return result_code != nullptr;
}

void ReadableResetResponse::Log() const {
    log_i("----------------------------------");
    log_i("ME ResetResponse:");

    ASSERT(result_code != nullptr);
    result_code->Log();

    vendor_specific_payloads.Log();
    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %lu", unknown_elements);
    }
    log_i("----------------------------------");
}