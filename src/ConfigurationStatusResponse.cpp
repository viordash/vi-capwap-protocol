

#include "ConfigurationStatusResponse.h"
#include "Logging.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"

WritableConfigurationStatusResponse::WritableConfigurationStatusResponse(
    const WritableCAPWAPTimers &capwap_timers,
    WritableDecryptionErrorReportPeriodArray &decryption_error_report_periods,
    const uint32_t idle_timeout,
    const WTPFallback::Mode wtp_fallback,
    const nonstd::span<const uint32_t> &ac_ipv4_list,
    nonstd::span<IWritableConfigurationStatusResponseOptionalElement *const> optional_elements)
    : capwap_timers{ capwap_timers },
      decryption_error_report_periods{ decryption_error_report_periods },
      idle_timeout{ idle_timeout }, wtp_fallback{ wtp_fallback }, ac_ipv4_list{ ac_ipv4_list },
      optional_elements{ optional_elements } {
}

WritableConfigurationStatusResponse::WritableConfigurationStatusResponse(
    const WritableCAPWAPTimers &capwap_timers,
    WritableDecryptionErrorReportPeriodArray &decryption_error_report_periods,
    const uint32_t idle_timeout,
    const WTPFallback::Mode wtp_fallback,
    const nonstd::span<const uint32_t> &ac_ipv4_list,
    std::initializer_list<IWritableConfigurationStatusResponseOptionalElement *> optional_elements)
    : WritableConfigurationStatusResponse(
          capwap_timers,
          decryption_error_report_periods,
          idle_timeout,
          wtp_fallback,
          ac_ipv4_list,
          nonstd::span<IWritableConfigurationStatusResponseOptionalElement *const>(
              optional_elements.begin(),
              optional_elements.size())) {
}

ControlHeader::MessageType WritableConfigurationStatusResponse::GetMessageType() const {
    return ControlHeader::ConfigurationStatusResponse;
}

ControlHeader::MessageType WritableConfigurationStatusResponse::GetRequestMessageType() const {
    return ControlHeader::ConfigurationStatusResponse;
}

void WritableConfigurationStatusResponse::Serialize(RawData *raw_data) const {
    capwap_timers.Serialize(raw_data);
    decryption_error_report_periods.Serialize(raw_data);
    idle_timeout.Serialize(raw_data);
    wtp_fallback.Serialize(raw_data);
    ac_ipv4_list.Serialize(raw_data);

    for (auto *elem : optional_elements) {
        elem->Serialize(raw_data);
    }
}

ReadableConfigurationStatusResponse::ReadableConfigurationStatusResponse(
    nonstd::span<IReadableConfigurationStatusResponseOptionalElement *const> optional_elements)
    : key_optional_elements{ MapOptionalsElements(optional_elements) }, unknown_elements{} {
}

ReadableConfigurationStatusResponse::ReadableConfigurationStatusResponse(
    std::initializer_list<IReadableConfigurationStatusResponseOptionalElement *> optional_elements)
    : ReadableConfigurationStatusResponse(
          nonstd::span<IReadableConfigurationStatusResponseOptionalElement *const>(
              optional_elements.begin(),
              optional_elements.size())) {
}

ControlHeader::MessageType ReadableConfigurationStatusResponse::GetMessageType() const {
    return ControlHeader::ConfigurationStatusResponse;
}

bool ReadableConfigurationStatusResponse::Deserialize(RawData *raw_data) {
    while (raw_data->current + sizeof(ElementHeader) <= raw_data->end) {
        ElementHeader *element = (ElementHeader *)raw_data->current;

        switch (element->GetElementType()) {
            case ElementHeader::ElementType::CAPWAPTimers:
                if (!capwap_timers.Deserialize(raw_data)) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::DecryptionErrorReportPeriod:
                if (!decryption_error_report_periods.Deserialize(raw_data)) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::IdleTimeout:
                if (!idle_timeout.Deserialize(raw_data)) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::WTPFallback:
                if (!wtp_fallback.Deserialize(raw_data)) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::ACIPv4List:
                if (!ac_ipv4_list.Deserialize(raw_data)) {
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
                log_w("ReadableConfigurationStatusResponse::Deserialize unhandled element type: "
                      "0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }
    return capwap_timers.IsPresent() && decryption_error_report_periods.Get().size() > 0
        && idle_timeout.IsPresent() && wtp_fallback.IsPresent() && ac_ipv4_list.IsPresent();
}

void ReadableConfigurationStatusResponse::Log() const {
    log_i("----------------------------------");
    log_i("ME ConfigurationStatusResponse:");
    capwap_timers.Log();

    decryption_error_report_periods.Log();

    idle_timeout.Log();

    wtp_fallback.Log();

    ac_ipv4_list.Log();

    for (const auto &[_, value] : key_optional_elements) {
        value->Log();
    }

    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %zu", unknown_elements);
    }
    log_i("----------------------------------");
}

std::unordered_map<ElementHeader::ElementType,
                   IReadableConfigurationStatusResponseOptionalElement *const>
ReadableConfigurationStatusResponse::MapOptionalsElements(
    nonstd::span<IReadableConfigurationStatusResponseOptionalElement *const> optional_elements) {

    std::unordered_map<ElementHeader::ElementType,
                       IReadableConfigurationStatusResponseOptionalElement *const>
        map;

    for (auto *elem : optional_elements) {
        map.emplace(elem->GetElementType(), elem);
    }

    return map;
}