
#include "ACIPv4List.h"
#include "Logging.h"
#include "NetworkUtils.h"
#include "lassert.h"
#include <string.h>

WritableACIPv4List::Element::Element(size_t count)
    : ElementHeader(ElementHeader::ACIPv4List, count * sizeof(uint32_t)) {
}

WritableACIPv4List::WritableACIPv4List(const nonstd::span<const uint32_t> addresses)
    : element{ addresses.size() }, addresses{ addresses } {
    static_assert(sizeof(element) == 4);
    ASSERT(addresses.size() <= ReadableACIPv4List::Element::max_count);
}

void WritableACIPv4List::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(ElementHeader) + element.GetLength() <= raw_data->end);

#pragma GCC diagnostic push
#if __GNUC__ >= 8
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif
    std::memcpy(raw_data->current, &element, sizeof(element));
#pragma GCC diagnostic pop

    raw_data->current += sizeof(ElementHeader);
    std::memcpy(raw_data->current, addresses.data(), addresses.size() * sizeof(uint32_t));
    raw_data->current += addresses.size() * sizeof(uint32_t);
}

void WritableACIPv4List::Log() const {
    log_i("ME ACIPv4List size:%zu, adrs:", addresses.size());
    for (size_t i = 0; i < addresses.size(); i++) {
        log_i("     #%zu, %s", i, IpToString(addresses[i]).c_str());
    }
}

ReadableACIPv4List::Element::Element() : ElementHeader(ElementHeader::ACIPv4List, 0) {
}
bool ReadableACIPv4List::Element::Validate() const {
    static_assert(sizeof(ReadableACIPv4List::Element) == 4);
    return ElementHeader::GetElementType() == ElementHeader::ACIPv4List
        && GetLength() <= ReadableACIPv4List::Element::max_count * sizeof(uint32_t)
        && (GetLength() % sizeof(uint32_t)) == 0;
}
size_t ReadableACIPv4List::Element::GetCount() const {
    return GetLength() / sizeof(uint32_t);
}

bool ReadableACIPv4List::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ElementHeader) > raw_data->end) {
        return false;
    }

    if (raw_data->current + sizeof(ReadableACIPv4List::Element) > raw_data->end) {
        return false;
    }

    auto item = (ReadableACIPv4List::Element *)raw_data->current;
    if (!item->Validate()) {
        return false;
    }

    uint8_t *last = raw_data->current + sizeof(ElementHeader) + item->GetLength();
    if (last > raw_data->end) {
        return false;
    }

    raw_data->current = last;
    element = item;
    is_present = true;
    return true;
}

const ReadableACIPv4List::Element *ReadableACIPv4List::Get() const {
    return element;
}

void ReadableACIPv4List::Log() const {
    ASSERT(element != nullptr);

    log_i("ME ACIPv4List size:%zu, adrs:", element->GetCount());
    for (size_t i = 0; i < element->GetCount(); i++) {
        log_i("     #%zu, %s", i, IpToString(element->addresses[i]).c_str());
    }
}

ElementHeader::ElementType ReadableACIPv4List::GetElementType() const {
    return ElementHeader::ACIPv4List;
}

bool ReadableACIPv4List::IsPresent() const {
    return is_present;
}