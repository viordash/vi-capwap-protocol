#pragma once

#include "CapwapMessage.h"
#include "IElement.h"
#include "elements/AddStation.h"
#include "elements/DeleteStation.h"
#include "elements/VendorSpecificPayload.h"
#include "span.hpp"
#include <limits>
#include <unordered_map>

struct WritableStationConfigurationRequest : WritableCapwapRequest {
  protected:
    WritableAddStationArray *add_station;
    WritableDeleteStationArray *delete_station;

    nonstd::span<IWritableStationConfigurationRequestOptionalElement *const> optional_elements;

  public:
    WritableStationConfigurationRequest(const WritableStationConfigurationRequest &) = delete;
    WritableStationConfigurationRequest(WritableAddStationArray *add_station);
    WritableStationConfigurationRequest(
        WritableAddStationArray *add_station,
        nonstd::span<IWritableStationConfigurationRequestOptionalElement *const> optional_elements);

    WritableStationConfigurationRequest(WritableDeleteStationArray *delete_station);
    WritableStationConfigurationRequest(
        WritableDeleteStationArray *delete_station,
        nonstd::span<IWritableStationConfigurationRequestOptionalElement *const> optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetResponseMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableStationConfigurationRequest : ReadableCapwapRequest {
  protected:
    std::unordered_map<ElementHeader::ElementType,
                       IReadableStationConfigurationRequestOptionalElement *const>
        key_optional_elements;

    std::unordered_map<ElementHeader::ElementType,
                       IReadableStationConfigurationRequestOptionalElement *const>
    MapOptionalsElements(
        nonstd::span<IReadableStationConfigurationRequestOptionalElement *const> optional_elements);

  public:
    ReadableAddStationArray add_station;
    ReadableDeleteStationArray delete_station;

    size_t unknown_elements;

    ReadableStationConfigurationRequest(const ReadableStationConfigurationRequest &) = delete;
    ReadableStationConfigurationRequest();
    ReadableStationConfigurationRequest(
        nonstd::span<IReadableStationConfigurationRequestOptionalElement *const> optional_elements);

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
