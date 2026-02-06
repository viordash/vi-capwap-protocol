#pragma once

#include "CapwapMessage.h"
#include "elements/ACIPv4List.h"
#include "elements/CAPWAPTimers.h"
#include "elements/DecryptionErrorReportPeriod.h"
#include "elements/IdleTimeout.h"
#include "elements/VendorSpecificPayload.h"
#include "elements/WTPFallback.h"
#include "elements/WTPStaticIPAddressInformation.h"
#include "span.hpp"
#include <limits>
#include <vector>

struct WritableConfigurationStatusResponse : WritableCapwapResponse {
  private:
    const CAPWAPTimers &capwap_timers;
    WritableDecryptionErrorReportPeriodArray &decryption_error_report_periods;
    const IdleTimeout idle_timeout;
    const WTPFallback wtp_fallback;
    const WritableACIPv4List ac_ipv4_list;

    const WTPStaticIPAddressInformation *wtp_static_ipaddress;
    WritableVendorSpecificPayloadArray &vendor_specific_payloads;

  public:
    WritableConfigurationStatusResponse(const WritableConfigurationStatusResponse &) = delete;
    WritableConfigurationStatusResponse(
        const CAPWAPTimers &capwap_timers,
        WritableDecryptionErrorReportPeriodArray &decryption_error_report_periods,
        const uint32_t idle_timeout,
        const WTPFallback::Mode wtp_fallback,
        const nonstd::span<const uint32_t> &ac_ipv4_list,
        const WTPStaticIPAddressInformation *wtp_static_ipaddress,
        WritableVendorSpecificPayloadArray &vendor_specific_payloads);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetRequestMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableConfigurationStatusResponse : ReadableCapwapResponse {
    CAPWAPTimers *capwap_timers;
    ReadableDecryptionErrorReportPeriodArray decryption_error_report_periods;
    IdleTimeout *idle_timeout;
    WTPFallback *wtp_fallback;
    ReadableACIPv4List *ac_ipv4_list;

    WTPStaticIPAddressInformation *wtp_static_ipaddress;
    ReadableVendorSpecificPayloadArray vendor_specific_payloads;

    size_t unknown_elements;

    ReadableConfigurationStatusResponse(const ReadableConfigurationStatusResponse &) = delete;
    ReadableConfigurationStatusResponse();

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;
};
