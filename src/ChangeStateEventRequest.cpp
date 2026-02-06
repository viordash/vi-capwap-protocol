

#include "ChangeStateEventRequest.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"
#include "logging.h"

WritableChangeStateEventRequest::WritableChangeStateEventRequest(
    WritableRadioOperationalStateArray &radio_operational_states,
    ResultCode &result_code,
    WritableReturnedMessageElementArray &returned_message_elements,
    WritableVendorSpecificPayloadArray &vendor_specific_payloads)
    : radio_operational_states{ radio_operational_states }, result_code{ result_code },
      returned_message_elements{ returned_message_elements },
      vendor_specific_payloads{ vendor_specific_payloads } {

    if (!this->returned_message_elements.Validate()) {
        this->returned_message_elements.Clear();
        log_e("ChangeStateEventRequest: invalid one of Returned Message Element. Switch ResultCode "
              "to Failure_UnrecognizedMessageElement");
        this->result_code = { ResultCode::Type::Failure_UnrecognizedMessageElement };
    }
}

ControlHeader::MessageType WritableChangeStateEventRequest::GetMessageType() const {
    return ControlHeader::ChangeStateEventRequest;
}

ControlHeader::MessageType WritableChangeStateEventRequest::GetResponseMessageType() const {
    return ControlHeader::ChangeStateEventResponse;
}

void WritableChangeStateEventRequest::Serialize(RawData *raw_data) const {
    radio_operational_states.Serialize(raw_data);
    result_code.Serialize(raw_data);
    returned_message_elements.Serialize(raw_data);
    vendor_specific_payloads.Serialize(raw_data);
}

ResultCode::Type WritableChangeStateEventRequest::GetResultCode() {
    return result_code.type;
}

void WritableChangeStateEventRequest::Clear() {
    radio_operational_states.Clear();
    returned_message_elements.Clear();
    vendor_specific_payloads.Clear();
}

bool WritableChangeStateEventRequest::Validate() {
    if (radio_operational_states.Empty()) {
        return false;
    }
    if (!result_code.Validate()) {
        return false;
    }
    return true;
}

ReadableChangeStateEventRequest::ReadableChangeStateEventRequest() : unknown_elements{} {
    result_code = nullptr;
}

ControlHeader::MessageType ReadableChangeStateEventRequest::GetMessageType() const {
    return ControlHeader::ChangeStateEventRequest;
}

bool ReadableChangeStateEventRequest::Deserialize(RawData *raw_data) {
    while (raw_data->current + sizeof(ElementHeader) <= raw_data->end) {
        ElementHeader *element = (ElementHeader *)raw_data->current;

        switch (element->GetElementType()) {
            case ElementHeader::ElementType::RadioOperationalState:
                if (!radio_operational_states.Deserialize(raw_data)) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::ResultCode:
                result_code = ResultCode::Deserialize(raw_data);
                if (result_code == nullptr) {
                    return false;
                }
                break;

            case ElementHeader::ElementType::ReturnedMessageElement:
                if (!returned_message_elements.Deserialize(raw_data)) {
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
                log_w("ReadableChangeStateEventRequest::Deserialize unhandled element type: 0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }
    return radio_operational_states.Get().size() > 0 && result_code != nullptr;
}

void ReadableChangeStateEventRequest::Log() const {
    log_i("----------------------------------");
    log_i("ME ChangeStateEventRequest:");

    radio_operational_states.Log();

    ASSERT(result_code != nullptr);
    result_code->Log();

    returned_message_elements.Log();

    vendor_specific_payloads.Log();

    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %lu", unknown_elements);
    }
    log_i("----------------------------------");
}