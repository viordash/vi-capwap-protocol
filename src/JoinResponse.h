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
#include "elements/ImageIdentifier.h"
#include "elements/MaximumMessageLength.h"
#include "elements/ResultCode.h"
#include "elements/VendorSpecificPayload.h"
#include "elements/WTPRadioInformation.h"
#include "span.hpp"
#include <limits>
#include <string_view>
#include <vector>

struct WritableJoinResponse : WritableCapwapResponse {

  private:
    const ResultCode result_code;
    const WritableACDescriptor &ac_descriptor;
    const WritableACName ac_name;
    const WritableWTPRadioInformationArray wtp_radio_informations;
    const ECNSupport ecn_support;
    const WritableCAPWAPControlIPV4AdrArray control_ip_addresses;
    const WritableCAPWAPLocalIPV4AdrArray local_ip_addresses;

    const WritableACIPv4List *ac_ipv4_list;
    const CapwapTransportProtocol *capwap_transport_protocol;
    const WritableImageIdentifier *image_identifier;
    const MaximumMessageLength *maximum_message_length;
    WritableVendorSpecificPayloadArray &vendor_specific_payloads;

  public:
    WritableJoinResponse(const WritableJoinResponse &) = delete;
    WritableJoinResponse(const ResultCode::Type result_code,
                         const WritableACDescriptor &ac_descriptor,
                         const std::string_view ac_name,
                         const nonstd::span<const WTPRadioInformation> &wtp_radio_info,
                         const ECNSupport::Type ecn_support,
                         const nonstd::span<const CAPWAPControlIPv4Address> &control_ip_addresses,
                         const nonstd::span<const CAPWAPLocalIPv4Address> &local_ip_addresses,
                         const WritableACIPv4List *ac_ipv4_list,
                         const CapwapTransportProtocol *capwap_transport_protocol,
                         const WritableImageIdentifier *image_identifier,
                         const MaximumMessageLength *maximum_message_length,
                         WritableVendorSpecificPayloadArray &vendor_specific_payloads);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetRequestMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableJoinResponse : ReadableCapwapResponse {
    ResultCode *result_code;
    ReadableACDescriptor ac_descriptor;
    ReadableACName *ac_name;
    ReadableWTPRadioInformationArray wtp_radio_informations;
    ECNSupport *ecn_support;
    ReadableCAPWAPControlIPV4AdrArray control_ip_addresses;
    ReadableCAPWAPLocalIPV4AdrArray local_ip_addresses;

    ReadableACIPv4List *ac_ipv4_list;
    CapwapTransportProtocol *capwap_transport_protocol;
    ReadableImageIdentifier image_identifier;
    MaximumMessageLength *maximum_message_length;
    ReadableVendorSpecificPayloadArray vendor_specific_payloads;

    size_t unknown_elements;

    ReadableJoinResponse(const ReadableJoinResponse &) = delete;
    ReadableJoinResponse();

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;
};
