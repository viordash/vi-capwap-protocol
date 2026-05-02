

#include "ChangeStateEventRequest.h"
#include "Logging.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"

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
    WritableResultCode{ result_code.type }.Serialize(raw_data);
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
                if (!result_code.Deserialize(raw_data)) {
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
    return radio_operational_states.Get().size() > 0 && result_code.IsPresent();
}

void ReadableChangeStateEventRequest::Log() const {
    log_i("----------------------------------");
    log_i("ME ChangeStateEventRequest:");

    radio_operational_states.Log();

    result_code.Log();

    returned_message_elements.Log();

    vendor_specific_payloads.Log();

    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %zu", unknown_elements);
    }
    log_i("----------------------------------");
}