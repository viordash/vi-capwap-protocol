

#include "JoinResponse.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"
#include "logging.h"

WritableJoinResponse::WritableJoinResponse(
    const ResultCode::Type result_code,
    const WritableACDescriptor &ac_descriptor,
    const std::string_view ac_name,
    const nonstd::span<const WTPRadioInformation> &wtp_radio_info,
    const ECNSupport::Type ecn_support,
    const nonstd::span<const CAPWAPControlIPv4Address> &control_ip_addresses,
    const nonstd::span<const CAPWAPLocalIPv4Address> &local_ip_addresses,

    const WritableACIPv4List *ac_ipv4_list,
    const CapwapTransportProtocol *capwap_transport_protocol,
    const WritableImageIdentifier *image_identifier,
    const MaximumMessageLength *maximum_message_length,
    WritableVendorSpecificPayloadArray &vendor_specific_payloads)
    : result_code{ result_code }, ac_descriptor{ ac_descriptor }, ac_name{ ac_name },
      wtp_radio_informations{ wtp_radio_info }, ecn_support{ ecn_support },
      control_ip_addresses{ control_ip_addresses }, local_ip_addresses{ local_ip_addresses },
      ac_ipv4_list{ ac_ipv4_list }, capwap_transport_protocol{ capwap_transport_protocol },
      image_identifier{ image_identifier }, maximum_message_length{ maximum_message_length },
      vendor_specific_payloads{ vendor_specific_payloads } {
}

ControlHeader::MessageType WritableJoinResponse::GetMessageType() const {
    return ControlHeader::JoinResponse;
}

ControlHeader::MessageType WritableJoinResponse::GetRequestMessageType() const {
    return ControlHeader::JoinRequest;
}

void WritableJoinResponse::Serialize(RawData *raw_data) const {
    result_code.Serialize(raw_data);
    ac_descriptor.Serialize(raw_data);
    ac_name.Serialize(raw_data);
    wtp_radio_informations.Serialize(raw_data);
    ecn_support.Serialize(raw_data);
    control_ip_addresses.Serialize(raw_data);
    local_ip_addresses.Serialize(raw_data);
    if (ac_ipv4_list != nullptr) {
        ac_ipv4_list->Serialize(raw_data);
    }
    if (capwap_transport_protocol != nullptr) {
        capwap_transport_protocol->Serialize(raw_data);
    }
    if (image_identifier != nullptr) {
        image_identifier->Serialize(raw_data);
    }
    if (maximum_message_length != nullptr) {
        maximum_message_length->Serialize(raw_data);
    }
    vendor_specific_payloads.Serialize(raw_data);
}

ReadableJoinResponse::ReadableJoinResponse() : unknown_elements{} {
    result_code = nullptr;
    ac_name = nullptr;
    ecn_support = nullptr;
    ac_ipv4_list = nullptr;
    capwap_transport_protocol = nullptr;
    maximum_message_length = nullptr;
}

ControlHeader::MessageType ReadableJoinResponse::GetMessageType() const {
    return ControlHeader::JoinResponse;
}

bool ReadableJoinResponse::Deserialize(RawData *raw_data) {
    while (raw_data->current + sizeof(ElementHeader) <= raw_data->end) {
        ElementHeader *element = (ElementHeader *)raw_data->current;

        switch (element->GetElementType()) {
            case ElementHeader::ElementType::ResultCode:
                result_code = ResultCode::Deserialize(raw_data);
                if (result_code == nullptr) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::ACDescriptor:
                if (!ac_descriptor.Deserialize(raw_data)) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::ACName:
                ac_name = ReadableACName::Deserialize(raw_data);
                if (ac_name == nullptr) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::WTPRadioInformation:
                if (!wtp_radio_informations.Deserialize(raw_data)) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::ECNSupport:
                ecn_support = ECNSupport::Deserialize(raw_data);
                if (ecn_support == nullptr) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::CAPWAPControlIPv4Address:
                if (!control_ip_addresses.Deserialize(raw_data)) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::CAPWAPLocalIPv4Address:
                if (!local_ip_addresses.Deserialize(raw_data)) {
                    return false;
                }
                break;

            case ElementHeader::ElementType::ACIPv4List:
                ac_ipv4_list = ReadableACIPv4List::Deserialize(raw_data);
                if (ac_ipv4_list == nullptr) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::CAPWAPTransportProtocol:
                capwap_transport_protocol = CapwapTransportProtocol::Deserialize(raw_data);
                if (capwap_transport_protocol == nullptr) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::ImageIdentifier:
                if (!image_identifier.Deserialize(raw_data)) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::MaximumMessageLength:
                maximum_message_length = MaximumMessageLength::Deserialize(raw_data);
                if (maximum_message_length == nullptr) {
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
                log_e("ReadableJoinResponse::Deserialize unhandled element type: 0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }
    return result_code != nullptr && ac_descriptor.Get().size() > 0 && ac_name != nullptr
        && wtp_radio_informations.Get().size() && ecn_support != nullptr
        && control_ip_addresses.Get().size() > 0 && local_ip_addresses.Get().size() > 0;
}

void ReadableJoinResponse::Log() const {
    log_i("----------------------------------");
    log_i("ME JoinResponse:");

    ASSERT(result_code != nullptr);
    result_code->Log();

    ac_descriptor.Log();

    ASSERT(ac_name != nullptr);
    ac_name->Log();

    wtp_radio_informations.Log();

    ASSERT(ecn_support != nullptr);
    ecn_support->Log();

    control_ip_addresses.Log();
    local_ip_addresses.Log();

    if (ac_ipv4_list != nullptr) {
        ac_ipv4_list->Log();
    }
    if (capwap_transport_protocol != nullptr) {
        capwap_transport_protocol->Log();
    }
    image_identifier.Log();

    if (maximum_message_length != nullptr) {
        maximum_message_length->Log();
    }
    vendor_specific_payloads.Log();

    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %lu", unknown_elements);
    }
    log_i("----------------------------------");
}