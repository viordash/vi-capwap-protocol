

#include "WlanConfigurationRequest.h"
#include "Logging.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"

WritableWlanConfigurationRequest::WritableWlanConfigurationRequest(
    WritableUpdateWlanArray *update_wlan)
    : WritableWlanConfigurationRequest(
          update_wlan,
          nonstd::span<IWritableWlanConfigurationRequestOptionalElement *const>{}) {
}

WritableWlanConfigurationRequest::WritableWlanConfigurationRequest(
    WritableDeleteWlanArray *delete_wlan)
    : WritableWlanConfigurationRequest(
          delete_wlan,
          nonstd::span<IWritableWlanConfigurationRequestOptionalElement *const>{}) {
}

WritableWlanConfigurationRequest::WritableWlanConfigurationRequest(WritableAddWlanArray *add_wlan)
    : WritableWlanConfigurationRequest(
          add_wlan,
          nonstd::span<IWritableWlanConfigurationRequestOptionalElement *const>{}) {
}

WritableWlanConfigurationRequest::WritableWlanConfigurationRequest(
    WritableAddWlanArray *add_wlan,
    nonstd::span<IWritableWlanConfigurationRequestOptionalElement *const> optional_elements)
    : add_wlan{ add_wlan }, delete_wlan{ nullptr }, update_wlan{ nullptr },
      optional_elements{ optional_elements } {
}

WritableWlanConfigurationRequest::WritableWlanConfigurationRequest(
    WritableDeleteWlanArray *delete_wlan,
    nonstd::span<IWritableWlanConfigurationRequestOptionalElement *const> optional_elements)
    : add_wlan{ nullptr }, delete_wlan{ delete_wlan }, update_wlan{ nullptr },
      optional_elements{ optional_elements } {
}

WritableWlanConfigurationRequest::WritableWlanConfigurationRequest(
    WritableUpdateWlanArray *update_wlan,
    nonstd::span<IWritableWlanConfigurationRequestOptionalElement *const> optional_elements)
    : add_wlan{ nullptr }, delete_wlan{ nullptr }, update_wlan{ update_wlan },
      optional_elements{ optional_elements } {
}

ControlHeader::MessageType WritableWlanConfigurationRequest::GetMessageType() const {
    return ControlHeader::WlanConfigurationRequest;
}

ControlHeader::MessageType WritableWlanConfigurationRequest::GetResponseMessageType() const {
    return ControlHeader::WlanConfigurationResponse;
}

void WritableWlanConfigurationRequest::Serialize(RawData *raw_data) const {
    if (add_wlan != nullptr) {
        add_wlan->Serialize(raw_data);
    }
    if (delete_wlan != nullptr) {
        delete_wlan->Serialize(raw_data);
    }
    if (update_wlan != nullptr) {
        update_wlan->Serialize(raw_data);
    }

    for (auto *elem : optional_elements) {
        ASSERT(elem != nullptr);
        elem->Serialize(raw_data);
    }
}

ReadableWlanConfigurationRequest::ReadableWlanConfigurationRequest()
    : ReadableWlanConfigurationRequest(
          nonstd::span<IReadableWlanConfigurationRequestOptionalElement *const>{}) {
}

ReadableWlanConfigurationRequest::ReadableWlanConfigurationRequest(
    nonstd::span<IReadableWlanConfigurationRequestOptionalElement *const> optional_elements)
    : key_optional_elements{ MapOptionalsElements(optional_elements) }, unknown_elements{} {
}

ControlHeader::MessageType ReadableWlanConfigurationRequest::GetMessageType() const {
    return ControlHeader::WlanConfigurationRequest;
}

bool ReadableWlanConfigurationRequest::Deserialize(RawData *raw_data) {
    while (raw_data->current + sizeof(ElementHeader) <= raw_data->end) {
        ElementHeader *element = (ElementHeader *)raw_data->current;

        switch (element->GetElementType()) {
            case ElementHeader::ElementType::AddWlan:
                if (!add_wlan.Deserialize(raw_data)) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::DeleteWlan:
                if (!delete_wlan.Deserialize(raw_data)) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::UpdateWlan:
                if (!update_wlan.Deserialize(raw_data)) {
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
                log_w("ReadableWlanConfigurationRequest::Deserialize unhandled element type: "
                      "0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }
    return add_wlan.IsPresent() || delete_wlan.IsPresent() || update_wlan.IsPresent();
}

void ReadableWlanConfigurationRequest::Log() const {
    log_i("----------------------------------");
    log_i("ME WlanConfigurationRequest:");

    if (add_wlan.IsPresent()) {
        add_wlan.Log();
    }

    if (delete_wlan.IsPresent()) {
        delete_wlan.Log();
    }

    if (update_wlan.IsPresent()) {
        update_wlan.Log();
    }

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

std::unordered_map<ElementHeader::ElementType,
                   IReadableWlanConfigurationRequestOptionalElement *const>
ReadableWlanConfigurationRequest::MapOptionalsElements(
    nonstd::span<IReadableWlanConfigurationRequestOptionalElement *const> optional_elements) {

    std::unordered_map<ElementHeader::ElementType,
                       IReadableWlanConfigurationRequestOptionalElement *const>
        map;

    for (auto *elem : optional_elements) {
        ASSERT(elem != nullptr);
        map.emplace(elem->GetElementType(), elem);
    }

    return map;
}
