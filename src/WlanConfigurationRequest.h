#pragma once

#include "CapwapMessage.h"
#include "IElement.h"
#include "elements/IEEE80211/AddWlan.h"
#include "elements/IEEE80211/DeleteWlan.h"
#include "elements/IEEE80211/InformationElement.h"
#include "elements/IEEE80211/UpdateWlan.h"
#include "elements/VendorSpecificPayload.h"
#include "span.hpp"
#include <limits>
#include <unordered_map>

struct WritableWlanConfigurationRequest : WritableCapwapRequest {
  protected:
    WritableAddWlanArray *add_wlan;
    WritableDeleteWlanArray *delete_wlan;
    WritableUpdateWlanArray *update_wlan;

    nonstd::span<IWritableWlanConfigurationRequestOptionalElement *const> optional_elements;

  public:
    WritableWlanConfigurationRequest(const WritableWlanConfigurationRequest &) = delete;
    WritableWlanConfigurationRequest(WritableAddWlanArray *add_wlan);
    WritableWlanConfigurationRequest(
        WritableAddWlanArray *add_wlan,
        nonstd::span<IWritableWlanConfigurationRequestOptionalElement *const> optional_elements);

    WritableWlanConfigurationRequest(WritableDeleteWlanArray *delete_wlan);
    WritableWlanConfigurationRequest(
        WritableDeleteWlanArray *delete_wlan,
        nonstd::span<IWritableWlanConfigurationRequestOptionalElement *const> optional_elements);

    WritableWlanConfigurationRequest(WritableUpdateWlanArray *update_wlan);
    WritableWlanConfigurationRequest(
        WritableUpdateWlanArray *update_wlan,
        nonstd::span<IWritableWlanConfigurationRequestOptionalElement *const> optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetResponseMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableWlanConfigurationRequest : ReadableCapwapRequest {
  protected:
    std::unordered_map<ElementHeader::ElementType,
                       IReadableWlanConfigurationRequestOptionalElement *const>
        key_optional_elements;

    std::unordered_map<ElementHeader::ElementType,
                       IReadableWlanConfigurationRequestOptionalElement *const>
    MapOptionalsElements(
        nonstd::span<IReadableWlanConfigurationRequestOptionalElement *const> optional_elements);

  public:
    ReadableAddWlanArray add_wlan;
    ReadableDeleteWlanArray delete_wlan;
    ReadableUpdateWlanArray update_wlan;

    size_t unknown_elements;

    ReadableWlanConfigurationRequest(const ReadableWlanConfigurationRequest &) = delete;
    ReadableWlanConfigurationRequest();
    ReadableWlanConfigurationRequest(
        nonstd::span<IReadableWlanConfigurationRequestOptionalElement *const> optional_elements);

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
