#pragma once

#include "CapwapMessage.h"
#include "elements/ACNameWithPriority.h"
#include "elements/ACTimestamp.h"
#include "elements/AddMacAclEntry.h"
#include "elements/CAPWAPTimers.h"
#include "elements/DecryptionErrorReportPeriod.h"
#include "elements/DeleteMacAclEntry.h"
#include "elements/IdleTimeout.h"
#include "elements/ImageIdentifier.h"
#include "elements/LocationData.h"
#include "elements/RadioAdministrativeState.h"
#include "elements/StatisticsTimer.h"
#include "elements/VendorSpecificPayload.h"
#include "elements/WTPFallback.h"
#include "elements/WTPName.h"
#include "elements/WTPStaticIPAddressInformation.h"
#include "span.hpp"
#include <limits>
#include <optional>
#include <vector>

struct WritableConfigurationUpdateRequest : WritableCapwapRequest {

  private:
    WritableACNameWithPriorityArray &ac_names_with_priority;
    std::optional<ACTimestamp> &ac_timestamp;
    WritableAddMacAclEntry &add_mac_acl_entry;
    std::optional<CAPWAPTimers> &capwap_timers;
    WritableDecryptionErrorReportPeriodArray &decryption_error_report_periods;
    WritableDeleteMacAclEntry &delete_mac_acl_entry;
    std::optional<IdleTimeout> &idle_timeout;
    std::optional<WritableLocationData> &location_data;
    WritableRadioAdministrativeStateArray &radio_states;
    std::optional<StatisticsTimer> &statistics_timer;
    std::optional<WTPFallback> &wtp_fallback;
    std::optional<WritableWTPName> &wtp_name;
    std::optional<WTPStaticIPAddressInformation> &wtp_static_ipaddress;
    std::optional<WritableImageIdentifier> &image_identifier;
    WritableVendorSpecificPayloadArray &vendor_specific_payloads;

  public:
    WritableConfigurationUpdateRequest(const WritableConfigurationUpdateRequest &) = delete;
    WritableConfigurationUpdateRequest(
        WritableACNameWithPriorityArray &ac_names_with_priority,
        std::optional<ACTimestamp> &ac_timestamp,
        WritableAddMacAclEntry &add_mac_acl_entry,
        std::optional<CAPWAPTimers> &capwap_timers,
        WritableDecryptionErrorReportPeriodArray &decryption_error_report_periods,
        WritableDeleteMacAclEntry &delete_mac_acl_entry,
        std::optional<IdleTimeout> &idle_timeout,
        std::optional<WritableLocationData> &location_data,
        WritableRadioAdministrativeStateArray &radio_states,
        std::optional<StatisticsTimer> &statistics_timer,
        std::optional<WTPFallback> &wtp_fallback,
        std::optional<WritableWTPName> &wtp_name,
        std::optional<WTPStaticIPAddressInformation> &wtp_static_ipaddress,
        std::optional<WritableImageIdentifier> &image_identifier,
        WritableVendorSpecificPayloadArray &vendor_specific_payloads);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetResponseMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
    void Clear();
    bool Validate();
};

struct ReadableConfigurationUpdateRequest : ReadableCapwapRequest {
    ReadableACNameWithPriorityArray ac_names_with_priority;
    ACTimestamp *ac_timestamp;
    ReadableAddMacAclEntry add_mac_acl_entry;
    CAPWAPTimers *capwap_timers;
    ReadableDecryptionErrorReportPeriodArray decryption_error_report_periods;
    ReadableDeleteMacAclEntry delete_mac_acl_entry;
    IdleTimeout *idle_timeout;
    ReadableLocationData *location_data;
    ReadableRadioAdministrativeStateArray radio_states;
    StatisticsTimer *statistics_timer;
    WTPFallback *wtp_fallback;
    ReadableWTPName *wtp_name;
    WTPStaticIPAddressInformation *wtp_static_ipaddress;
    ReadableImageIdentifier image_identifier;
    ReadableVendorSpecificPayloadArray vendor_specific_payloads;

    size_t unknown_elements;

    ReadableConfigurationUpdateRequest(const ReadableConfigurationUpdateRequest &) = delete;
    ReadableConfigurationUpdateRequest();

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;
};
