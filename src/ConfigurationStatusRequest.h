#pragma once

#include "CapwapMessage.h"
#include "elements/ACName.h"
#include "elements/ACNameWithPriority.h"
#include "elements/CapwapTransportProtocol.h"
#include "elements/RadioAdministrativeState.h"
#include "elements/StatisticsTimer.h"
#include "elements/VendorSpecificPayload.h"
#include "elements/WTPRebootStatistics.h"
#include "elements/WTPStaticIPAddressInformation.h"
#include "span.hpp"
#include <limits>
#include <vector>

struct WritableConfigurationStatusRequest : WritableCapwapRequest {
  private:
    const WritableACName ac_name;
    WritableRadioAdministrativeStateArray &radio_states;
    const StatisticsTimer statistics_timer;
    const WTPRebootStatistics &wtp_reboot_statistics;

    WritableACNameWithPriorityArray &ac_names_with_priority;
    const CapwapTransportProtocol *capwap_transport_protocol;
    const WTPStaticIPAddressInformation *wtp_static_ipaddress;
    WritableVendorSpecificPayloadArray &vendor_specific_payloads;

  public:
    WritableConfigurationStatusRequest(const WritableConfigurationStatusRequest &) = delete;
    WritableConfigurationStatusRequest(
        const std::string_view ac_name,
        WritableRadioAdministrativeStateArray &radio_states,
        const uint16_t statistics_timer,
        const WTPRebootStatistics &wtp_reboot_statistics,
        WritableACNameWithPriorityArray &ac_names_with_priority,
        const CapwapTransportProtocol *capwap_transport_protocol,
        const WTPStaticIPAddressInformation *wtp_static_ipaddress,
        WritableVendorSpecificPayloadArray &vendor_specific_payloads);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetResponseMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableConfigurationStatusRequest : ReadableCapwapRequest {
    ReadableACName *ac_name;
    ReadableRadioAdministrativeStateArray radio_states;
    StatisticsTimer *statistics_timer;
    WTPRebootStatistics *wtp_reboot_statistics;

    ReadableACNameWithPriorityArray ac_names_with_priority;
    CapwapTransportProtocol *capwap_transport_protocol;
    WTPStaticIPAddressInformation *wtp_static_ipaddress;
    ReadableVendorSpecificPayloadArray vendor_specific_payloads;

    size_t unknown_elements;

    ReadableConfigurationStatusRequest(const ReadableConfigurationStatusRequest &) = delete;
    ReadableConfigurationStatusRequest();

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;
};
