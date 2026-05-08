

#include "ResetRequest.h"
#include "Logging.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"

WritableResetRequest::WritableResetRequest(WritableImageIdentifier &image_identifier)
    : WritableResetRequest(image_identifier,
                           nonstd::span<IWritableResetRequestOptionalElement *const>{}) {
}

WritableResetRequest::WritableResetRequest(
    WritableImageIdentifier &image_identifier,
    nonstd::span<IWritableResetRequestOptionalElement *const> optional_elements)
    : image_identifier{ image_identifier }, optional_elements{ optional_elements } {
}

ControlHeader::MessageType WritableResetRequest::GetMessageType() const {
    return ControlHeader::ResetRequest;
}

ControlHeader::MessageType WritableResetRequest::GetResponseMessageType() const {
    return ControlHeader::ResetResponse;
}

void WritableResetRequest::Serialize(RawData *raw_data) const {
    image_identifier.Serialize(raw_data);

    for (auto *elem : optional_elements) {
        ASSERT(elem != nullptr);
        elem->Serialize(raw_data);
    }
}

ReadableResetRequest::ReadableResetRequest()
    : ReadableResetRequest(nonstd::span<IReadableResetRequestOptionalElement *const>{}) {
}

ReadableResetRequest::ReadableResetRequest(
    nonstd::span<IReadableResetRequestOptionalElement *const> optional_elements)
    : key_optional_elements{ MapOptionalsElements(optional_elements) }, unknown_elements{} {
}

ControlHeader::MessageType ReadableResetRequest::GetMessageType() const {
    return ControlHeader::ResetRequest;
}

bool ReadableResetRequest::Deserialize(RawData *raw_data) {
    while (raw_data->current + sizeof(ElementHeader) <= raw_data->end) {
        ElementHeader *element = (ElementHeader *)raw_data->current;

        switch (element->GetElementType()) {
            case ElementHeader::ElementType::ImageIdentifier:
                if (!image_identifier.Deserialize(raw_data)) {
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
                log_w("ReadableResetRequest::Deserialize unhandled element type: "
                      "0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }
    return image_identifier.IsPresent();
}

void ReadableResetRequest::Log() const {
    log_i("----------------------------------");
    log_i("ME ResetRequest:");

    image_identifier.Log();

    for (const auto &[_, value] : key_optional_elements) {
        (void)_;
        value->Log();
    }

    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %zu", unknown_elements);
    }
    log_i("----------------------------------");
}

std::unordered_map<ElementHeader::ElementType, IReadableResetRequestOptionalElement *const>
ReadableResetRequest::MapOptionalsElements(
    nonstd::span<IReadableResetRequestOptionalElement *const> optional_elements) {

    std::unordered_map<ElementHeader::ElementType, IReadableResetRequestOptionalElement *const> map;

    for (auto *elem : optional_elements) {
        ASSERT(elem != nullptr);
        map.emplace(elem->GetElementType(), elem);
    }

    return map;
}