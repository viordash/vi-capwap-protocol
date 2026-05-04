#include "WTPQualityOfService.h"
#include "Logging.h"
#include "lassert.h"
#include <algorithm>
#include <cstring>

uint8_t QoSSubElement::Get8021pTag() const {
    // Bits 10-8: 802.1p priority value (3 bits)
    return (TagFields.Get() >> 8) & 0x07;
}

uint8_t QoSSubElement::GetDscpTag() const {
    // Bits 5-0: DSCP Tag (6 bits)
    return TagFields.Get() & 0x3F;
}

void QoSSubElement::Set8021pTag(uint8_t tag) {
    uint16_t value = TagFields.Get();
    value = (value & 0xF8FF) | ((tag & 0x07) << 8);
    TagFields = NetworkU16{ value };
}

void QoSSubElement::SetDscpTag(uint8_t tag) {
    uint16_t value = TagFields.Get();
    value = (value & 0xFFC0) | (tag & 0x3F);
    TagFields = NetworkU16{ value };
}

WTPQualityOfService::WTPQualityOfService(uint8_t radio_id, uint8_t tagging_policy)
    : ElementHeader(ElementHeader::WTPQualityOfService,
                    sizeof(WTPQualityOfService) - sizeof(ElementHeader)),
      RadioID{ radio_id }, TaggingPolicy{ tagging_policy }, Voice{}, Video{}, BestEffort{},
      Background{} {
}

bool WTPQualityOfService::GetP() const {
    return (TaggingPolicy >> 4) & 0x01;
}

void WTPQualityOfService::SetP(bool value) {
    if (value) {
        TaggingPolicy |= (1 << 4);
    } else {
        TaggingPolicy &= ~(1 << 4);
    }
}

bool WTPQualityOfService::GetQ() const {
    return (TaggingPolicy >> 3) & 0x01;
}

void WTPQualityOfService::SetQ(bool value) {
    if (value) {
        TaggingPolicy |= (1 << 3);
    } else {
        TaggingPolicy &= ~(1 << 3);
    }
}

bool WTPQualityOfService::GetD() const {
    return (TaggingPolicy >> 2) & 0x01;
}

void WTPQualityOfService::SetD(bool value) {
    if (value) {
        TaggingPolicy |= (1 << 2);
    } else {
        TaggingPolicy &= ~(1 << 2);
    }
}

bool WTPQualityOfService::GetO() const {
    return (TaggingPolicy >> 1) & 0x01;
}

void WTPQualityOfService::SetO(bool value) {
    if (value) {
        TaggingPolicy |= (1 << 1);
    } else {
        TaggingPolicy &= ~(1 << 1);
    }
}

bool WTPQualityOfService::GetI() const {
    return TaggingPolicy & 0x01;
}

void WTPQualityOfService::SetI(bool value) {
    if (value) {
        TaggingPolicy |= 1;
    } else {
        TaggingPolicy &= ~1;
    }
}

bool WTPQualityOfService::Validate() const {
    static_assert(sizeof(QoSSubElement) == 8);
    static_assert(sizeof(WTPQualityOfService) == 38); // 4 header + 34 value
    if (ElementHeader::GetElementType() != ElementHeader::WTPQualityOfService) {
        return false;
    }
    if (ElementHeader::GetLength() != (sizeof(WTPQualityOfService) - sizeof(ElementHeader))) {
        return false;
    }
    if (RadioID > 31) {
        return false;
    }
    // Reserved bits (7-5) must be zero
    if ((TaggingPolicy & 0xE0) != 0) {
        return false;
    }
    return true;
}

WritableWTPQualityOfServiceArray::WritableWTPQualityOfServiceArray() {
    static_assert(sizeof(items[0]) == 38);
    items.reserve(ReadableWTPQualityOfServiceArray::max_count);
}

void WritableWTPQualityOfServiceArray::Add(WTPQualityOfService element) {
    ASSERT(items.size() + 1 <= ReadableWTPQualityOfServiceArray::max_count);

    auto it_exists =
        std::find_if(items.begin(), items.end(), [&element](const WTPQualityOfService &item) {
            return item.RadioID == element.RadioID;
        });

    if (it_exists != items.end()) {
        *it_exists = std::move(element);
        log_i("WTPQualityOfService: replace RadioID: %u", (*it_exists).RadioID);
    } else {
        items.emplace_back(std::move(element));
    }
}

bool WritableWTPQualityOfServiceArray::Empty() const {
    return items.empty();
}

void WritableWTPQualityOfServiceArray::Clear() {
    items.clear();
}

void WritableWTPQualityOfServiceArray::Serialize(RawData *raw_data) const {
    for (const auto &item : items) {
        ASSERT(raw_data->current + sizeof(item) <= raw_data->end);
        std::memcpy(raw_data->current, &item, sizeof(item));
        raw_data->current += sizeof(item);
    }
}

void WritableWTPQualityOfServiceArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME WTPQualityOfService #%zu RadioID:%u, TaggingPolicy:0x%02X",
              i,
              items[i].RadioID,
              items[i].TaggingPolicy);
    }
}

ReadableWTPQualityOfServiceArray::ReadableWTPQualityOfServiceArray() : count{ 0 } {
}

ElementHeader::ElementType ReadableWTPQualityOfServiceArray::GetElementType() const {
    return ElementHeader::WTPQualityOfService;
}

bool ReadableWTPQualityOfServiceArray::IsPresent() const {
    return count > 0;
}

bool ReadableWTPQualityOfServiceArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableWTPQualityOfServiceArray::Deserialize elements count exceeds");
        return false;
    }

    if (raw_data->current + sizeof(WTPQualityOfService) > raw_data->end) {
        return false;
    }

    auto item = (WTPQualityOfService *)raw_data->current;
    if (!item->Validate()) {
        return false;
    }

    raw_data->current += sizeof(WTPQualityOfService);
    items[count] = item;
    count++;
    return true;
}

nonstd::span<const WTPQualityOfService *const> ReadableWTPQualityOfServiceArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableWTPQualityOfServiceArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME WTPQualityOfService #%zu RadioID:%u, TaggingPolicy:0x%02X",
              i,
              items[i]->RadioID,
              items[i]->TaggingPolicy);
    }
}
