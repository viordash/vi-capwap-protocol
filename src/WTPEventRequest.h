#pragma once

#include "CapwapMessage.h"
#include "IElement.h"
#include "elements/DecryptionErrorReport.h"
#include "elements/DeleteStation.h"
#include "elements/DuplicateIPv4Address.h"
#include "elements/IEEE80211/MICCountermeasures.h"
#include "elements/IEEE80211/RSNAErrorReportFromStation.h"
#include "elements/IEEE80211/Statistics.h"
#include "elements/VendorSpecificPayload.h"
#include "elements/WTPRadioStatistics.h"
#include "elements/WTPRebootStatistics.h"
#include "span.hpp"
#include <limits>
#include <unordered_map>

struct WritableWTPEventRequest : WritableCapwapRequest {
  private:
    nonstd::span<IWritableWTPEventRequestOptionalElement *const> optional_elements;

  public:
    WritableWTPEventRequest(const WritableWTPEventRequest &) = delete;
    WritableWTPEventRequest(
        nonstd::span<IWritableWTPEventRequestOptionalElement *const> optional_elements);


    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetResponseMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableWTPEventRequest : ReadableCapwapRequest {
  protected:
    std::unordered_map<ElementHeader::ElementType, IReadableWTPEventRequestOptionalElement *const>
        key_optional_elements;

    std::unordered_map<ElementHeader::ElementType, IReadableWTPEventRequestOptionalElement *const>
    MapOptionalsElements(
        nonstd::span<IReadableWTPEventRequestOptionalElement *const> optional_elements);

  public:
    size_t unknown_elements;

    ReadableWTPEventRequest(const ReadableWTPEventRequest &) = delete;
    ReadableWTPEventRequest(
        nonstd::span<IReadableWTPEventRequestOptionalElement *const> optional_elements);


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
