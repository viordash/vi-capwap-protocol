#pragma once

#include "CapwapMessage.h"
#include "IElement.h"
#include "elements/ACName.h"
#include "elements/ACNameWithPriority.h"
#include "elements/CapwapTransportProtocol.h"
#include "elements/IEEE80211/Antenna.h"
#include "elements/IEEE80211/DirectSequenceControl.h"
#include "elements/IEEE80211/MACOperation.h"
#include "elements/IEEE80211/MultiDomainCapability.h"
#include "elements/IEEE80211/OFDMControl.h"
#include "elements/IEEE80211/SupportedRates.h"
#include "elements/IEEE80211/TxPower.h"
#include "elements/IEEE80211/TxPowerLevel.h"
#include "elements/IEEE80211/WTPRadioConfiguration.h"
#include "elements/IEEE80211/WTPRadioInformation.h"
#include "elements/RadioAdministrativeState.h"
#include "elements/StatisticsTimer.h"
#include "elements/VendorSpecificPayload.h"
#include "elements/WTPRebootStatistics.h"
#include "elements/WTPStaticIPAddressInformation.h"
#include "span.hpp"
#include <limits>
#include <unordered_map>

struct WritableConfigurationStatusRequest : WritableCapwapRequest {
  private:
    const WritableACName ac_name;
    WritableRadioAdministrativeStateArray &radio_states;
    const WritableStatisticsTimer statistics_timer;
    const WTPRebootStatistics &wtp_reboot_statistics;

    nonstd::span<IWritableConfigurationStatusRequestOptionalElement *const> optional_elements;

  public:
    WritableConfigurationStatusRequest(const WritableConfigurationStatusRequest &) = delete;
    WritableConfigurationStatusRequest(
        const std::string_view ac_name,
        WritableRadioAdministrativeStateArray &radio_states,
        const uint16_t statistics_timer,
        const WTPRebootStatistics &wtp_reboot_statistics,
        nonstd::span<IWritableConfigurationStatusRequestOptionalElement *const> optional_elements);



    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetResponseMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableConfigurationStatusRequest : ReadableCapwapRequest {
  protected:
    std::unordered_map<ElementHeader::ElementType,
                       IReadableConfigurationStatusRequestOptionalElement *const>
        key_optional_elements;

    std::unordered_map<ElementHeader::ElementType,
                       IReadableConfigurationStatusRequestOptionalElement *const>
    MapOptionalsElements(
        nonstd::span<IReadableConfigurationStatusRequestOptionalElement *const> optional_elements);

  public:
    ReadableACName ac_name;
    ReadableRadioAdministrativeStateArray radio_states;
    ReadableStatisticsTimer statistics_timer;
    ReadableWTPRebootStatistics wtp_reboot_statistics;

    size_t unknown_elements;

    ReadableConfigurationStatusRequest(const ReadableConfigurationStatusRequest &) = delete;
    ReadableConfigurationStatusRequest(
        nonstd::span<IReadableConfigurationStatusRequestOptionalElement *const> optional_elements);



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
