

#include "WTPEventRequest.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"
#include "logging.h"

WritableWTPEventRequest::WritableWTPEventRequest(
    WritableDecryptionErrorReportArray &decryption_error_report,
    WritableDuplicateIPv4AdrArray &duplicate_ipv4_address,
    WritableWTPRadioStatisticsArray &wtp_radio_statistics,
    std::optional<WTPRebootStatistics> &wtp_reboot_statistics,
    WritableDeleteStationArray &delete_station,
    WritableVendorSpecificPayloadArray &vendor_specific_payloads)
    : decryption_error_report{ decryption_error_report },
      duplicate_ipv4_address{ duplicate_ipv4_address },
      wtp_radio_statistics{ wtp_radio_statistics }, wtp_reboot_statistics{ wtp_reboot_statistics },
      delete_station{ delete_station }, vendor_specific_payloads{ vendor_specific_payloads } {
}

ControlHeader::MessageType WritableWTPEventRequest::GetMessageType() const {
    return ControlHeader::WTPEventRequest;
}

ControlHeader::MessageType WritableWTPEventRequest::GetResponseMessageType() const {
    return ControlHeader::WTPEventResponse;
}

void WritableWTPEventRequest::Serialize(RawData *raw_data) const {
    decryption_error_report.Serialize(raw_data);
    duplicate_ipv4_address.Serialize(raw_data);
    wtp_radio_statistics.Serialize(raw_data);
    if (wtp_reboot_statistics.has_value()) {
        wtp_reboot_statistics.value().Serialize(raw_data);
    }
    delete_station.Serialize(raw_data);
    vendor_specific_payloads.Serialize(raw_data);
}

void WritableWTPEventRequest::Clear() {
    decryption_error_report.Clear();
    duplicate_ipv4_address.Clear();
    wtp_radio_statistics.Clear();
    wtp_reboot_statistics.reset();
    delete_station.Clear();
    vendor_specific_payloads.Clear();
}

bool WritableWTPEventRequest::Validate() {
    if (!decryption_error_report.Empty()) {
        return true;
    }
    if (!duplicate_ipv4_address.Empty()) {
        return true;
    }
    if (!wtp_radio_statistics.Empty()) {
        return true;
    }
    if (wtp_reboot_statistics.has_value()) {
        return true;
    }
    if (!delete_station.Empty()) {
        return true;
    }
    if (!vendor_specific_payloads.Empty()) {
        return true;
    }
    return false;
}

ReadableWTPEventRequest::ReadableWTPEventRequest() : unknown_elements{} {
    wtp_reboot_statistics = nullptr;
}

ControlHeader::MessageType ReadableWTPEventRequest::GetMessageType() const {
    return ControlHeader::WTPEventRequest;
}

bool ReadableWTPEventRequest::Deserialize(RawData *raw_data) {
    while (raw_data->current + sizeof(ElementHeader) <= raw_data->end) {
        ElementHeader *element = (ElementHeader *)raw_data->current;

        switch (element->GetElementType()) {
            case ElementHeader::ElementType::DecryptionErrorReport:
                if (!decryption_error_report.Deserialize(raw_data)) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::DuplicateIPv4Address:
                if (!duplicate_ipv4_address.Deserialize(raw_data)) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::WTPRadioStatistics:
                if (!wtp_radio_statistics.Deserialize(raw_data)) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::WTPRebootStatistics:
                wtp_reboot_statistics = WTPRebootStatistics::Deserialize(raw_data);
                if (wtp_reboot_statistics == nullptr) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::DeleteStation:
                if (!delete_station.Deserialize(raw_data)) {
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
                log_w("ReadableWTPEventRequest::Deserialize unhandled element type: "
                      "0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }
    return decryption_error_report.Get().size() > 0  //
        || duplicate_ipv4_address.Get().size() > 0   //
        || wtp_radio_statistics.Get().size() > 0     //
        || wtp_reboot_statistics != nullptr          //
        || delete_station.Get().size() > 0           //
        || vendor_specific_payloads.Get().size() > 0 //
        ;
}

void ReadableWTPEventRequest::Log() const {
    log_i("----------------------------------");
    log_i("ME WTPEventRequest:");

    decryption_error_report.Log();
    duplicate_ipv4_address.Log();
    wtp_radio_statistics.Log();
    if (wtp_reboot_statistics != nullptr) {
        wtp_reboot_statistics->Log();
    }
    delete_station.Log();
    vendor_specific_payloads.Log();

    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %lu", unknown_elements);
    }
    log_i("----------------------------------");
}