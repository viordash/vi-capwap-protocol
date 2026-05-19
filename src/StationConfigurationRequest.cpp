

#include "StationConfigurationRequest.h"
#include "Logging.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"

WritableStationConfigurationRequest::WritableStationConfigurationRequest(
    WritableAddStationArray *add_station)
    : WritableStationConfigurationRequest(
          add_station,
          nonstd::span<IWritableStationConfigurationRequestOptionalElement *const>{}) {
}

WritableStationConfigurationRequest::WritableStationConfigurationRequest(
    WritableDeleteStationArray *delete_station)
    : WritableStationConfigurationRequest(
          delete_station,
          nonstd::span<IWritableStationConfigurationRequestOptionalElement *const>{}) {
}

WritableStationConfigurationRequest::WritableStationConfigurationRequest(
    WritableAddStationArray *add_station,
    nonstd::span<IWritableStationConfigurationRequestOptionalElement *const> optional_elements)
    : add_station{ add_station }, delete_station{ nullptr },
      optional_elements{ optional_elements } {
}

WritableStationConfigurationRequest::WritableStationConfigurationRequest(
    WritableDeleteStationArray *delete_station,
    nonstd::span<IWritableStationConfigurationRequestOptionalElement *const> optional_elements)
    : add_station{ nullptr }, delete_station{ delete_station },
      optional_elements{ optional_elements } {
}

ControlHeader::MessageType WritableStationConfigurationRequest::GetMessageType() const {
    return ControlHeader::StationConfigurationRequest;
}

ControlHeader::MessageType WritableStationConfigurationRequest::GetResponseMessageType() const {
    return ControlHeader::StationConfigurationResponse;
}

void WritableStationConfigurationRequest::Serialize(RawData *raw_data) const {
    ASSERT((add_station != nullptr && delete_station == nullptr)
           || (add_station == nullptr && delete_station != nullptr));

    if (add_station != nullptr) {
        add_station->Serialize(raw_data);
    }
    if (delete_station != nullptr) {
        delete_station->Serialize(raw_data);
    }

    for (auto *elem : optional_elements) {
        ASSERT(elem != nullptr);
        elem->Serialize(raw_data);
    }
}

ReadableStationConfigurationRequest::ReadableStationConfigurationRequest()
    : ReadableStationConfigurationRequest(
          nonstd::span<IReadableStationConfigurationRequestOptionalElement *const>{}) {
}

ReadableStationConfigurationRequest::ReadableStationConfigurationRequest(
    nonstd::span<IReadableStationConfigurationRequestOptionalElement *const> optional_elements)
    : key_optional_elements{ MapOptionalsElements(optional_elements) }, unknown_elements{} {
}

ControlHeader::MessageType ReadableStationConfigurationRequest::GetMessageType() const {
    return ControlHeader::StationConfigurationRequest;
}

bool ReadableStationConfigurationRequest::Deserialize(RawData *raw_data) {
    while (raw_data->current + sizeof(ElementHeader) <= raw_data->end) {
        ElementHeader *element = (ElementHeader *)raw_data->current;

        switch (element->GetElementType()) {
            case ElementHeader::ElementType::AddStation:
                if (!add_station.Deserialize(raw_data)) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::DeleteStation:
                if (!delete_station.Deserialize(raw_data)) {
                    return false;
                }
                break;

            default: {
                auto it = key_optional_elements.find(element->GetElementType());
                if (it != key_optional_elements.end()) {
                    if (!it->second->Deserialize(raw_data)) {
                        return false;
                    }
                    break;
                }

                auto unknownElement = UnrecognizedElement::Deserialize(raw_data);
                if (unknownElement == nullptr) {
                    return false;
                }
                unknown_elements++;
                log_w("ReadableStationConfigurationRequest::Deserialize unhandled element type: "
                      "0x{:04X}",
                      (unsigned)unknownElement->GetElementType());

                break;
            }
        }
    }
    if (add_station.IsPresent() && !delete_station.IsPresent()) {
        return true;
    }
    if (!add_station.IsPresent() && delete_station.IsPresent()) {
        return true;
    }
    return false;
}

void ReadableStationConfigurationRequest::Log() const {
    log_i("----------------------------------");
    log_i("ME StationConfigurationRequest:");

    if (add_station.IsPresent()) {
        add_station.Log();
    }

    if (delete_station.IsPresent()) {
        delete_station.Log();
    }

    for (const auto &[type, value] : key_optional_elements) {
        if (value->IsPresent()) {
            value->Log();
        } else {
            log_i("  expected optional element is missing, type: 0x{:04X}", (unsigned)type);
        }
    }

    if (unknown_elements > 0) {
        log_i("  UnknownElements count: {}", unknown_elements);
    }
    log_i("----------------------------------");
}

std::unordered_map<ElementHeader::ElementType,
                   IReadableStationConfigurationRequestOptionalElement *const>
ReadableStationConfigurationRequest::MapOptionalsElements(
    nonstd::span<IReadableStationConfigurationRequestOptionalElement *const> optional_elements) {

    std::unordered_map<ElementHeader::ElementType,
                       IReadableStationConfigurationRequestOptionalElement *const>
        map;

    for (auto *elem : optional_elements) {
        ASSERT(elem != nullptr);
        map.emplace(elem->GetElementType(), elem);
    }

    return map;
}
