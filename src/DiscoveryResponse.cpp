

#include "DiscoveryResponse.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"
#include "logging.h"

WritableDiscoveryResponse::WritableDiscoveryResponse(
    const WritableACDescriptor &ac_descriptor,
    const std::string_view ac_name,
    const nonstd::span<const WTPRadioInformation> &wtp_radio_info,
    const nonstd::span<const CAPWAPControlIPv4Address> &ip_addresses,
    WritableVendorSpecificPayloadArray &vendor_specific_payloads)
    : ac_descriptor{ ac_descriptor }, ac_name{ ac_name }, wtp_radio_informations{ wtp_radio_info },
      ip_addresses{ ip_addresses }, vendor_specific_payloads{ vendor_specific_payloads } {
}

ControlHeader::MessageType WritableDiscoveryResponse::GetMessageType() const {
    return ControlHeader::DiscoveryResponse;
}

ControlHeader::MessageType WritableDiscoveryResponse::GetRequestMessageType() const {
    return ControlHeader::DiscoveryRequest;
}

uint16_t WritableDiscoveryResponse::CalcTotalSize() {
    uint16_t total = ac_descriptor.GetTotalLength();
    total += ac_name.GetTotalLength();
    total += wtp_radio_informations.GetTotalLength();
    total += ip_addresses.GetTotalLength();
    total += vendor_specific_payloads.GetTotalLength();
    return total;
}

void WritableDiscoveryResponse::Serialize(RawData *raw_data) const {
    ac_descriptor.Serialize(raw_data);
    ac_name.Serialize(raw_data);
    wtp_radio_informations.Serialize(raw_data);
    ip_addresses.Serialize(raw_data);
    vendor_specific_payloads.Serialize(raw_data);
}

ReadableDiscoveryResponse::ReadableDiscoveryResponse() : unknown_elements{} {
    ac_name = nullptr;
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
            case ElementHeader::ElementType::CAPWAPControlIPv4Address:
                if (!ip_addresses.Deserialize(raw_data)) {
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
                log_e("ReadableDiscoveryResponse::Deserialize unhandled element type: 0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }
    return ac_descriptor.header != nullptr && ac_name != nullptr
        && wtp_radio_informations.Get().size() > 0 && ip_addresses.Get().size() > 0;
}

void ReadableDiscoveryResponse::Log() const {
    log_i("----------------------------------");
    log_i("ME DiscoveryResponse:");

    ac_descriptor.Log();

    ASSERT(ac_name != nullptr);
    ac_name->Log();

    wtp_radio_informations.Log();
    ip_addresses.Log();

    vendor_specific_payloads.Log();
    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %lu", unknown_elements);
    }
    log_i("----------------------------------");
}