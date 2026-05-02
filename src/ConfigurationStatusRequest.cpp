

#include "ConfigurationStatusRequest.h"
#include "Logging.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"

WritableConfigurationStatusRequest::WritableConfigurationStatusRequest(
    const std::string_view ac_name,
    WritableRadioAdministrativeStateArray &radio_states,
    const uint16_t statistics_timer,
    const WTPRebootStatistics &wtp_reboot_statistics,
    nonstd::span<IWritableConfigurationStatusRequestOptionalElement *const> optional_elements)

    : ac_name{ ac_name }, radio_states{ radio_states }, statistics_timer{ statistics_timer },
      wtp_reboot_statistics{ wtp_reboot_statistics }, optional_elements{ optional_elements } {
}

WritableConfigurationStatusRequest::WritableConfigurationStatusRequest(
    const std::string_view ac_name,
    WritableRadioAdministrativeStateArray &radio_states,
    const uint16_t statistics_timer,
    const WTPRebootStatistics &wtp_reboot_statistics,
    std::initializer_list<IWritableConfigurationStatusRequestOptionalElement *> optional_elements)
    : WritableConfigurationStatusRequest(
          ac_name,
          radio_states,
          statistics_timer,
          wtp_reboot_statistics,
          nonstd::span<IWritableConfigurationStatusRequestOptionalElement *const>(
              optional_elements.begin(),
              optional_elements.size())) {
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
    WritableWTPRebootStatistics{ wtp_reboot_statistics.GetRebootCount(),
                                 wtp_reboot_statistics.GetACInitiatedCount(),
                                 wtp_reboot_statistics.GetLinkFailureCount(),
                                 wtp_reboot_statistics.GetSWFailureCount(),
                                 wtp_reboot_statistics.GetHWFailureCount(),
                                 wtp_reboot_statistics.GetOtherFailureCount(),
                                 wtp_reboot_statistics.GetUnknownFailureCount(),
                                 wtp_reboot_statistics.GetLastFailureType() }
        .Serialize(raw_data);

    for (auto *elem : optional_elements) {
        elem->Serialize(raw_data);
    }
}

ReadableConfigurationStatusRequest::ReadableConfigurationStatusRequest(
    nonstd::span<IReadableConfigurationStatusRequestOptionalElement *const> optional_elements)
    : key_optional_elements{ MapOptionalsElements(optional_elements) }, unknown_elements{} {
    ac_name = nullptr;
}

ReadableConfigurationStatusRequest::ReadableConfigurationStatusRequest(
    std::initializer_list<IReadableConfigurationStatusRequestOptionalElement *> optional_elements)
    : ReadableConfigurationStatusRequest(
          nonstd::span<IReadableConfigurationStatusRequestOptionalElement *const>(
              optional_elements.begin(),
              optional_elements.size())) {
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
                if (!statistics_timer.Deserialize(raw_data)) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::WTPRebootStatistics:
                if (!wtp_reboot_statistics.Deserialize(raw_data)) {
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
                log_w("ReadableConfigurationStatusRequest::Deserialize unhandled element type: "
                      "0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }
    return ac_name != nullptr && radio_states.IsPresent() > 0 && statistics_timer.IsPresent()
        && wtp_reboot_statistics.IsPresent();
}

void ReadableConfigurationStatusRequest::Log() const {
    log_i("----------------------------------");
    log_i("ME ConfigurationStatusRequest:");

    ASSERT(ac_name != nullptr);
    ac_name->Log();

    radio_states.Log();

    statistics_timer.Log();

    wtp_reboot_statistics.Log();

    for (const auto &[_, value] : key_optional_elements) {
        value->Log();
    }

    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %zu", unknown_elements);
    }
    log_i("----------------------------------");
}

std::unordered_map<ElementHeader::ElementType,
                   IReadableConfigurationStatusRequestOptionalElement *const>
ReadableConfigurationStatusRequest::MapOptionalsElements(
    nonstd::span<IReadableConfigurationStatusRequestOptionalElement *const> optional_elements) {

    std::unordered_map<ElementHeader::ElementType,
                       IReadableConfigurationStatusRequestOptionalElement *const>
        map;

    for (auto *elem : optional_elements) {
        map.emplace(elem->GetElementType(), elem);
    }

    return map;
}