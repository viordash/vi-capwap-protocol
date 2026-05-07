

#include "WlanConfigurationResponse.h"
#include "Logging.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"

WritableWlanConfigurationResponse::WritableWlanConfigurationResponse(
    const ResultCode::Type result_code,
    nonstd::span<IWritableWlanConfigurationResponseOptionalElement *const> optional_elements)
    : result_code{ result_code }, optional_elements{ optional_elements } {
}

WritableWlanConfigurationResponse::WritableWlanConfigurationResponse(
    const ResultCode::Type result_code,
    std::initializer_list<IWritableWlanConfigurationResponseOptionalElement *> optional_elements)
    : WritableWlanConfigurationResponse(
          result_code,
          nonstd::span<IWritableWlanConfigurationResponseOptionalElement *const>(
              optional_elements.begin(),
              optional_elements.size())) {
}

ControlHeader::MessageType WritableWlanConfigurationResponse::GetMessageType() const {
    return ControlHeader::WlanConfigurationResponse;
}

ControlHeader::MessageType WritableWlanConfigurationResponse::GetRequestMessageType() const {
    return ControlHeader::WlanConfigurationRequest;
}

void WritableWlanConfigurationResponse::Serialize(RawData *raw_data) const {
    result_code.Serialize(raw_data);

    for (auto *elem : optional_elements) {
        ASSERT(elem != nullptr);
        elem->Serialize(raw_data);
    }
}

ReadableWlanConfigurationResponse::ReadableWlanConfigurationResponse(
    nonstd::span<IReadableWlanConfigurationResponseOptionalElement *const> optional_elements)
    : key_optional_elements{ MapOptionalsElements(optional_elements) }, unknown_elements{} {
}

ReadableWlanConfigurationResponse::ReadableWlanConfigurationResponse(
    std::initializer_list<IReadableWlanConfigurationResponseOptionalElement *> optional_elements)
    : ReadableWlanConfigurationResponse(
          nonstd::span<IReadableWlanConfigurationResponseOptionalElement *const>(
              optional_elements.begin(),
              optional_elements.size())) {
}

ControlHeader::MessageType ReadableWlanConfigurationResponse::GetMessageType() const {
    return ControlHeader::WlanConfigurationResponse;
}

bool ReadableWlanConfigurationResponse::Deserialize(RawData *raw_data) {
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
                log_w("ReadableWlanConfigurationResponse::Deserialize unhandled element type: "
                      "0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }
    return result_code.IsPresent();
}

void ReadableWlanConfigurationResponse::Log() const {
    log_i("----------------------------------");
    log_i("ME WlanConfigurationResponse:");

    result_code.Log();

    for (const auto &[_, value] : key_optional_elements) {
        value->Log();
    }

    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %zu", unknown_elements);
    }
    log_i("----------------------------------");
}

std::unordered_map<ElementHeader::ElementType,
                   IReadableWlanConfigurationResponseOptionalElement *const>
ReadableWlanConfigurationResponse::MapOptionalsElements(
    nonstd::span<IReadableWlanConfigurationResponseOptionalElement *const> optional_elements) {

    std::unordered_map<ElementHeader::ElementType,
                       IReadableWlanConfigurationResponseOptionalElement *const>
        map;

    for (auto *elem : optional_elements) {
        ASSERT(elem != nullptr);
        map.emplace(elem->GetElementType(), elem);
    }

    return map;
}
