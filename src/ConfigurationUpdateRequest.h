#pragma once

#include "CapwapMessage.h"
#include "IElement.h"
#include "elements/ACNameWithPriority.h"
#include "elements/ACTimestamp.h"
#include "elements/AddMacAclEntry.h"
#include "elements/CAPWAPTimers.h"
#include "elements/DecryptionErrorReportPeriod.h"
#include "elements/DeleteMacAclEntry.h"
#include "elements/IdleTimeout.h"
#include "elements/ImageIdentifier.h"
#include "elements/LocationData.h"
#include "elements/RadioAdministrativeState.h"
#include "elements/StatisticsTimer.h"
#include "elements/VendorSpecificPayload.h"
#include "elements/WTPFallback.h"
#include "elements/WTPName.h"
#include "elements/WTPStaticIPAddressInformation.h"
#include "span.hpp"
#include <unordered_map>

struct WritableConfigurationUpdateRequest : WritableCapwapRequest {

  private:
    nonstd::span<IWritableConfigurationUpdateRequestOptionalElement *const> optional_elements;

  public:
    WritableConfigurationUpdateRequest(const WritableConfigurationUpdateRequest &) = delete;

    WritableConfigurationUpdateRequest(
        nonstd::span<IWritableConfigurationUpdateRequestOptionalElement *const> optional_elements);
    WritableConfigurationUpdateRequest(
        std::initializer_list<IWritableConfigurationUpdateRequestOptionalElement *const>
            optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetResponseMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableConfigurationUpdateRequest : ReadableCapwapRequest {
  protected:
    std::unordered_map<ElementHeader::ElementType,
                       IReadableConfigurationUpdateRequestOptionalElement *const>
        key_optional_elements;

    std::unordered_map<ElementHeader::ElementType,
                       IReadableConfigurationUpdateRequestOptionalElement *const>
    MapOptionalsElements(
        nonstd::span<IReadableConfigurationUpdateRequestOptionalElement *const> optional_elements);

  public:
    size_t unknown_elements;

    ReadableConfigurationUpdateRequest(const ReadableConfigurationUpdateRequest &) = delete;
    ReadableConfigurationUpdateRequest(nonstd::span<IReadableConfigurationUpdateRequestOptionalElement *const> optional_elements);

    ReadableConfigurationUpdateRequest(
        std::initializer_list<IReadableConfigurationUpdateRequestOptionalElement *> optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;
};
