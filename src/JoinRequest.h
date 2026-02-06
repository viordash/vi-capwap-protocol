#pragma once

#include "CapwapMessage.h"
#include "elements/CAPWAPLocalIPv4Address.h"
#include "elements/CapwapTransportProtocol.h"
#include "elements/ECNSupport.h"
#include "elements/LocationData.h"
#include "elements/MaximumMessageLength.h"
#include "elements/SessionId.h"
#include "elements/VendorSpecificPayload.h"
#include "elements/WTPBoardData.h"
#include "elements/WTPDescriptor.h"
#include "elements/WTPFrameTunnelMode.h"
#include "elements/WTPMACType.h"
#include "elements/WTPName.h"
#include "elements/WTPRadioInformation.h"
#include "elements/WTPRebootStatistics.h"
#include "span.hpp"
#include <limits>
#include <vector>

struct WritableJoinRequest : WritableCapwapRequest {
  private:
    const WritableLocationData location_data;
    const WritableWTPBoardData &wtp_board_data;
    const WritableWTPDescriptor &wtp_descriptor;
    const WritableWTPName wtp_name;
    const SessionId &session_id;
    const WTPFrameTunnelMode &wtp_frame_tunnel_mode;
    const WTPMACType wtp_mac_type;
    const WritableWTPRadioInformationArray wtp_radio_informations;
    const ECNSupport ecn_support;
    const WritableCAPWAPLocalIPV4AdrArray ip_addresses;

    const CapwapTransportProtocol *capwap_transport_protocol;
    const MaximumMessageLength *maximum_message_length;
    const WTPRebootStatistics *wtp_reboot_statistics;
    WritableVendorSpecificPayloadArray &vendor_specific_payloads;

  public:
    WritableJoinRequest(const WritableJoinRequest &) = delete;
    WritableJoinRequest(const std::string_view location_data,
                        const WritableWTPBoardData &wtp_board_data,
                        const WritableWTPDescriptor &wtp_descriptor,
                        const std::string_view wtp_name,
                        const SessionId &session_id,
                        const WTPFrameTunnelMode &wtp_frame_tunnel_mode,
                        const WTPMACType::Type mac_type,
                        const nonstd::span<const WTPRadioInformation> &wtp_radio_info,
                        const ECNSupport::Type ecn_support,
                        const nonstd::span<const CAPWAPLocalIPv4Address> &ip_addresses,
                        const CapwapTransportProtocol *capwap_transport_protocol,
                        const MaximumMessageLength *maximum_message_length,
                        const WTPRebootStatistics *wtp_reboot_statistics,
                        WritableVendorSpecificPayloadArray &vendor_specific_payloads);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetResponseMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableJoinRequest : ReadableCapwapRequest {
    ReadableLocationData *location_data;
    ReadableWTPBoardData wtp_board_data;
    ReadableWTPDescriptor wtp_descriptor;
    ReadableWTPName *wtp_name;
    SessionId *session_id;
    WTPFrameTunnelMode *wtp_frame_tunnel_mode;
    WTPMACType *wtp_mac_type;
    ReadableWTPRadioInformationArray wtp_radio_informations;
    ECNSupport *ecn_support;
    ReadableCAPWAPLocalIPV4AdrArray ip_addresses;

    CapwapTransportProtocol *capwap_transport_protocol;
    MaximumMessageLength *maximum_message_length;
    WTPRebootStatistics *wtp_reboot_statistics;

    ReadableVendorSpecificPayloadArray vendor_specific_payloads;

    size_t unknown_elements;

    ReadableJoinRequest(const ReadableJoinRequest &) = delete;
    ReadableJoinRequest();

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;
};
