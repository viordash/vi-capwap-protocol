

#include "ChangeStateEventRequest.h"
#include "Logging.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"
#include <algorithm>

WritableChangeStateEventRequest::WritableChangeStateEventRequest(
    WritableRadioOperationalStateArray &radio_operational_states,
    ResultCode &result_code)
    : WritableChangeStateEventRequest(
          radio_operational_states,
          result_code,
          nonstd::span<IWritableChangeStateEventRequestOptionalElement *const>{}) {
}

WritableChangeStateEventRequest::WritableChangeStateEventRequest(
    WritableRadioOperationalStateArray &radio_operational_states,
    ResultCode &result_code,
    nonstd::span<IWritableChangeStateEventRequestOptionalElement *const> optional_elements)
    : radio_operational_states{ radio_operational_states }, result_code{ result_code },
      optional_elements{ optional_elements } {

    ValidateReturnedMessageElement();
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
    for (auto *elem : optional_elements) {
        ASSERT(elem != nullptr);
        elem->Serialize(raw_data);
    }
}

ResultCode::Type WritableChangeStateEventRequest::GetResultCode() {
    return result_code.type;
}

void WritableChangeStateEventRequest::ValidateReturnedMessageElement() {
    for (auto *elem : optional_elements) {
        ASSERT(elem != nullptr);
        if (elem->GetElementType() == ElementHeader::ReturnedMessageElement) {
            auto *returned_message_elements =
                static_cast<WritableReturnedMessageElementArray *>(elem);
            if (!returned_message_elements->Validate()) {
                returned_message_elements->Clear();
                log_e("ChangeStateEventRequest: invalid one of Returned Message Element. Switch "
                      "ResultCode "
                      "to Failure_UnrecognizedMessageElement");
                this->result_code = { ResultCode::Type::Failure_UnrecognizedMessageElement };
            }
            break;
        }
    }
}

ReadableChangeStateEventRequest::ReadableChangeStateEventRequest()
    : ReadableChangeStateEventRequest(
          nonstd::span<IReadableChangeStateEventRequestOptionalElement *const>{}) {
}

ReadableChangeStateEventRequest::ReadableChangeStateEventRequest(
    nonstd::span<IReadableChangeStateEventRequestOptionalElement *const> optional_elements)
    : key_optional_elements{ MapOptionalsElements(optional_elements) }, unknown_elements{} {
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

            default: {
                auto it = key_optional_elements.find(element->GetElementType());
                if (it != key_optional_elements.end()) {
                    if (!it->second->Deserialize(raw_data)) {
                        return false;
                    }
                    break;
                }

                auto unknownElement = UnrecognizedElement::Deserialize(raw_data);
                if (unknownElement == nullptr) {
                    return false;
                }
                unknown_elements++;
                log_w(
                    "ReadableChangeStateEventRequest::Deserialize unhandled element type: 0x{:04X}",
                    (unsigned)unknownElement->GetElementType());

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

    for (const auto &[type, value] : key_optional_elements) {
        if (value->IsPresent()) {
            value->Log();
        } else {
            log_i("  expected optional element is missing, type: 0x{:04X}", (unsigned)type);
        }
    }
    if (unknown_elements > 0) {
        log_i("  UnknownElements count: {}", unknown_elements);
    }
    log_i("----------------------------------");
}

std::unordered_map<ElementHeader::ElementType,
                   IReadableChangeStateEventRequestOptionalElement *const>
ReadableChangeStateEventRequest::MapOptionalsElements(
    nonstd::span<IReadableChangeStateEventRequestOptionalElement *const> optional_elements) {

    std::unordered_map<ElementHeader::ElementType,
                       IReadableChangeStateEventRequestOptionalElement *const>
        map;

    for (auto *elem : optional_elements) {
        ASSERT(elem != nullptr);
        map.emplace(elem->GetElementType(), elem);
    }

    return map;
}