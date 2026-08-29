

#include "StationConfigurationResponse.h"
#include "Logging.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"

WritableStationConfigurationResponse::WritableStationConfigurationResponse(
    const ResultCode::Type result_code)
    : WritableStationConfigurationResponse(
          result_code,
          nonstd::span<IWritableStationConfigurationResponseOptionalElement *const>{}) {
}

WritableStationConfigurationResponse::WritableStationConfigurationResponse(
    const ResultCode::Type result_code,
    nonstd::span<IWritableStationConfigurationResponseOptionalElement *const> optional_elements)
    : result_code{ result_code }, optional_elements{ optional_elements } {
}

ControlHeader::MessageType WritableStationConfigurationResponse::GetMessageType() const {
    return ControlHeader::StationConfigurationResponse;
}

ControlHeader::MessageType WritableStationConfigurationResponse::GetRequestMessageType() const {
    return ControlHeader::StationConfigurationRequest;
}

void WritableStationConfigurationResponse::Serialize(RawData *raw_data) const {
    result_code.Serialize(raw_data);

    for (auto *elem : optional_elements) {
        ASSERT(elem != nullptr);
        elem->Serialize(raw_data);
    }
}

ReadableStationConfigurationResponse::ReadableStationConfigurationResponse()
    : ReadableStationConfigurationResponse(
          nonstd::span<IReadableStationConfigurationResponseOptionalElement *const>{}) {
}

ReadableStationConfigurationResponse::ReadableStationConfigurationResponse(
    nonstd::span<IReadableStationConfigurationResponseOptionalElement *const> optional_elements)
    : key_optional_elements{ MapOptionalsElements(optional_elements) }, unknown_elements{} {
}

ControlHeader::MessageType ReadableStationConfigurationResponse::GetMessageType() const {
    return ControlHeader::StationConfigurationResponse;
}

bool ReadableStationConfigurationResponse::Deserialize(RawData *raw_data) {
    while (raw_data->current + sizeof(ElementHeader) <= raw_data->end) {
        ElementHeader *element = (ElementHeader *)raw_data->current;

        switch (element->GetElementType()) {
            case ElementHeader::ElementType::ResultCode:
                if (!result_code.Deserialize(raw_data)) {
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
                log_w("ReadableStationConfigurationResponse::Deserialize unhandled element type: "
                      "0x{:04X}",
                      (unsigned)unknownElement->GetElementType());

                break;
            }
        }
    }
    return result_code.IsPresent();
}

void ReadableStationConfigurationResponse::Log() const {
    log_i("----------------------------------");
    log_i("ME StationConfigurationResponse:");

    result_code.Log();

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
                   IReadableStationConfigurationResponseOptionalElement *const>
ReadableStationConfigurationResponse::MapOptionalsElements(
    nonstd::span<IReadableStationConfigurationResponseOptionalElement *const> optional_elements) {

    std::unordered_map<ElementHeader::ElementType,
                       IReadableStationConfigurationResponseOptionalElement *const>
        map;

    for (auto *elem : optional_elements) {
        ASSERT(elem != nullptr);
        map.emplace(elem->GetElementType(), elem);
    }

    return map;
}
