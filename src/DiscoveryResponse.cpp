

#include "DiscoveryResponse.h"
#include "Logging.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"

WritableDiscoveryResponse::WritableDiscoveryResponse(
    const WritableACDescriptor &ac_descriptor,
    const std::string_view ac_name,
    WritableWTPRadioInformationArray &wtp_radio_informations,
    const nonstd::span<const CAPWAPControlIPv4Address> &ip_addresses,
    nonstd::span<IWritableDiscoveryResponseOptionalElement *const> optional_elements)
    : ac_descriptor{ ac_descriptor }, ac_name{ ac_name },
      wtp_radio_informations{ wtp_radio_informations }, ip_addresses{ ip_addresses },
      optional_elements{ optional_elements } {
}

WritableDiscoveryResponse::WritableDiscoveryResponse(
    const WritableACDescriptor &ac_descriptor,
    const std::string_view ac_name,
    WritableWTPRadioInformationArray &wtp_radio_informations,
    const nonstd::span<const CAPWAPControlIPv4Address> &ip_addresses,
    std::initializer_list<IWritableDiscoveryResponseOptionalElement *const> optional_elements)
    : WritableDiscoveryResponse(ac_descriptor,
                                ac_name,
                                wtp_radio_informations,
                                ip_addresses,
                                nonstd::span<IWritableDiscoveryResponseOptionalElement *const>(
                                    optional_elements.begin(),
                                    optional_elements.size())) {
}

ControlHeader::MessageType WritableDiscoveryResponse::GetMessageType() const {
    return ControlHeader::DiscoveryResponse;
}

ControlHeader::MessageType WritableDiscoveryResponse::GetRequestMessageType() const {
    return ControlHeader::DiscoveryRequest;
}

void WritableDiscoveryResponse::Serialize(RawData *raw_data) const {
    ac_descriptor.Serialize(raw_data);
    ac_name.Serialize(raw_data);
    wtp_radio_informations.Serialize(raw_data);
    ip_addresses.Serialize(raw_data);

    for (auto *elem : optional_elements) {
        elem->Serialize(raw_data);
    }
}

ReadableDiscoveryResponse::ReadableDiscoveryResponse(
    nonstd::span<IReadableDiscoveryResponseOptionalElement *const> optional_elements)
    : key_optional_elements{ MapOptionalsElements(optional_elements) }, unknown_elements{} {
}

ReadableDiscoveryResponse::ReadableDiscoveryResponse(
    std::initializer_list<IReadableDiscoveryResponseOptionalElement *> optional_elements)
    : ReadableDiscoveryResponse(nonstd::span<IReadableDiscoveryResponseOptionalElement *const>(
          optional_elements.begin(),
          optional_elements.size())) {
}

ControlHeader::MessageType ReadableDiscoveryResponse::GetMessageType() const {
    return ControlHeader::DiscoveryResponse;
}

bool ReadableDiscoveryResponse::Deserialize(RawData *raw_data) {
    while (raw_data->current + sizeof(ElementHeader) <= raw_data->end) {
        ElementHeader *element = (ElementHeader *)raw_data->current;

        switch (element->GetElementType()) {
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
            case ElementHeader::ElementType::CAPWAPControlIPv4Address:
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
                log_e("ReadableDiscoveryResponse::Deserialize unhandled element type: 0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }
    return ac_descriptor.IsPresent() && ac_name.IsPresent() && wtp_radio_informations.IsPresent()
        && ip_addresses.IsPresent();
}

void ReadableDiscoveryResponse::Log() const {
    log_i("----------------------------------");
    log_i("ME DiscoveryResponse:");

    ac_descriptor.Log();

    ASSERT(ac_name.IsPresent());
    ac_name.Log();

    wtp_radio_informations.Log();
    ip_addresses.Log();

    for (const auto &[_, value] : key_optional_elements) {
        value->Log();
    }

    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %zu", unknown_elements);
    }
    log_i("----------------------------------");
}

std::unordered_map<ElementHeader::ElementType, IReadableDiscoveryResponseOptionalElement *const>
ReadableDiscoveryResponse::MapOptionalsElements(
    nonstd::span<IReadableDiscoveryResponseOptionalElement *const> optional_elements) {

    std::unordered_map<ElementHeader::ElementType, IReadableDiscoveryResponseOptionalElement *const>
        map;

    for (auto *elem : optional_elements) {
        map.emplace(elem->GetElementType(), elem);
    }

    return map;
}