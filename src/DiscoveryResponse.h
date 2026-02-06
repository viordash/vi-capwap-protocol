#pragma once

#include "CapwapMessage.h"
#include "elements/ACDescriptor.h"
#include "elements/ACName.h"
#include "elements/CAPWAPControlIPv4Address.h"
#include "elements/ElementHeader.h"
#include "elements/VendorSpecificPayload.h"
#include "elements/WTPRadioInformation.h"
#include "span.hpp"
#include <limits>
#include <string_view>
#include <vector>

struct WritableDiscoveryResponse : WritableCapwapResponse {

  private:
    const WritableACDescriptor &ac_descriptor;
    const WritableACName ac_name;
    const WritableWTPRadioInformationArray wtp_radio_informations;
    const WritableCAPWAPControlIPV4AdrArray ip_addresses;

    WritableVendorSpecificPayloadArray &vendor_specific_payloads;

    uint16_t CalcTotalSize();

  public:
    WritableDiscoveryResponse(const WritableDiscoveryResponse &) = delete;
    WritableDiscoveryResponse(const WritableACDescriptor &ac_descriptor,
                              const std::string_view ac_name,
                              const nonstd::span<const WTPRadioInformation> &wtp_radio_info,
                              const nonstd::span<const CAPWAPControlIPv4Address> &ip_addresses,
                              WritableVendorSpecificPayloadArray &vendor_specific_payloads);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetRequestMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableDiscoveryResponse : ReadableCapwapResponse {
    ReadableACDescriptor ac_descriptor;
    ReadableACName *ac_name;
    ReadableWTPRadioInformationArray wtp_radio_informations;
    ReadableCAPWAPControlIPV4AdrArray ip_addresses;
    ReadableVendorSpecificPayloadArray vendor_specific_payloads;

    size_t unknown_elements;

    ReadableDiscoveryResponse(const ReadableDiscoveryResponse &) = delete;
    ReadableDiscoveryResponse();

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;
};
