#include "MTUDiscoveryPadding.h"
#include "Logging.h"
#include "lassert.h"
#include <cstring>
#include <string.h>

MTUDiscoveryPadding::MTUDiscoveryPadding(uint16_t size)
    : ElementHeader(ElementHeader::MTUDiscoveryPadding, size) {
}

bool MTUDiscoveryPadding::Validate() const {
    static_assert(sizeof(MTUDiscoveryPadding) == 4);
    return ElementHeader::GetElementType() == ElementHeader::MTUDiscoveryPadding;
}

WritableMTUDiscoveryPadding::WritableMTUDiscoveryPadding(uint16_t size) : element{ size } {
    static_assert(sizeof(element) == 4);
}

void WritableMTUDiscoveryPadding::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(ElementHeader) + element.GetLength() <= raw_data->end);
    std::memcpy(raw_data->current, &element, sizeof(element));
    raw_data->current += sizeof(element);

    std::memset(raw_data->current, 0xFF, element.GetLength());
    raw_data->current += element.GetLength();
}

uint16_t WritableMTUDiscoveryPadding::GetLength() const {
    return element.GetLength();
}

uint16_t WritableMTUDiscoveryPadding::GetTotalLength() const {
    return element.GetLength() + sizeof(ElementHeader);
}

void WritableMTUDiscoveryPadding::Log() const {
    log_i("ME MTUDiscoveryPadding Padding:{}", element.GetLength());
}

bool ReadableMTUDiscoveryPadding::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ElementHeader) > raw_data->end) {
        return false;
    }

    if (raw_data->current + sizeof(ReadableMTUDiscoveryPadding::Element) > raw_data->end) {
        return false;
    }

    auto item = (ReadableMTUDiscoveryPadding::Element *)raw_data->current;
    if (!item->Validate()) {
        return false;
    }

    uint8_t *last = raw_data->current + sizeof(ElementHeader) + item->GetLength();
    if (last > raw_data->end) {
        return false;
    }

#ifdef VALIDATE_MTUDiscoveryPadding
    for (size_t i = 0; i < res->GetLength(); i++) {
        if (res->padding[i] != 0xFF) {
            return false;
        }
    }
#endif // VALIDATE_MTUDiscoveryPadding

    raw_data->current = last;
    element = item;
    is_present = true;
    return true;
}

const ReadableMTUDiscoveryPadding::Element *ReadableMTUDiscoveryPadding::Get() const {
    return element;
}

void ReadableMTUDiscoveryPadding::Log() const {
    ASSERT(element != nullptr);
    log_i("ME MTUDiscoveryPadding Padding:{}", element->GetLength());
}

ElementHeader::ElementType ReadableMTUDiscoveryPadding::GetElementType() const {
    return ElementHeader::MTUDiscoveryPadding;
}

bool ReadableMTUDiscoveryPadding::IsPresent() const {
    return is_present;
}
