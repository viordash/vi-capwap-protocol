

#include "ChangeStateEventResponse.h"
#include "Logging.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"

WritableChangeStateEventResponse::WritableChangeStateEventResponse(
    nonstd::span<IWritableChangeStateEventResponseOptionalElement *const> optional_elements)
    : optional_elements{ optional_elements } {
}

WritableChangeStateEventResponse::WritableChangeStateEventResponse(
    std::initializer_list<IWritableChangeStateEventResponseOptionalElement *const>
        optional_elements)
    : WritableChangeStateEventResponse(
          nonstd::span<IWritableChangeStateEventResponseOptionalElement *const>(
              optional_elements.begin(),
              optional_elements.size())) {
}

ControlHeader::MessageType WritableChangeStateEventResponse::GetMessageType() const {
    return ControlHeader::ChangeStateEventResponse;
}

ControlHeader::MessageType WritableChangeStateEventResponse::GetRequestMessageType() const {
    return ControlHeader::ChangeStateEventRequest;
}

void WritableChangeStateEventResponse::Serialize(RawData *raw_data) const {
    for (auto *elem : optional_elements) {
        ASSERT(elem != nullptr);
        elem->Serialize(raw_data);
    }
}

ReadableChangeStateEventResponse::ReadableChangeStateEventResponse(
    nonstd::span<IReadableChangeStateEventResponseOptionalElement *const> optional_elements)
    : key_optional_elements{ MapOptionalsElements(optional_elements) }, unknown_elements{} {
}

ReadableChangeStateEventResponse::ReadableChangeStateEventResponse(
    std::initializer_list<IReadableChangeStateEventResponseOptionalElement *> optional_elements)
    : ReadableChangeStateEventResponse(
          nonstd::span<IReadableChangeStateEventResponseOptionalElement *const>(
              optional_elements.begin(),
              optional_elements.size())) {
}

ControlHeader::MessageType ReadableChangeStateEventResponse::GetMessageType() const {
    return ControlHeader::ChangeStateEventResponse;
}

bool ReadableChangeStateEventResponse::Deserialize(RawData *raw_data) {
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
                log_w("ReadableChangeStateEventResponse::Deserialize unhandled element type: "
                      "0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }
    return true;
}

void ReadableChangeStateEventResponse::Log() const {
    log_i("----------------------------------");
    log_i("ME ChangeStateEventResponse:");

    for (const auto &[_, value] : key_optional_elements) {
        value->Log();
    }

    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %zu", unknown_elements);
    }
    log_i("----------------------------------");
}

std::unordered_map<ElementHeader::ElementType,
                   IReadableChangeStateEventResponseOptionalElement *const>
ReadableChangeStateEventResponse::MapOptionalsElements(
    nonstd::span<IReadableChangeStateEventResponseOptionalElement *const> optional_elements) {

    std::unordered_map<ElementHeader::ElementType,
                       IReadableChangeStateEventResponseOptionalElement *const>
        map;

    for (auto *elem : optional_elements) {
        ASSERT(elem != nullptr);
        map.emplace(elem->GetElementType(), elem);
    }

    return map;
}