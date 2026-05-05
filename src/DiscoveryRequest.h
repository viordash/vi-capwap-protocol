#pragma once

#include "CapwapMessage.h"
#include "IElement.h"
#include "elements/DiscoveryType.h"
#include "elements/IEEE80211/WTPRadioInformation.h"
#include "elements/MTUDiscoveryPadding.h"
#include "elements/VendorSpecificPayload.h"
#include "elements/WTPBoardData.h"
#include "elements/WTPDescriptor.h"
#include "elements/WTPFrameTunnelMode.h"
#include "elements/WTPMACType.h"
#include "span.hpp"
#include <limits>
#include <unordered_map>

struct WritableDiscoveryRequest : WritableCapwapRequest {

  private:
    const WritableDiscoveryType discovery_type;
    const WritableWTPBoardData &wtp_board_data;
    const WritableWTPDescriptor &wtp_descriptor;
    const WritableWTPFrameTunnelMode &wtp_frame_tunnel_mode;
    const WritableWTPMACType wtp_mac_type;
    WritableWTPRadioInformationArray &wtp_radio_informations;

    nonstd::span<IWritableDiscoveryRequestOptionalElement *const> optional_elements;

    const WritableMTUDiscoveryPadding padding;

    uint16_t CalcTotalSize();
    uint16_t CalcMessageLength(uint16_t probe_mtu_size);
    uint16_t CalcMTUPadding(uint16_t probe_mtu_size);

  public:
    static const uint16_t no_probe_mtu_size = std::numeric_limits<uint16_t>::min();

    WritableDiscoveryRequest(const WritableDiscoveryRequest &) = delete;
    WritableDiscoveryRequest(
        const DiscoveryType::Type discovery_type,
        const WritableWTPBoardData &wtp_board_data,
        const WritableWTPDescriptor &wtp_descriptor,
        const WritableWTPFrameTunnelMode &wtp_frame_tunnel_mode,
        const WTPMACType::Type mac_type,
        WritableWTPRadioInformationArray &wtp_radio_informations,
        nonstd::span<IWritableDiscoveryRequestOptionalElement *const> optional_elements,
        const uint16_t probe_mtu_size = no_probe_mtu_size);

    WritableDiscoveryRequest(
        const DiscoveryType::Type discovery_type,
        const WritableWTPBoardData &wtp_board_data,
        const WritableWTPDescriptor &wtp_descriptor,
        const WritableWTPFrameTunnelMode &wtp_frame_tunnel_mode,
        const WTPMACType::Type mac_type,
        WritableWTPRadioInformationArray &wtp_radio_informations,
        std::initializer_list<IWritableDiscoveryRequestOptionalElement *const> optional_elements,
        const uint16_t probe_mtu_size = no_probe_mtu_size);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetResponseMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableDiscoveryRequest : ReadableCapwapRequest {
  protected:
    std::unordered_map<ElementHeader::ElementType, IReadableDiscoveryRequestOptionalElement *const>
        key_optional_elements;

    std::unordered_map<ElementHeader::ElementType, IReadableDiscoveryRequestOptionalElement *const>
    MapOptionalsElements(
        nonstd::span<IReadableDiscoveryRequestOptionalElement *const> optional_elements);

  public:
    ReadableDiscoveryType discovery_type;
    ReadableWTPBoardData wtp_board_data;
    ReadableWTPDescriptor wtp_descriptor;
    ReadableWTPFrameTunnelMode wtp_frame_tunnel_mode;
    ReadableWTPMACType wtp_mac_type;
    ReadableWTPRadioInformationArray wtp_radio_informations;

    ReadableMTUDiscoveryPadding padding;

    size_t unknown_elements;

    ReadableDiscoveryRequest(const ReadableDiscoveryRequest &) = delete;
    ReadableDiscoveryRequest(
        nonstd::span<IReadableDiscoveryRequestOptionalElement *const> optional_elements);

    ReadableDiscoveryRequest(
        std::initializer_list<IReadableDiscoveryRequestOptionalElement *> optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;

    template <typename T> T *GetOptionalElement(ElementHeader::ElementType element_type) {
        auto it = key_optional_elements.find(element_type);
        if (it != key_optional_elements.end()) {
            return static_cast<T *>(it->second);
        }
        return nullptr;
    }
};
