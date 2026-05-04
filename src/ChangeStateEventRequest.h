#pragma once

#include "CapwapMessage.h"
#include "IElement.h"
#include "elements/IEEE80211/WTPRadioFailAlarmIndication.h"
#include "elements/RadioOperationalState.h"
#include "elements/ResultCode.h"
#include "elements/ReturnedMessageElement.h"
#include "elements/VendorSpecificPayload.h"
#include "span.hpp"
#include <limits>
#include <unordered_map>

struct WritableChangeStateEventRequest : WritableCapwapRequest {

  private:
    WritableRadioOperationalStateArray &radio_operational_states;
    ResultCode &result_code;

    nonstd::span<IWritableChangeStateEventRequestOptionalElement *const> optional_elements;

  public:
    static const uint16_t no_probe_mtu_size = std::numeric_limits<uint16_t>::min();

    WritableChangeStateEventRequest(const WritableChangeStateEventRequest &) = delete;
    WritableChangeStateEventRequest(
        WritableRadioOperationalStateArray &radio_operational_states,
        ResultCode &result_code,
        nonstd::span<IWritableChangeStateEventRequestOptionalElement *const> optional_elements);
    WritableChangeStateEventRequest(
        WritableRadioOperationalStateArray &radio_operational_states,
        ResultCode &result_code,
        std::initializer_list<IWritableChangeStateEventRequestOptionalElement *const>
            optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetResponseMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
    ResultCode::Type GetResultCode();
    void ValidateReturnedMessageElement();
};

struct ReadableChangeStateEventRequest : ReadableCapwapRequest {
  protected:
    std::unordered_map<ElementHeader::ElementType,
                       IReadableChangeStateEventRequestOptionalElement *const>
        key_optional_elements;

    std::unordered_map<ElementHeader::ElementType,
                       IReadableChangeStateEventRequestOptionalElement *const>
    MapOptionalsElements(
        nonstd::span<IReadableChangeStateEventRequestOptionalElement *const> optional_elements);

  public:
    ReadableRadioOperationalStateArray radio_operational_states;
    ReadableResultCode result_code;

    size_t unknown_elements;

    ReadableChangeStateEventRequest(const ReadableChangeStateEventRequest &) = delete;
    ReadableChangeStateEventRequest(
        nonstd::span<IReadableChangeStateEventRequestOptionalElement *const> optional_elements);
    ReadableChangeStateEventRequest(
        std::initializer_list<IReadableChangeStateEventRequestOptionalElement *> optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;
};
