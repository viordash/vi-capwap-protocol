

#include "ConfigurationUpdateRequest.h"
#include "Logging.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"

WritableConfigurationUpdateRequest::WritableConfigurationUpdateRequest(
    nonstd::span<IWritableConfigurationUpdateRequestOptionalElement *const> optional_elements)
    : optional_elements{ optional_elements } {
}


ControlHeader::MessageType WritableConfigurationUpdateRequest::GetMessageType() const {
    return ControlHeader::ConfigurationUpdateRequest;
}

ControlHeader::MessageType WritableConfigurationUpdateRequest::GetResponseMessageType() const {
    return ControlHeader::ConfigurationUpdateResponse;
}

void WritableConfigurationUpdateRequest::Serialize(RawData *raw_data) const {
    for (auto *elem : optional_elements) {
        ASSERT(elem != nullptr);
        elem->Serialize(raw_data);
    }
}

ReadableConfigurationUpdateRequest::ReadableConfigurationUpdateRequest(
    nonstd::span<IReadableConfigurationUpdateRequestOptionalElement *const> optional_elements)
    : key_optional_elements{ MapOptionalsElements(optional_elements) }, unknown_elements{} {
}


ControlHeader::MessageType ReadableConfigurationUpdateRequest::GetMessageType() const {
    return ControlHeader::ConfigurationUpdateRequest;
}

bool ReadableConfigurationUpdateRequest::Deserialize(RawData *raw_data) {
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
                log_w("ReadableConfigurationUpdateRequest::Deserialize unhandled element type: "
                      "0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }
    return true;
}

void ReadableConfigurationUpdateRequest::Log() const {
    log_i("----------------------------------");
    log_i("ME ConfigurationUpdateRequest:");

    for (const auto &[_, value] : key_optional_elements) {
        value->Log();
    }

    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %zu", unknown_elements);
    }
    log_i("----------------------------------");
}

std::unordered_map<ElementHeader::ElementType,
                   IReadableConfigurationUpdateRequestOptionalElement *const>
ReadableConfigurationUpdateRequest::MapOptionalsElements(
    nonstd::span<IReadableConfigurationUpdateRequestOptionalElement *const> optional_elements) {

    std::unordered_map<ElementHeader::ElementType,
                       IReadableConfigurationUpdateRequestOptionalElement *const>
        map;

    for (auto *elem : optional_elements) {
        ASSERT(elem != nullptr);
        map.emplace(elem->GetElementType(), elem);
    }

    return map;
}