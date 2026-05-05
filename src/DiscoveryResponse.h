#pragma once

#include "CapwapMessage.h"
#include "elements/ACDescriptor.h"
#include "elements/ACName.h"
#include "elements/CAPWAPControlIPv4Address.h"
#include "elements/ElementHeader.h"
#include "elements/IEEE80211/WTPRadioInformation.h"
#include "elements/VendorSpecificPayload.h"
#include "span.hpp"
#include <limits>
#include <string_view>
#include <unordered_map>

struct WritableDiscoveryResponse : WritableCapwapResponse {

  private:
    const WritableACDescriptor &ac_descriptor;
    const WritableACName ac_name;
    WritableWTPRadioInformationArray &wtp_radio_informations;
    const WritableCAPWAPControlIPV4AdrArray ip_addresses;

    nonstd::span<IWritableDiscoveryResponseOptionalElement *const> optional_elements;

  public:
    WritableDiscoveryResponse(const WritableDiscoveryResponse &) = delete;
    WritableDiscoveryResponse(
        const WritableACDescriptor &ac_descriptor,
        const std::string_view ac_name,
        WritableWTPRadioInformationArray &wtp_radio_informations,
        const nonstd::span<const CAPWAPControlIPv4Address> &ip_addresses,
        nonstd::span<IWritableDiscoveryResponseOptionalElement *const> optional_elements);

    WritableDiscoveryResponse(
        const WritableACDescriptor &ac_descriptor,
        const std::string_view ac_name,
        WritableWTPRadioInformationArray &wtp_radio_informations,
        const nonstd::span<const CAPWAPControlIPv4Address> &ip_addresses,
        std::initializer_list<IWritableDiscoveryResponseOptionalElement *const> optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetRequestMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableDiscoveryResponse : ReadableCapwapResponse {
  protected:
    std::unordered_map<ElementHeader::ElementType, IReadableDiscoveryResponseOptionalElement *const>
        key_optional_elements;

    std::unordered_map<ElementHeader::ElementType, IReadableDiscoveryResponseOptionalElement *const>
    MapOptionalsElements(
        nonstd::span<IReadableDiscoveryResponseOptionalElement *const> optional_elements);

  public:
    ReadableACDescriptor ac_descriptor;
    ReadableACName ac_name;
    ReadableWTPRadioInformationArray wtp_radio_informations;
    ReadableCAPWAPControlIPV4AdrArray ip_addresses;

    size_t unknown_elements;

    ReadableDiscoveryResponse(const ReadableDiscoveryResponse &) = delete;
    ReadableDiscoveryResponse(
        nonstd::span<IReadableDiscoveryResponseOptionalElement *const> optional_elements);
    ReadableDiscoveryResponse(
        std::initializer_list<IReadableDiscoveryResponseOptionalElement *> optional_elements);

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
