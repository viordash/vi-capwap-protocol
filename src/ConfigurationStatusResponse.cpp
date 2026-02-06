

#include "ConfigurationStatusResponse.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"
#include "logging.h"

WritableConfigurationStatusResponse::WritableConfigurationStatusResponse(
    const CAPWAPTimers &capwap_timers,
    WritableDecryptionErrorReportPeriodArray &decryption_error_report_periods,
    const uint32_t idle_timeout,
    const WTPFallback::Mode wtp_fallback,
    const nonstd::span<const uint32_t> &ac_ipv4_list,
    const WTPStaticIPAddressInformation *wtp_static_ipaddress,
    WritableVendorSpecificPayloadArray &vendor_specific_payloads)

    : capwap_timers{ capwap_timers },
      decryption_error_report_periods{ decryption_error_report_periods },
      idle_timeout{ idle_timeout }, wtp_fallback{ wtp_fallback }, ac_ipv4_list{ ac_ipv4_list },
      wtp_static_ipaddress{ wtp_static_ipaddress },
      vendor_specific_payloads{ vendor_specific_payloads } {
}

ControlHeader::MessageType WritableConfigurationStatusResponse::GetMessageType() const {
    return ControlHeader::ConfigurationStatusResponse;
}

ControlHeader::MessageType WritableConfigurationStatusResponse::GetRequestMessageType() const {
    return ControlHeader::ConfigurationStatusRequest;
}

void WritableConfigurationStatusResponse::Serialize(RawData *raw_data) const {
    capwap_timers.Serialize(raw_data);
    decryption_error_report_periods.Serialize(raw_data);
    idle_timeout.Serialize(raw_data);
    wtp_fallback.Serialize(raw_data);
    ac_ipv4_list.Serialize(raw_data);

    if (wtp_static_ipaddress != nullptr) {
        wtp_static_ipaddress->Serialize(raw_data);
    }
    vendor_specific_payloads.Serialize(raw_data);
}

ReadableConfigurationStatusResponse::ReadableConfigurationStatusResponse() : unknown_elements{} {
    capwap_timers = nullptr;
    idle_timeout = nullptr;
    wtp_fallback = nullptr;
    ac_ipv4_list = nullptr;
    wtp_static_ipaddress = nullptr;
}

ControlHeader::MessageType ReadableConfigurationStatusResponse::GetMessageType() const {
    return ControlHeader::ConfigurationStatusResponse;
}

bool ReadableConfigurationStatusResponse::Deserialize(RawData *raw_data) {
    while (raw_data->current + sizeof(ElementHeader) <= raw_data->end) {
        ElementHeader *element = (ElementHeader *)raw_data->current;

        switch (element->GetElementType()) {
            case ElementHeader::ElementType::CAPWAPTimers:
                capwap_timers = CAPWAPTimers::Deserialize(raw_data);
                if (capwap_timers == nullptr) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::DecryptionErrorReportPeriod:
                if (!decryption_error_report_periods.Deserialize(raw_data)) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::IdleTimeout:
                idle_timeout = IdleTimeout::Deserialize(raw_data);
                if (idle_timeout == nullptr) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::WTPFallback:
                wtp_fallback = WTPFallback::Deserialize(raw_data);
                if (wtp_fallback == nullptr) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::ACIPv4List:
                ac_ipv4_list = ReadableACIPv4List::Deserialize(raw_data);
                if (ac_ipv4_list == nullptr) {
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
                log_w("ReadableConfigurationStatusResponse::Deserialize unhandled element type: "
                      "0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }
    return capwap_timers != nullptr && decryption_error_report_periods.Get().size() > 0
        && idle_timeout != nullptr && wtp_fallback != nullptr && ac_ipv4_list != nullptr;
}

void ReadableConfigurationStatusResponse::Log() const {
    log_i("----------------------------------");
    log_i("ME ConfigurationStatusResponse:");
    ASSERT(capwap_timers != nullptr);
    capwap_timers->Log();

    decryption_error_report_periods.Log();

    ASSERT(idle_timeout != nullptr);
    idle_timeout->Log();

    ASSERT(wtp_fallback != nullptr);
    wtp_fallback->Log();

    ASSERT(ac_ipv4_list != nullptr);
    ac_ipv4_list->Log();

    if (wtp_static_ipaddress != nullptr) {
        wtp_static_ipaddress->Log();
    }
    vendor_specific_payloads.Log();
    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %lu", unknown_elements);
    }
    log_i("----------------------------------");
}