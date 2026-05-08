

#include "ImageDataResponse.h"
#include "Logging.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"

WritableImageDataResponse::WritableImageDataResponse(const ResultCode::Type result_code)
    : WritableImageDataResponse(result_code,
                                nonstd::span<IWritableImageDataResponseOptionalElement *const>{}) {
}

WritableImageDataResponse::WritableImageDataResponse(
    const ResultCode::Type result_code,
    nonstd::span<IWritableImageDataResponseOptionalElement *const> optional_elements)
    : result_code{ result_code }, optional_elements{ optional_elements } {
}

ControlHeader::MessageType WritableImageDataResponse::GetMessageType() const {
    return ControlHeader::ImageDataResponse;
}

ControlHeader::MessageType WritableImageDataResponse::GetRequestMessageType() const {
    return ControlHeader::ImageDataRequest;
}

void WritableImageDataResponse::Serialize(RawData *raw_data) const {
    result_code.Serialize(raw_data);

    for (auto *elem : optional_elements) {
        ASSERT(elem != nullptr);
        elem->Serialize(raw_data);
    }
}

ReadableImageDataResponse::ReadableImageDataResponse()
    : ReadableImageDataResponse(nonstd::span<IReadableImageDataResponseOptionalElement *const>{}) {
}

ReadableImageDataResponse::ReadableImageDataResponse(
    nonstd::span<IReadableImageDataResponseOptionalElement *const> optional_elements)
    : key_optional_elements{ MapOptionalsElements(optional_elements) }, unknown_elements{} {
}

ControlHeader::MessageType ReadableImageDataResponse::GetMessageType() const {
    return ControlHeader::ImageDataResponse;
}

bool ReadableImageDataResponse::Deserialize(RawData *raw_data) {
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
                log_w("ReadableImageDataResponse::Deserialize unhandled element type: 0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }
    return result_code.IsPresent();
}

void ReadableImageDataResponse::Log() const {
    log_i("----------------------------------");
    log_i("ME ImageDataResponse:");

    result_code.Log();

    for (const auto &[type, value] : key_optional_elements) {
        if (value->IsPresent()) {
            value->Log();
        } else {
            log_i("  expected optional element is missing, type: 0x%04X", (unsigned)type);
        }
    }
    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %zu", unknown_elements);
    }
    log_i("----------------------------------");
}

std::unordered_map<ElementHeader::ElementType, IReadableImageDataResponseOptionalElement *const>
ReadableImageDataResponse::MapOptionalsElements(
    nonstd::span<IReadableImageDataResponseOptionalElement *const> optional_elements) {

    std::unordered_map<ElementHeader::ElementType, IReadableImageDataResponseOptionalElement *const>
        map;

    for (auto *elem : optional_elements) {
        ASSERT(elem != nullptr);
        map.emplace(elem->GetElementType(), elem);
    }

    return map;
}