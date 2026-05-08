

#include "WTPEventRequest.h"
#include "Logging.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"

WritableWTPEventRequest::WritableWTPEventRequest()
    : WritableWTPEventRequest(nonstd::span<IWritableWTPEventRequestOptionalElement *const>{}) {
}

WritableWTPEventRequest::WritableWTPEventRequest(
    nonstd::span<IWritableWTPEventRequestOptionalElement *const> optional_elements)
    : optional_elements{ optional_elements } {
}

ControlHeader::MessageType WritableWTPEventRequest::GetMessageType() const {
    return ControlHeader::WTPEventRequest;
}

ControlHeader::MessageType WritableWTPEventRequest::GetResponseMessageType() const {
    return ControlHeader::WTPEventResponse;
}

void WritableWTPEventRequest::Serialize(RawData *raw_data) const {
    for (auto *elem : optional_elements) {
        ASSERT(elem != nullptr);
        elem->Serialize(raw_data);
    }
}

ReadableWTPEventRequest::ReadableWTPEventRequest()
    : ReadableWTPEventRequest(nonstd::span<IReadableWTPEventRequestOptionalElement *const>{}) {
}

ReadableWTPEventRequest::ReadableWTPEventRequest(
    nonstd::span<IReadableWTPEventRequestOptionalElement *const> optional_elements)
    : key_optional_elements{ MapOptionalsElements(optional_elements) }, unknown_elements{} {
}

ControlHeader::MessageType ReadableWTPEventRequest::GetMessageType() const {
    return ControlHeader::WTPEventRequest;
}

bool ReadableWTPEventRequest::Deserialize(RawData *raw_data) {
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
                log_w("ReadableWTPEventRequest::Deserialize unhandled element type: "
                      "0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }
    return true;
}

void ReadableWTPEventRequest::Log() const {
    log_i("----------------------------------");
    log_i("ME WTPEventRequest:");

    for (const auto &[_, value] : key_optional_elements) {
        (void)_;
        value->Log();
    }

    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %zu", unknown_elements);
    }
    log_i("----------------------------------");
}

std::unordered_map<ElementHeader::ElementType, IReadableWTPEventRequestOptionalElement *const>
ReadableWTPEventRequest::MapOptionalsElements(
    nonstd::span<IReadableWTPEventRequestOptionalElement *const> optional_elements) {

    std::unordered_map<ElementHeader::ElementType, IReadableWTPEventRequestOptionalElement *const>
        map;

    for (auto *elem : optional_elements) {
        ASSERT(elem != nullptr);
        map.emplace(elem->GetElementType(), elem);
    }

    return map;
}