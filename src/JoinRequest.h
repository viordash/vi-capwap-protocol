#pragma once

#include "CapwapMessage.h"
#include "IElement.h"
#include "elements/CAPWAPLocalIPv4Address.h"
#include "elements/CapwapTransportProtocol.h"
#include "elements/ECNSupport.h"
#include "elements/IEEE80211/WTPRadioInformation.h"
#include "elements/LocationData.h"
#include "elements/MaximumMessageLength.h"
#include "elements/SessionId.h"
#include "elements/VendorSpecificPayload.h"
#include "elements/WTPBoardData.h"
#include "elements/WTPDescriptor.h"
#include "elements/WTPFrameTunnelMode.h"
#include "elements/WTPMACType.h"
#include "elements/WTPName.h"
#include "elements/WTPRebootStatistics.h"
#include "span.hpp"
#include <unordered_map>

struct WritableJoinRequest : WritableCapwapRequest {
  private:
    const WritableLocationData location_data;
    const WritableWTPBoardData &wtp_board_data;
    const WritableWTPDescriptor &wtp_descriptor;
    const WritableWTPName wtp_name;
    const WritableSessionId session_id;
    const WTPFrameTunnelMode &wtp_frame_tunnel_mode;
    const WTPMACType wtp_mac_type;
    WritableWTPRadioInformationArray &wtp_radio_informations;
    const WritableECNSupport ecn_support;
    const WritableCAPWAPLocalIPV4AdrArray ip_addresses;

    nonstd::span<IWritableJoinRequestOptionalElement *const> optional_elements;

  public:
    WritableJoinRequest(const WritableJoinRequest &) = delete;
    WritableJoinRequest(const std::string_view location_data,
                        const WritableWTPBoardData &wtp_board_data,
                        const WritableWTPDescriptor &wtp_descriptor,
                        const std::string_view wtp_name,
                        const SessionId &session_id,
                        const WTPFrameTunnelMode &wtp_frame_tunnel_mode,
                        const WTPMACType::Type mac_type,
                        WritableWTPRadioInformationArray &wtp_radio_informations,
                        const ECNSupport::Type ecn_support,
                        const nonstd::span<const CAPWAPLocalIPv4Address> &ip_addresses,
                        nonstd::span<IWritableJoinRequestOptionalElement *const> optional_elements);

    WritableJoinRequest(
        const std::string_view location_data,
        const WritableWTPBoardData &wtp_board_data,
        const WritableWTPDescriptor &wtp_descriptor,
        const std::string_view wtp_name,
        const SessionId &session_id,
        const WTPFrameTunnelMode &wtp_frame_tunnel_mode,
        const WTPMACType::Type mac_type,
        WritableWTPRadioInformationArray &wtp_radio_informations,
        const ECNSupport::Type ecn_support,
        const nonstd::span<const CAPWAPLocalIPv4Address> &ip_addresses,
        std::initializer_list<IWritableJoinRequestOptionalElement *> optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetResponseMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableJoinRequest : ReadableCapwapRequest {
  protected:
    std::unordered_map<ElementHeader::ElementType, IReadableJoinRequestOptionalElement *const>
        key_optional_elements;

    std::unordered_map<ElementHeader::ElementType, IReadableJoinRequestOptionalElement *const>
    MapOptionalsElements(
        nonstd::span<IReadableJoinRequestOptionalElement *const> optional_elements);

  public:
    ReadableLocationData location_data;
    ReadableWTPBoardData wtp_board_data;
    ReadableWTPDescriptor wtp_descriptor;
    ReadableWTPName wtp_name;
    ReadableSessionId session_id;
    WTPFrameTunnelMode *wtp_frame_tunnel_mode;
    WTPMACType *wtp_mac_type;
    ReadableWTPRadioInformationArray wtp_radio_informations;
    ReadableECNSupport ecn_support;
    ReadableCAPWAPLocalIPV4AdrArray ip_addresses;

    size_t unknown_elements;

    ReadableJoinRequest(const ReadableJoinRequest &) = delete;
    ReadableJoinRequest(nonstd::span<IReadableJoinRequestOptionalElement *const> optional_elements);

    ReadableJoinRequest(
        std::initializer_list<IReadableJoinRequestOptionalElement *> optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;
};
