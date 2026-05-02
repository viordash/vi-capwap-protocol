#pragma once

#include "CapwapMessage.h"
#include "elements/ACDescriptor.h"
#include "elements/ACIPv4List.h"
#include "elements/ACName.h"
#include "elements/CAPWAPControlIPv4Address.h"
#include "elements/CAPWAPLocalIPv4Address.h"
#include "elements/CapwapTransportProtocol.h"
#include "elements/ECNSupport.h"
#include "elements/ElementHeader.h"
#include "elements/IEEE80211/WTPRadioInformation.h"
#include "elements/ImageIdentifier.h"
#include "elements/MaximumMessageLength.h"
#include "elements/ResultCode.h"
#include "elements/VendorSpecificPayload.h"
#include "span.hpp"
#include <limits>
#include <string_view>
#include <unordered_map>

struct WritableJoinResponse : WritableCapwapResponse {

  private:
    const WritableResultCode result_code;
    const WritableACDescriptor &ac_descriptor;
    const WritableACName ac_name;
    WritableWTPRadioInformationArray &wtp_radio_informations;
    const WritableECNSupport ecn_support;
    const WritableCAPWAPControlIPV4AdrArray control_ip_addresses;
    const WritableCAPWAPLocalIPV4AdrArray local_ip_addresses;

    nonstd::span<IWritableJoinResponseOptionalElement *const> optional_elements;

  public:
    WritableJoinResponse(const WritableJoinResponse &) = delete;
    WritableJoinResponse(
        const ResultCode::Type result_code,
        const WritableACDescriptor &ac_descriptor,
        const std::string_view ac_name,
        WritableWTPRadioInformationArray &wtp_radio_informations,
        const ECNSupport::Type ecn_support,
        const nonstd::span<const CAPWAPControlIPv4Address> &control_ip_addresses,
        const nonstd::span<const CAPWAPLocalIPv4Address> &local_ip_addresses,
        nonstd::span<IWritableJoinResponseOptionalElement *const> optional_elements);

    WritableJoinResponse(
        const ResultCode::Type result_code,
        const WritableACDescriptor &ac_descriptor,
        const std::string_view ac_name,
        WritableWTPRadioInformationArray &wtp_radio_informations,
        const ECNSupport::Type ecn_support,
        const nonstd::span<const CAPWAPControlIPv4Address> &control_ip_addresses,
        const nonstd::span<const CAPWAPLocalIPv4Address> &local_ip_addresses,
        std::initializer_list<IWritableJoinResponseOptionalElement *const> optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetRequestMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableJoinResponse : ReadableCapwapResponse {
  protected:
    std::unordered_map<ElementHeader::ElementType, IReadableJoinResponseOptionalElement *const>
        key_optional_elements;

    std::unordered_map<ElementHeader::ElementType, IReadableJoinResponseOptionalElement *const>
    MapOptionalsElements(
        nonstd::span<IReadableJoinResponseOptionalElement *const> optional_elements);

  public:
    ReadableResultCode result_code;
    ReadableACDescriptor ac_descriptor;
    ReadableACName ac_name;
    ReadableWTPRadioInformationArray wtp_radio_informations;
    ReadableECNSupport ecn_support;
    ReadableCAPWAPControlIPV4AdrArray control_ip_addresses;
    ReadableCAPWAPLocalIPV4AdrArray local_ip_addresses;

    size_t unknown_elements;

    ReadableJoinResponse(const ReadableJoinResponse &) = delete;
    ReadableJoinResponse(
        nonstd::span<IReadableJoinResponseOptionalElement *const> optional_elements);
    ReadableJoinResponse(
        std::initializer_list<IReadableJoinResponseOptionalElement *> optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;
};
