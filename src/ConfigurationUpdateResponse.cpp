

#include "ConfigurationUpdateResponse.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"
#include "logging.h"

WritableConfigurationUpdateResponse::WritableConfigurationUpdateResponse(
    const ResultCode::Type result_code,
    WritableRadioOperationalStateArray &radio_operational_states,
    WritableVendorSpecificPayloadArray &vendor_specific_payloads)
    : result_code{ result_code }, radio_operational_states{ radio_operational_states },
      vendor_specific_payloads{ vendor_specific_payloads } {
}

ControlHeader::MessageType WritableConfigurationUpdateResponse::GetMessageType() const {
    return ControlHeader::ConfigurationUpdateResponse;
}

ControlHeader::MessageType WritableConfigurationUpdateResponse::GetRequestMessageType() const {
    return ControlHeader::ConfigurationUpdateRequest;
}

void WritableConfigurationUpdateResponse::Serialize(RawData *raw_data) const {
    result_code.Serialize(raw_data);
    radio_operational_states.Serialize(raw_data);
    vendor_specific_payloads.Serialize(raw_data);
}

ReadableConfigurationUpdateResponse::ReadableConfigurationUpdateResponse() : unknown_elements{} {
    result_code = nullptr;
}

ControlHeader::MessageType ReadableConfigurationUpdateResponse::GetMessageType() const {
    return ControlHeader::ConfigurationUpdateResponse;
}

bool ReadableConfigurationUpdateResponse::Deserialize(RawData *raw_data) {
    while (raw_data->current + sizeof(ElementHeader) <= raw_data->end) {
        ElementHeader *element = (ElementHeader *)raw_data->current;

        switch (element->GetElementType()) {
            case ElementHeader::ElementType::ResultCode:
                result_code = ResultCode::Deserialize(raw_data);
                if (result_code == nullptr) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::RadioOperationalState:
                if (!radio_operational_states.Deserialize(raw_data)) {
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
                log_w("ReadableConfigurationUpdateResponse::Deserialize unhandled element type: "
                      "0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }
    return result_code != nullptr;
}

void ReadableConfigurationUpdateResponse::Log() const {
    log_i("----------------------------------");
    log_i("ME ConfigurationUpdateResponse:");

    ASSERT(result_code != nullptr);
    result_code->Log();

    radio_operational_states.Log();
    vendor_specific_payloads.Log();
    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %lu", unknown_elements);
    }
    log_i("----------------------------------");
}