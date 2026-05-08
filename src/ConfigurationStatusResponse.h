#pragma once

#include "CapwapMessage.h"
#include "IElement.h"
#include "elements/ACIPv4List.h"
#include "elements/CAPWAPTimers.h"
#include "elements/DecryptionErrorReportPeriod.h"
#include "elements/IEEE80211/Antenna.h"
#include "elements/IEEE80211/DirectSequenceControl.h"
#include "elements/IEEE80211/MACOperation.h"
#include "elements/IEEE80211/MultiDomainCapability.h"
#include "elements/IEEE80211/OFDMControl.h"
#include "elements/IEEE80211/RateSet.h"
#include "elements/IEEE80211/SupportedRates.h"
#include "elements/IEEE80211/TxPower.h"
#include "elements/IEEE80211/WTPQualityOfService.h"
#include "elements/IEEE80211/WTPRadioConfiguration.h"
#include "elements/IdleTimeout.h"
#include "elements/VendorSpecificPayload.h"
#include "elements/WTPFallback.h"
#include "elements/WTPStaticIPAddressInformation.h"
#include "span.hpp"
#include <limits>
#include <unordered_map>

struct WritableConfigurationStatusResponse : WritableCapwapResponse {
  private:
    const WritableCAPWAPTimers &capwap_timers;
    WritableDecryptionErrorReportPeriodArray &decryption_error_report_periods;
    const WritableIdleTimeout idle_timeout;
    const WritableWTPFallback wtp_fallback;
    const WritableACIPv4List ac_ipv4_list;

    nonstd::span<IWritableConfigurationStatusResponseOptionalElement *const> optional_elements;

  public:
    WritableConfigurationStatusResponse(const WritableConfigurationStatusResponse &) = delete;
    WritableConfigurationStatusResponse(
        const WritableCAPWAPTimers &capwap_timers,
        WritableDecryptionErrorReportPeriodArray &decryption_error_report_periods,
        const uint32_t idle_timeout,
        const WTPFallback::Mode wtp_fallback,
        const nonstd::span<const uint32_t> &ac_ipv4_list,
        nonstd::span<IWritableConfigurationStatusResponseOptionalElement *const> optional_elements);



    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetRequestMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableConfigurationStatusResponse : ReadableCapwapResponse {
  protected:
    std::unordered_map<ElementHeader::ElementType,
                       IReadableConfigurationStatusResponseOptionalElement *const>
        key_optional_elements;

    std::unordered_map<ElementHeader::ElementType,
                       IReadableConfigurationStatusResponseOptionalElement *const>
    MapOptionalsElements(
        nonstd::span<IReadableConfigurationStatusResponseOptionalElement *const> optional_elements);

  public:
    ReadableCAPWAPTimers capwap_timers;
    ReadableDecryptionErrorReportPeriodArray decryption_error_report_periods;
    ReadableIdleTimeout idle_timeout;
    ReadableWTPFallback wtp_fallback;
    ReadableACIPv4List ac_ipv4_list;

    size_t unknown_elements;

    ReadableConfigurationStatusResponse(const ReadableConfigurationStatusResponse &) = delete;
    ReadableConfigurationStatusResponse(
        nonstd::span<IReadableConfigurationStatusResponseOptionalElement *const> optional_elements);



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
