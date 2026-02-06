

#include "JoinRequest.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"
#include "logging.h"

WritableJoinRequest::WritableJoinRequest(
    const std::string_view location_data,
    const WritableWTPBoardData &wtp_board_data,
    const WritableWTPDescriptor &wtp_descriptor,
    const std::string_view wtp_name,
    const SessionId &session_id,
    const WTPFrameTunnelMode &wtp_frame_tunnel_mode,
    const WTPMACType::Type mac_type,
    const nonstd::span<const WTPRadioInformation> &wtp_radio_info,
    const ECNSupport::Type ecn_support,
    const nonstd::span<const CAPWAPLocalIPv4Address> &ip_addresses,
    const CapwapTransportProtocol *capwap_transport_protocol,
    const MaximumMessageLength *maximum_message_length,
    const WTPRebootStatistics *wtp_reboot_statistics,
    WritableVendorSpecificPayloadArray &vendor_specific_payloads)
    : location_data{ location_data }, wtp_board_data{ wtp_board_data },
      wtp_descriptor{ wtp_descriptor }, wtp_name{ wtp_name }, session_id{ session_id },
      wtp_frame_tunnel_mode{ wtp_frame_tunnel_mode }, wtp_mac_type{ mac_type },
      wtp_radio_informations{ wtp_radio_info }, ecn_support{ ecn_support },
      ip_addresses{ ip_addresses }, capwap_transport_protocol{ capwap_transport_protocol },
      maximum_message_length{ maximum_message_length },
      wtp_reboot_statistics{ wtp_reboot_statistics },
      vendor_specific_payloads{ vendor_specific_payloads } {
}

ControlHeader::MessageType WritableJoinRequest::GetMessageType() const {
    return ControlHeader::JoinRequest;
}

ControlHeader::MessageType WritableJoinRequest::GetResponseMessageType() const {
    return ControlHeader::JoinResponse;
}

void WritableJoinRequest::Serialize(RawData *raw_data) const {
    location_data.Serialize(raw_data);
    wtp_board_data.Serialize(raw_data);
    wtp_descriptor.Serialize(raw_data);
    wtp_name.Serialize(raw_data);
    session_id.Serialize(raw_data);
    wtp_frame_tunnel_mode.Serialize(raw_data);
    wtp_mac_type.Serialize(raw_data);
    wtp_radio_informations.Serialize(raw_data);
    ecn_support.Serialize(raw_data);
    ip_addresses.Serialize(raw_data);
    if (capwap_transport_protocol != nullptr) {
        capwap_transport_protocol->Serialize(raw_data);
    }
    if (maximum_message_length != nullptr) {
        maximum_message_length->Serialize(raw_data);
    }
    if (wtp_reboot_statistics != nullptr) {
        wtp_reboot_statistics->Serialize(raw_data);
    }
    vendor_specific_payloads.Serialize(raw_data);
}

ReadableJoinRequest::ReadableJoinRequest() : unknown_elements{} {
    location_data = nullptr;
    wtp_name = nullptr;
    session_id = nullptr;
    wtp_frame_tunnel_mode = nullptr;
    wtp_mac_type = nullptr;

    ecn_support = nullptr;
    capwap_transport_protocol = nullptr;
    maximum_message_length = nullptr;
    wtp_reboot_statistics = nullptr;
}

ControlHeader::MessageType ReadableJoinRequest::GetMessageType() const {
    return ControlHeader::JoinRequest;
}

bool ReadableJoinRequest::Deserialize(RawData *raw_data) {
    while (raw_data->current + sizeof(ElementHeader) <= raw_data->end) {
        ElementHeader *element = (ElementHeader *)raw_data->current;

        switch (element->GetElementType()) {
            case ElementHeader::ElementType::LocationData:
                location_data = ReadableLocationData::Deserialize(raw_data);
                if (location_data == nullptr) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::WTPBoardData:
                if (!wtp_board_data.Deserialize(raw_data)) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::WTPDescriptor:
                if (!wtp_descriptor.Deserialize(raw_data)) {
                    return false;
                }
                break;

            case ElementHeader::ElementType::WTPName:
                wtp_name = ReadableWTPName::Deserialize(raw_data);
                if (wtp_name == nullptr) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::SessionID:
                session_id = SessionId::Deserialize(raw_data);
                if (session_id == nullptr) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::WTPFrameTunnelMode:
                wtp_frame_tunnel_mode = WTPFrameTunnelMode::Deserialize(raw_data);
                if (wtp_frame_tunnel_mode == nullptr) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::WTPMACType:
                wtp_mac_type = WTPMACType::Deserialize(raw_data);
                if (wtp_mac_type == nullptr) {
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
            case ElementHeader::ElementType::CAPWAPLocalIPv4Address:
                if (!ip_addresses.Deserialize(raw_data)) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::CAPWAPTransportProtocol:
                capwap_transport_protocol = CapwapTransportProtocol::Deserialize(raw_data);
                if (capwap_transport_protocol == nullptr) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::MaximumMessageLength:
                maximum_message_length = MaximumMessageLength::Deserialize(raw_data);
                if (maximum_message_length == nullptr) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::WTPRebootStatistics:
                wtp_reboot_statistics = WTPRebootStatistics::Deserialize(raw_data);
                if (wtp_reboot_statistics == nullptr) {
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
                log_e("ReadableJoinRequest::Deserialize unhandled element type: 0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }

    return location_data != nullptr && wtp_board_data.header != nullptr
        && wtp_descriptor.header != nullptr && wtp_name != nullptr && session_id != nullptr
        && wtp_frame_tunnel_mode != nullptr && wtp_mac_type != nullptr
        && wtp_radio_informations.Get().size() > 0 && ecn_support != nullptr
        && ip_addresses.Get().size() > 0;
}

void ReadableJoinRequest::Log() const {
    log_i("----------------------------------");
    log_i("ME JoinRequest:");

    ASSERT(location_data != nullptr);
    location_data->Log();

    wtp_board_data.Log();
    wtp_descriptor.Log();

    ASSERT(wtp_name != nullptr);
    wtp_name->Log();

    ASSERT(session_id != nullptr);
    session_id->Log();

    ASSERT(wtp_frame_tunnel_mode != nullptr);
    wtp_frame_tunnel_mode->Log();

    ASSERT(wtp_mac_type != nullptr);
    wtp_mac_type->Log();

    wtp_radio_informations.Log();

    ASSERT(ecn_support != nullptr);
    ecn_support->Log();

    ip_addresses.Log();

    if (capwap_transport_protocol != nullptr) {
        capwap_transport_protocol->Log();
    }
    if (maximum_message_length != nullptr) {
        maximum_message_length->Log();
    }
    if (wtp_reboot_statistics != nullptr) {
        wtp_reboot_statistics->Log();
    }
    vendor_specific_payloads.Log();

    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %lu", unknown_elements);
    }
    log_i("----------------------------------");
}