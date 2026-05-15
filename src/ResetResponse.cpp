

#include "ResetResponse.h"
#include "Logging.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"

WritableResetResponse::WritableResetResponse()
    : WritableResetResponse(nonstd::span<IWritableResetResponseOptionalElement *const>{}) {
}

WritableResetResponse::WritableResetResponse(
    nonstd::span<IWritableResetResponseOptionalElement *const> optional_elements)
    : optional_elements{ optional_elements } {
}

ControlHeader::MessageType WritableResetResponse::GetMessageType() const {
    return ControlHeader::ResetResponse;
}

ControlHeader::MessageType WritableResetResponse::GetRequestMessageType() const {
    return ControlHeader::ResetRequest;
}

void WritableResetResponse::Serialize(RawData *raw_data) const {
    for (auto *elem : optional_elements) {
        ASSERT(elem != nullptr);
        elem->Serialize(raw_data);
    }
}

ReadableResetResponse::ReadableResetResponse()
    : ReadableResetResponse(nonstd::span<IReadableResetResponseOptionalElement *const>{}) {
}

ReadableResetResponse::ReadableResetResponse(
    nonstd::span<IReadableResetResponseOptionalElement *const> optional_elements)
    : key_optional_elements{ MapOptionalsElements(optional_elements) }, unknown_elements{} {
}

ControlHeader::MessageType ReadableResetResponse::GetMessageType() const {
    return ControlHeader::ResetResponse;
}

bool ReadableResetResponse::Deserialize(RawData *raw_data) {
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
                log_w("ReadableResetResponse::Deserialize unhandled element type: "
                      "0x%04X",
                      (unsigned)unknownElement->GetElementType());

                break;
            }
        }
    }
    return true;
}

void ReadableResetResponse::Log() const {
    log_i("----------------------------------");
    log_i("ME ResetResponse:");

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

std::unordered_map<ElementHeader::ElementType, IReadableResetResponseOptionalElement *const>
ReadableResetResponse::MapOptionalsElements(
    nonstd::span<IReadableResetResponseOptionalElement *const> optional_elements) {

    std::unordered_map<ElementHeader::ElementType, IReadableResetResponseOptionalElement *const>
        map;

    for (auto *elem : optional_elements) {
        ASSERT(elem != nullptr);
        map.emplace(elem->GetElementType(), elem);
    }

    return map;
}