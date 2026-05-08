

#include "JoinResponse.h"
#include "Logging.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"

WritableJoinResponse::WritableJoinResponse(
    const ResultCode::Type result_code,
    const WritableACDescriptor &ac_descriptor,
    const std::string_view ac_name,
    WritableWTPRadioInformationArray &wtp_radio_informations,
    const ECNSupport::Type ecn_support,
    const nonstd::span<const CAPWAPControlIPv4Address> &control_ip_addresses,
    const nonstd::span<const CAPWAPLocalIPv4Address> &local_ip_addresses)
    : WritableJoinResponse(result_code,
                           ac_descriptor,
                           ac_name,
                           wtp_radio_informations,
                           ecn_support,
                           control_ip_addresses,
                           local_ip_addresses,
                           nonstd::span<IWritableJoinResponseOptionalElement *const>{}) {
}

WritableJoinResponse::WritableJoinResponse(
    const ResultCode::Type result_code,
    const WritableACDescriptor &ac_descriptor,
    const std::string_view ac_name,
    WritableWTPRadioInformationArray &wtp_radio_informations,
    const ECNSupport::Type ecn_support,
    const nonstd::span<const CAPWAPControlIPv4Address> &control_ip_addresses,
    const nonstd::span<const CAPWAPLocalIPv4Address> &local_ip_addresses,
    nonstd::span<IWritableJoinResponseOptionalElement *const> optional_elements)
    : result_code{ result_code }, ac_descriptor{ ac_descriptor }, ac_name{ ac_name },
      wtp_radio_informations{ wtp_radio_informations }, ecn_support{ ecn_support },
      control_ip_addresses{ control_ip_addresses }, local_ip_addresses{ local_ip_addresses },
      optional_elements{ optional_elements } {
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

    for (auto *elem : optional_elements) {
        ASSERT(elem != nullptr);
        elem->Serialize(raw_data);
    }
}

ReadableJoinResponse::ReadableJoinResponse()
    : ReadableJoinResponse(nonstd::span<IReadableJoinResponseOptionalElement *const>{}) {
}

ReadableJoinResponse::ReadableJoinResponse(
    nonstd::span<IReadableJoinResponseOptionalElement *const> optional_elements)
    : key_optional_elements{ MapOptionalsElements(optional_elements) }, unknown_elements{} {
}

ControlHeader::MessageType ReadableJoinResponse::GetMessageType() const {
    return ControlHeader::JoinResponse;
}

bool ReadableJoinResponse::Deserialize(RawData *raw_data) {
    while (raw_data->current + sizeof(ElementHeader) <= raw_data->end) {
        ElementHeader *element = (ElementHeader *)raw_data->current;

        switch (element->GetElementType()) {
            case ElementHeader::ElementType::ResultCode:
                if (!result_code.Deserialize(raw_data)) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::ACDescriptor:
                if (!ac_descriptor.Deserialize(raw_data)) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::ACName:
                if (!ac_name.Deserialize(raw_data)) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::WTPRadioInformation:
                if (!wtp_radio_informations.Deserialize(raw_data)) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::ECNSupport:
                if (!ecn_support.Deserialize(raw_data)) {
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
                log_e("ReadableJoinResponse::Deserialize unhandled element type: 0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }
    return result_code.IsPresent() && ac_descriptor.IsPresent() > 0 && ac_name.IsPresent()
        && wtp_radio_informations.IsPresent() && ecn_support.IsPresent()
        && control_ip_addresses.IsPresent() && local_ip_addresses.IsPresent();
}

void ReadableJoinResponse::Log() const {
    log_i("----------------------------------");
    log_i("ME JoinResponse:");

    result_code.Log();

    ac_descriptor.Log();

    ac_name.Log();

    wtp_radio_informations.Log();

    ecn_support.Log();

    control_ip_addresses.Log();
    local_ip_addresses.Log();

    for (const auto &[_, value] : key_optional_elements) {
        (void)_;
        value->Log();
    }

    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %zu", unknown_elements);
    }
    log_i("----------------------------------");
}

std::unordered_map<ElementHeader::ElementType, IReadableJoinResponseOptionalElement *const>
ReadableJoinResponse::MapOptionalsElements(
    nonstd::span<IReadableJoinResponseOptionalElement *const> optional_elements) {

    std::unordered_map<ElementHeader::ElementType, IReadableJoinResponseOptionalElement *const> map;

    for (auto *elem : optional_elements) {
        ASSERT(elem != nullptr);
        map.emplace(elem->GetElementType(), elem);
    }

    return map;
}