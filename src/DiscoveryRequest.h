#pragma once

#include "CapwapMessage.h"
#include "elements/DiscoveryType.h"
#include "elements/MTUDiscoveryPadding.h"
#include "elements/VendorSpecificPayload.h"
#include "elements/WTPBoardData.h"
#include "elements/WTPDescriptor.h"
#include "elements/WTPFrameTunnelMode.h"
#include "elements/WTPMACType.h"
#include "elements/WTPRadioInformation.h"
#include "span.hpp"
#include <limits>
#include <vector>

struct WritableDiscoveryRequest : WritableCapwapRequest {

  private:
    const DiscoveryType discovery_type;
    const WritableWTPBoardData &wtp_board_data;
    const WritableWTPDescriptor &wtp_descriptor;
    const WTPFrameTunnelMode &wtp_frame_tunnel_mode;
    const WTPMACType wtp_mac_type;
    const WritableWTPRadioInformationArray wtp_radio_informations;

    WritableVendorSpecificPayloadArray &vendor_specific_payloads;

    const MTUDiscoveryPadding padding;

    uint16_t CalcTotalSize();
    uint16_t CalcMessageLength(uint16_t probe_mtu_size);
    uint16_t CalcMTUPadding(uint16_t probe_mtu_size);

  public:
    static const uint16_t no_probe_mtu_size = std::numeric_limits<uint16_t>::min();

    WritableDiscoveryRequest(const WritableDiscoveryRequest &) = delete;
    WritableDiscoveryRequest(const WritableWTPBoardData &wtp_board_data,
                             const WritableWTPDescriptor &wtp_descriptor,
                             const WTPFrameTunnelMode &wtp_frame_tunnel_mode,
                             const WTPMACType::Type mac_type,
                             const nonstd::span<const WTPRadioInformation> &wtp_radio_info,
                             WritableVendorSpecificPayloadArray &vendor_specific_payloads,
                             const uint16_t probe_mtu_size = no_probe_mtu_size);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetResponseMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableDiscoveryRequest : ReadableCapwapRequest {
    DiscoveryType *discovery_type;
    ReadableWTPBoardData wtp_board_data;
    ReadableWTPDescriptor wtp_descriptor;
    WTPFrameTunnelMode *wtp_frame_tunnel_mode;
    WTPMACType *wtp_mac_type;
    ReadableWTPRadioInformationArray wtp_radio_informations;

    ReadableVendorSpecificPayloadArray vendor_specific_payloads;
    MTUDiscoveryPadding *padding;

    size_t unknown_elements;

    ReadableDiscoveryRequest(const ReadableDiscoveryRequest &) = delete;
    ReadableDiscoveryRequest();

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;
};
