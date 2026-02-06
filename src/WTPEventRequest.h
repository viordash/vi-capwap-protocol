#pragma once

#include "CapwapMessage.h"
#include "elements/DecryptionErrorReport.h"
#include "elements/DeleteStation.h"
#include "elements/DuplicateIPv4Address.h"
#include "elements/VendorSpecificPayload.h"
#include "elements/WTPRadioStatistics.h"
#include "elements/WTPRebootStatistics.h"
#include "span.hpp"
#include <limits>
#include <optional>
#include <vector>

struct WritableWTPEventRequest : WritableCapwapRequest {
  private:
    WritableDecryptionErrorReportArray &decryption_error_report;
    WritableDuplicateIPv4AdrArray &duplicate_ipv4_address;
    WritableWTPRadioStatisticsArray &wtp_radio_statistics;
    std::optional<WTPRebootStatistics> &wtp_reboot_statistics;
    WritableDeleteStationArray &delete_station;
    WritableVendorSpecificPayloadArray &vendor_specific_payloads;

  public:
    WritableWTPEventRequest(const WritableWTPEventRequest &) = delete;
    WritableWTPEventRequest(WritableDecryptionErrorReportArray &decryption_error_report,
                            WritableDuplicateIPv4AdrArray &duplicate_ipv4_address,
                            WritableWTPRadioStatisticsArray &wtp_radio_statistics,
                            std::optional<WTPRebootStatistics> &wtp_reboot_statistics,
                            WritableDeleteStationArray &delete_station,
                            WritableVendorSpecificPayloadArray &vendor_specific_payloads);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetResponseMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
    void Clear();
    bool Validate();
};

struct ReadableWTPEventRequest : ReadableCapwapRequest {
    ReadableDecryptionErrorReportArray decryption_error_report;
    ReadableDuplicateIPv4AdrArray duplicate_ipv4_address;
    ReadableWTPRadioStatisticsArray wtp_radio_statistics;
    WTPRebootStatistics *wtp_reboot_statistics;
    ReadableDeleteStationArray delete_station;
    ReadableVendorSpecificPayloadArray vendor_specific_payloads;

    size_t unknown_elements;

    ReadableWTPEventRequest(const ReadableWTPEventRequest &) = delete;
    ReadableWTPEventRequest();

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;
};
