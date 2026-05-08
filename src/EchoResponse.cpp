

#include "EchoResponse.h"
#include "Logging.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"

WritableEchoResponse::WritableEchoResponse()
    : WritableEchoResponse(nonstd::span<IWritableEchoResponseOptionalElement *const>{}) {
}

WritableEchoResponse::WritableEchoResponse(
    nonstd::span<IWritableEchoResponseOptionalElement *const> optional_elements)
    : optional_elements{ optional_elements } {
}

ControlHeader::MessageType WritableEchoResponse::GetMessageType() const {
    return ControlHeader::EchoResponse;
}

ControlHeader::MessageType WritableEchoResponse::GetRequestMessageType() const {
    return ControlHeader::EchoRequest;
}

void WritableEchoResponse::Serialize(RawData *raw_data) const {
    for (auto *elem : optional_elements) {
        ASSERT(elem != nullptr);
        elem->Serialize(raw_data);
    }
}

ReadableEchoResponse::ReadableEchoResponse()
    : ReadableEchoResponse(nonstd::span<IReadableEchoResponseOptionalElement *const>{}) {
}

ReadableEchoResponse::ReadableEchoResponse(
    nonstd::span<IReadableEchoResponseOptionalElement *const> optional_elements)
    : key_optional_elements{ MapOptionalsElements(optional_elements) }, unknown_elements{} {
}

ControlHeader::MessageType ReadableEchoResponse::GetMessageType() const {
    return ControlHeader::EchoResponse;
}

bool ReadableEchoResponse::Deserialize(RawData *raw_data) {
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
                log_w("ReadableEchoResponse::Deserialize unhandled element type: "
                      "0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }
    return true;
}

void ReadableEchoResponse::Log() const {
    log_i("----------------------------------");
    log_i("ME EchoResponse:");

    for (const auto &[_, value] : key_optional_elements) {
        (void)_;
        value->Log();
    }
    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %zu", unknown_elements);
    }
    log_i("----------------------------------");
}

std::unordered_map<ElementHeader::ElementType, IReadableEchoResponseOptionalElement *const>
ReadableEchoResponse::MapOptionalsElements(
    nonstd::span<IReadableEchoResponseOptionalElement *const> optional_elements) {

    std::unordered_map<ElementHeader::ElementType, IReadableEchoResponseOptionalElement *const> map;

    for (auto *elem : optional_elements) {
        ASSERT(elem != nullptr);
        map.emplace(elem->GetElementType(), elem);
    }

    return map;
}