

#include "ConfigurationStatusRequest.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"
#include "logging.h"

WritableConfigurationStatusRequest::WritableConfigurationStatusRequest(
    const std::string_view ac_name,
    WritableRadioAdministrativeStateArray &radio_states,
    const uint16_t statistics_timer,
    const WTPRebootStatistics &wtp_reboot_statistics,
    WritableACNameWithPriorityArray &ac_names_with_priority,
    const CapwapTransportProtocol *capwap_transport_protocol,
    const WTPStaticIPAddressInformation *wtp_static_ipaddress,
    WritableVendorSpecificPayloadArray &vendor_specific_payloads)

    : ac_name{ ac_name }, radio_states{ radio_states }, statistics_timer{ statistics_timer },
      wtp_reboot_statistics{ wtp_reboot_statistics },
      ac_names_with_priority{ ac_names_with_priority },
      capwap_transport_protocol{ capwap_transport_protocol },
      wtp_static_ipaddress{ wtp_static_ipaddress },
      vendor_specific_payloads{ vendor_specific_payloads } {
}

ControlHeader::MessageType WritableConfigurationStatusRequest::GetMessageType() const {
    return ControlHeader::ConfigurationStatusRequest;
}

ControlHeader::MessageType WritableConfigurationStatusRequest::GetResponseMessageType() const {
    return ControlHeader::ConfigurationStatusResponse;
}

void WritableConfigurationStatusRequest::Serialize(RawData *raw_data) const {
    ac_name.Serialize(raw_data);
    radio_states.Serialize(raw_data);
    statistics_timer.Serialize(raw_data);
    wtp_reboot_statistics.Serialize(raw_data);

    ac_names_with_priority.Serialize(raw_data);
    if (capwap_transport_protocol != nullptr) {
        capwap_transport_protocol->Serialize(raw_data);
    }
    if (wtp_static_ipaddress != nullptr) {
        wtp_static_ipaddress->Serialize(raw_data);
    }
    vendor_specific_payloads.Serialize(raw_data);
}

ReadableConfigurationStatusRequest::ReadableConfigurationStatusRequest() : unknown_elements{} {
    ac_name = nullptr;
    statistics_timer = nullptr;
    wtp_reboot_statistics = nullptr;
    capwap_transport_protocol = nullptr;
    wtp_static_ipaddress = nullptr;
}

ControlHeader::MessageType ReadableConfigurationStatusRequest::GetMessageType() const {
    return ControlHeader::ConfigurationStatusRequest;
}

bool ReadableConfigurationStatusRequest::Deserialize(RawData *raw_data) {
    while (raw_data->current + sizeof(ElementHeader) <= raw_data->end) {
        ElementHeader *element = (ElementHeader *)raw_data->current;

        switch (element->GetElementType()) {
            case ElementHeader::ElementType::ACName:
                ac_name = ReadableACName::Deserialize(raw_data);
                if (ac_name == nullptr) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::RadioAdministrativeState:
                if (!radio_states.Deserialize(raw_data)) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::StatisticsTimer:
                statistics_timer = StatisticsTimer::Deserialize(raw_data);
                if (statistics_timer == nullptr) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::WTPRebootStatistics:
                wtp_reboot_statistics = WTPRebootStatistics::Deserialize(raw_data);
                if (wtp_reboot_statistics == nullptr) {
                    return false;
                }
                break;

            case ElementHeader::ElementType::ACNameWithPriority:
                if (!ac_names_with_priority.Deserialize(raw_data)) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::CAPWAPTransportProtocol:
                capwap_transport_protocol = CapwapTransportProtocol::Deserialize(raw_data);
                if (capwap_transport_protocol == nullptr) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::WTPStaticIPAddressInformation:
                wtp_static_ipaddress = WTPStaticIPAddressInformation::Deserialize(raw_data);
                if (wtp_static_ipaddress == nullptr) {
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
                log_w("ReadableConfigurationStatusRequest::Deserialize unhandled element type: "
                      "0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }
    return ac_name != nullptr && radio_states.Get().size() > 0 && statistics_timer != nullptr
        && wtp_reboot_statistics != nullptr;
}

void ReadableConfigurationStatusRequest::Log() const {
    log_i("----------------------------------");
    log_i("ME ConfigurationStatusRequest:");

    ASSERT(ac_name != nullptr);
    ac_name->Log();

    radio_states.Log();

    ASSERT(statistics_timer != nullptr);
    statistics_timer->Log();

    ASSERT(wtp_reboot_statistics != nullptr);
    wtp_reboot_statistics->Log();
    ac_names_with_priority.Log();
    if (capwap_transport_protocol != nullptr) {
        capwap_transport_protocol->Log();
    }
    if (wtp_static_ipaddress != nullptr) {
        wtp_static_ipaddress->Log();
    }
    vendor_specific_payloads.Log();
    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %lu", unknown_elements);
    }
    log_i("----------------------------------");
}