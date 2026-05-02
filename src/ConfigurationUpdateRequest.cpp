

#include "ConfigurationUpdateRequest.h"
#include "Logging.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"

WritableConfigurationUpdateRequest::WritableConfigurationUpdateRequest(
    nonstd::span<IWritableConfigurationUpdateRequestOptionalElement *const> optional_elements)
    : optional_elements{ optional_elements } {
}

WritableConfigurationUpdateRequest::WritableConfigurationUpdateRequest(
    std::initializer_list<IWritableConfigurationUpdateRequestOptionalElement *const>
        optional_elements)
    : WritableConfigurationUpdateRequest(
          nonstd::span<IWritableConfigurationUpdateRequestOptionalElement *const>(
              optional_elements.begin(),
              optional_elements.size())) {
}

ControlHeader::MessageType WritableConfigurationUpdateRequest::GetMessageType() const {
    return ControlHeader::ConfigurationUpdateRequest;
}

ControlHeader::MessageType WritableConfigurationUpdateRequest::GetResponseMessageType() const {
    return ControlHeader::ConfigurationUpdateResponse;
}

void WritableConfigurationUpdateRequest::Serialize(RawData *raw_data) const {
    for (auto *elem : optional_elements) {
        elem->Serialize(raw_data);
    }
}

void WritableConfigurationUpdateRequest::Clear() {
    // for (auto *elem : optional_elements) {
    //     // elem->Serialize(raw_data);
    // }

    // ac_names_with_priority.Clear();
    // ac_timestamp.reset();
    // add_mac_acl_entry.Clear();
    // capwap_timers.reset();
    // decryption_error_report_periods.Clear();
    // delete_mac_acl_entry.Clear();
    // idle_timeout.reset();
    // location_data.reset();
    // radio_states.Clear();
    // statistics_timer.reset();
    // wtp_fallback.reset();
    // wtp_name.reset();
    // wtp_static_ipaddress.reset();
    // image_identifier.reset();
    // vendor_specific_payloads.Clear();
}

bool WritableConfigurationUpdateRequest::Validate() {
    // for (auto *elem : optional_elements) {
    //     // elem->Serialize(raw_data);
    // }

    // if (!ac_names_with_priority.Empty()) {
    //     return true;
    // }
    // if (ac_timestamp.has_value() && ac_timestamp.value().Validate()) {
    //     return true;
    // }
    // if (!add_mac_acl_entry.Empty()) {
    //     return true;
    // }
    // if (capwap_timers.has_value() && capwap_timers.value().Validate()) {
    //     return true;
    // }
    // if (!decryption_error_report_periods.Empty()) {
    //     return true;
    // }
    // if (!delete_mac_acl_entry.Empty()) {
    //     return true;
    // }
    // if (idle_timeout.has_value() && idle_timeout.value().Validate()) {
    //     return true;
    // }
    // if (location_data.has_value()) {
    //     return true;
    // }
    // if (!radio_states.Empty()) {
    //     return true;
    // }
    // if (statistics_timer.has_value() && statistics_timer.value().Validate()) {
    //     return true;
    // }
    // if (wtp_fallback.has_value() && wtp_fallback.value().Validate()) {
    //     return true;
    // }
    // if (wtp_name.has_value()) {
    //     return true;
    // }
    // if (wtp_static_ipaddress.has_value() && wtp_static_ipaddress.value().Validate()) {
    //     return true;
    // }
    // if (image_identifier.has_value()) {
    //     return true;
    // }
    // if (!vendor_specific_payloads.Empty()) {
    //     return true;
    // }
    return false;
}

ReadableConfigurationUpdateRequest::ReadableConfigurationUpdateRequest() : unknown_elements{} {
    ac_timestamp = nullptr;
    capwap_timers = nullptr;
    idle_timeout = nullptr;
    location_data = nullptr;
    statistics_timer = nullptr;
    wtp_name = nullptr;
}

ControlHeader::MessageType ReadableConfigurationUpdateRequest::GetMessageType() const {
    return ControlHeader::ConfigurationUpdateRequest;
}

bool ReadableConfigurationUpdateRequest::Deserialize(RawData *raw_data) {
    bool valid = false;
    while (raw_data->current + sizeof(ElementHeader) <= raw_data->end) {
        ElementHeader *element = (ElementHeader *)raw_data->current;

        switch (element->GetElementType()) {
            case ElementHeader::ElementType::ACNameWithPriority:
                if (!ac_names_with_priority.Deserialize(raw_data)) {
                    return false;
                }
                valid = true;
                break;

            // case ElementHeader::ElementType::ACTimestamp:
            //     ac_timestamp = ACTimestamp::Deserialize(raw_data);
            //     if (ac_timestamp == nullptr) {
            //         return false;
            //     }
            //     valid = true;
            //     break;
            case ElementHeader::ElementType::AddMACACLEntry:
                if (!add_mac_acl_entry.Deserialize(raw_data)) {
                    return false;
                }
                valid = true;
                break;
            // case ElementHeader::ElementType::CAPWAPTimers:
            //     capwap_timers = CAPWAPTimers::Deserialize(raw_data);
            //     if (capwap_timers == nullptr) {
            //         return false;
            //     }
            //     valid = true;
            //     break;
            case ElementHeader::ElementType::DecryptionErrorReportPeriod:
                if (!decryption_error_report_periods.Deserialize(raw_data)) {
                    return false;
                }
                valid = true;
                break;
            case ElementHeader::ElementType::DeleteMACACLEntry:
                if (!delete_mac_acl_entry.Deserialize(raw_data)) {
                    return false;
                }
                valid = true;
                break;
            // case ElementHeader::ElementType::IdleTimeout:
            //     idle_timeout = IdleTimeout::Deserialize(raw_data);
            //     if (idle_timeout == nullptr) {
            //         return false;
            //     }
            //     valid = true;
            //     break;
            // case ElementHeader::ElementType::LocationData:
            //     location_data = ReadableLocationData::Deserialize(raw_data);
            //     if (location_data == nullptr) {
            //         return false;
            //     }
            //     valid = true;
            //     break;
            case ElementHeader::ElementType::RadioAdministrativeState:
                if (!radio_states.Deserialize(raw_data)) {
                    return false;
                }
                valid = true;
                break;
            // case ElementHeader::ElementType::StatisticsTimer:
            //     statistics_timer = StatisticsTimer::Deserialize(raw_data);
            //     if (statistics_timer == nullptr) {
            //         return false;
            //     }
            //     valid = true;
            //     break;
            case ElementHeader::ElementType::WTPFallback:
                if (!wtp_fallback.Deserialize(raw_data)) {
                    return false;
                }
                valid = true;
                break;
            // case ElementHeader::ElementType::WTPName:
            //     wtp_name = ReadableWTPName::Deserialize(raw_data);
            //     if (wtp_name == nullptr) {
            //         return false;
            //     }
            //     valid = true;
            //     break;
            case ElementHeader::ElementType::WTPStaticIPAddressInformation:
                if (!wtp_static_ipaddress.Deserialize(raw_data)) {
                    return false;
                }
                valid = true;
                break;
            case ElementHeader::ElementType::ImageIdentifier:
                if (!image_identifier.Deserialize(raw_data)) {
                    return false;
                }
                valid = true;
                break;
            case ElementHeader::ElementType::VendorSpecificPayload:
                if (!vendor_specific_payloads.Deserialize(raw_data)) {
                    return false;
                }
                valid = true;
                break;

            default: {
                auto unknownElement = UnrecognizedElement::Deserialize(raw_data);
                if (unknownElement == nullptr) {
                    return false;
                }
                unknown_elements++;
                log_w("ReadableConfigurationUpdateRequest::Deserialize unhandled element type: "
                      "0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }
    return valid;
}

void ReadableConfigurationUpdateRequest::Log() const {
    log_i("----------------------------------");
    log_i("ME ConfigurationUpdateRequest:");

    if (ac_names_with_priority.Get().size() > 0) {
        ac_names_with_priority.Log();
    }
    if (ac_timestamp != nullptr) {
        ac_timestamp->Log();
    }
    if (add_mac_acl_entry.Get().size() > 0) {
        add_mac_acl_entry.Log();
    }
    if (capwap_timers != nullptr) {
        capwap_timers->Log();
    }
    if (decryption_error_report_periods.Get().size() > 0) {
        decryption_error_report_periods.Log();
    }
    if (delete_mac_acl_entry.Get().size() > 0) {
        delete_mac_acl_entry.Log();
    }
    if (idle_timeout != nullptr) {
        idle_timeout->Log();
    }
    if (location_data != nullptr) {
        location_data->Log();
    }
    if (radio_states.Get().size() > 0) {
        radio_states.Log();
    }
    if (statistics_timer != nullptr) {
        statistics_timer->Log();
    }
    if (wtp_fallback.IsPresent()) {
        wtp_fallback.Log();
    }
    if (wtp_name != nullptr) {
        wtp_name->Log();
    }
    if (wtp_static_ipaddress.IsPresent()) {
        wtp_static_ipaddress.Log();
    }
    image_identifier.Log();
    if (vendor_specific_payloads.Get().size() > 0) {
        vendor_specific_payloads.Log();
    }

    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %zu", unknown_elements);
    }
    log_i("----------------------------------");
}