

#include "WTPEventResponse.h"
#include "Logging.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"

WritableWTPEventResponse::WritableWTPEventResponse()
    : WritableWTPEventResponse(nonstd::span<IWritableWTPEventResponseOptionalElement *const>{}) {
}

WritableWTPEventResponse::WritableWTPEventResponse(
    nonstd::span<IWritableWTPEventResponseOptionalElement *const> optional_elements)
    : optional_elements{ optional_elements } {
}

ControlHeader::MessageType WritableWTPEventResponse::GetMessageType() const {
    return ControlHeader::WTPEventResponse;
}

ControlHeader::MessageType WritableWTPEventResponse::GetRequestMessageType() const {
    return ControlHeader::WTPEventRequest;
}

void WritableWTPEventResponse::Serialize(RawData *raw_data) const {
    for (auto *elem : optional_elements) {
        ASSERT(elem != nullptr);
        elem->Serialize(raw_data);
    }
}

ReadableWTPEventResponse::ReadableWTPEventResponse()
    : ReadableWTPEventResponse(nonstd::span<IReadableWTPEventResponseOptionalElement *const>{}) {
}

ReadableWTPEventResponse::ReadableWTPEventResponse(
    nonstd::span<IReadableWTPEventResponseOptionalElement *const> optional_elements)
    : key_optional_elements{ MapOptionalsElements(optional_elements) }, unknown_elements{} {
}

ControlHeader::MessageType ReadableWTPEventResponse::GetMessageType() const {
    return ControlHeader::WTPEventResponse;
}

bool ReadableWTPEventResponse::Deserialize(RawData *raw_data) {
    while (raw_data->current + sizeof(ElementHeader) <= raw_data->end) {
        ElementHeader *element = (ElementHeader *)raw_data->current;

        switch (element->GetElementType()) {
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
                log_w("ReadableWTPEventResponse::Deserialize unhandled element type: "
                      "0x{:04X}",
                      (unsigned)unknownElement->GetElementType());

                break;
            }
        }
    }
    return true;
}

void ReadableWTPEventResponse::Log() const {
    log_i("----------------------------------");
    log_i("ME WTPEventResponse:");

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

std::unordered_map<ElementHeader::ElementType, IReadableWTPEventResponseOptionalElement *const>
ReadableWTPEventResponse::MapOptionalsElements(
    nonstd::span<IReadableWTPEventResponseOptionalElement *const> optional_elements) {

    std::unordered_map<ElementHeader::ElementType, IReadableWTPEventResponseOptionalElement *const>
        map;

    for (auto *elem : optional_elements) {
        ASSERT(elem != nullptr);
        map.emplace(elem->GetElementType(), elem);
    }

    return map;
}