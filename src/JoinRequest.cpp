

#include "JoinRequest.h"
#include "Logging.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"

WritableJoinRequest::WritableJoinRequest(
    const std::string_view location_data,
    const WritableWTPBoardData &wtp_board_data,
    const WritableWTPDescriptor &wtp_descriptor,
    const std::string_view wtp_name,
    const SessionId &session_id,
    const WTPFrameTunnelMode &wtp_frame_tunnel_mode,
    const WTPMACType::Type mac_type,
    WritableWTPRadioInformationArray &wtp_radio_informations,
    const ECNSupport::Type ecn_support,
    const nonstd::span<const CAPWAPLocalIPv4Address> &ip_addresses,
    nonstd::span<IWritableJoinRequestOptionalElement *const> optional_elements)
    : location_data{ location_data }, wtp_board_data{ wtp_board_data },
      wtp_descriptor{ wtp_descriptor }, wtp_name{ wtp_name }, session_id{ session_id },
      wtp_frame_tunnel_mode{ wtp_frame_tunnel_mode }, wtp_mac_type{ mac_type },
      wtp_radio_informations{ wtp_radio_informations }, ecn_support{ ecn_support },
      ip_addresses{ ip_addresses }, optional_elements{ optional_elements } {
    ASSERT(wtp_descriptor.GetHeader().RadiosInUse <= wtp_radio_informations.Size());
}

WritableJoinRequest::WritableJoinRequest(
    const std::string_view location_data,
    const WritableWTPBoardData &wtp_board_data,
    const WritableWTPDescriptor &wtp_descriptor,
    const std::string_view wtp_name,
    const SessionId &session_id,
    const WTPFrameTunnelMode &wtp_frame_tunnel_mode,
    const WTPMACType::Type mac_type,
    WritableWTPRadioInformationArray &wtp_radio_informations,
    const ECNSupport::Type ecn_support,
    const nonstd::span<const CAPWAPLocalIPv4Address> &ip_addresses,
    std::initializer_list<IWritableJoinRequestOptionalElement *> optional_elements)
    : WritableJoinRequest(
          location_data,
          wtp_board_data,
          wtp_descriptor,
          wtp_name,
          session_id,
          wtp_frame_tunnel_mode,
          mac_type,
          wtp_radio_informations,
          ecn_support,
          ip_addresses,
          nonstd::span<IWritableJoinRequestOptionalElement *const>(optional_elements.begin(),
                                                                   optional_elements.size())) {
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

    for (auto *elem : optional_elements) {
        elem->Serialize(raw_data);
    }
}

ReadableJoinRequest::ReadableJoinRequest(
    nonstd::span<IReadableJoinRequestOptionalElement *const> optional_elements)
    : key_optional_elements{ MapOptionalsElements(optional_elements) }, unknown_elements{} {
    wtp_name = nullptr;
    session_id = nullptr;
    wtp_frame_tunnel_mode = nullptr;
    wtp_mac_type = nullptr;
}

ReadableJoinRequest::ReadableJoinRequest(
    std::initializer_list<IReadableJoinRequestOptionalElement *> optional_elements)
    : ReadableJoinRequest(
          nonstd::span<IReadableJoinRequestOptionalElement *const>(optional_elements.begin(),
                                                                   optional_elements.size())) {
}

ControlHeader::MessageType ReadableJoinRequest::GetMessageType() const {
    return ControlHeader::JoinRequest;
}

bool ReadableJoinRequest::Deserialize(RawData *raw_data) {
    while (raw_data->current + sizeof(ElementHeader) <= raw_data->end) {
        ElementHeader *element = (ElementHeader *)raw_data->current;

        switch (element->GetElementType()) {
            case ElementHeader::ElementType::LocationData:
                if (!location_data.Deserialize(raw_data)) {
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
                log_e("ReadableJoinRequest::Deserialize unhandled element type: 0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }

    return location_data.IsPresent() && wtp_board_data.header != nullptr
        && wtp_descriptor.header != nullptr && wtp_name != nullptr && session_id != nullptr
        && wtp_frame_tunnel_mode != nullptr && wtp_mac_type != nullptr
        && wtp_radio_informations.Get().size() > 0 && ecn_support != nullptr
        && ip_addresses.Get().size() > 0;
}

void ReadableJoinRequest::Log() const {
    log_i("----------------------------------");
    log_i("ME JoinRequest:");

    location_data.Log();

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

    for (const auto &[_, value] : key_optional_elements) {
        value->Log();
    }

    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %zu", unknown_elements);
    }
    log_i("----------------------------------");
}

std::unordered_map<ElementHeader::ElementType, IReadableJoinRequestOptionalElement *const>
ReadableJoinRequest::MapOptionalsElements(
    nonstd::span<IReadableJoinRequestOptionalElement *const> optional_elements) {

    std::unordered_map<ElementHeader::ElementType, IReadableJoinRequestOptionalElement *const> map;

    for (auto *elem : optional_elements) {
        map.emplace(elem->GetElementType(), elem);
    }

    return map;
}