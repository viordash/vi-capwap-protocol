
#include "ACNameWithPriority.h"
#include "Logging.h"
#include "lassert.h"
#include <algorithm>
#include <cstring>

ACNameWithPriorityHeader::ACNameWithPriorityHeader(uint8_t priority, uint16_t length)
    : ElementHeader(ElementHeader::ACNameWithPriority,
                    (sizeof(ACNameWithPriorityHeader) - sizeof(ElementHeader)) + length),
      priority{ priority } {
}

uint8_t ACNameWithPriorityHeader::GetPriority() const {
    return priority;
}

uint16_t ACNameWithPriorityHeader::GetNameLenght() const {
    return GetLength() - (sizeof(ACNameWithPriorityHeader) - sizeof(ElementHeader));
}

bool ACNameWithPriorityHeader::Validate() const {
    static_assert(sizeof(ACNameWithPriorityHeader) == 5);
    if (ElementHeader::GetElementType() != ElementHeader::ACNameWithPriority) {
        return false;
    }
    if (GetNameLenght() > ACNameWithPriorityHeader::max_data_size) {
        return false;
    }
    return true;
}

void ACNameWithPriorityHeader::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(ACNameWithPriorityHeader) <= raw_data->end);
    std::memcpy(raw_data->current, this, sizeof(ACNameWithPriorityHeader));
    raw_data->current += sizeof(ACNameWithPriorityHeader);
}

ACNameWithPriorityHeader *ACNameWithPriorityHeader::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ACNameWithPriorityHeader) > raw_data->end) {
        return nullptr;
    }

    auto res = (ACNameWithPriorityHeader *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }

    uint8_t *last = raw_data->current + sizeof(ElementHeader) + res->GetLength();
    if (last > raw_data->end) {
        return nullptr;
    }

    raw_data->current = last;
    return res;
}

WritableACNameWithPriorityArray::WritableACNameWithPriorityArray() {
    items.reserve(ReadableACNameWithPriorityArray::max_count);
}

void WritableACNameWithPriorityArray::Add(uint8_t priority, const std::string_view str) {
    ASSERT(items.size() + 1 <= ReadableACNameWithPriorityArray::max_count);

    auto it_exists =
        std::find_if(items.begin(),
                     items.end(),
                     [&priority, &str](const WritableACNameWithPriorityArray::Item &item) {
                         return item.header.GetPriority() == priority && item.name == str;
                     });
    if (it_exists != items.end()) {
        *it_exists = WritableACNameWithPriorityArray::Item{ priority, str };
        log_i("ACNameWithPriorityHeader: replace Priority: %u, Name: '%.*s'",
              priority,
              (*it_exists).header.GetNameLenght(),
              (*it_exists).name.data());
    } else {
        items.emplace_back(priority, str);
    }
}

bool WritableACNameWithPriorityArray::Empty() const {
    return items.empty();
}

void WritableACNameWithPriorityArray::Clear() {
    items.clear();
}

void WritableACNameWithPriorityArray::Serialize(RawData *raw_data) const {
    ASSERT(items.size() <= ReadableACNameWithPriorityArray::max_count);

    for (const auto &elem : items) {
        elem.header.Serialize(raw_data);
        uint16_t data_size =
            elem.header.GetLength() - (sizeof(ACNameWithPriorityHeader) - sizeof(ElementHeader));
        memcpy(raw_data->current, elem.name.data(), data_size);
        raw_data->current += data_size;
    }
}

void WritableACNameWithPriorityArray::Log() const {
    log_i("ME ACNameWithPriorityHeader count:%zu", items.size());
    for (size_t i = 0; i < items.size(); i++) {
        log_i("     #%zu: priority: %u, :%.*s",
              i,
              items[i].header.GetPriority(),
              items[i].header.GetNameLenght(),
              (char *)items[i].name.data());
    }
}

ReadableACNameWithPriorityArray::ReadableACNameWithPriorityArray() : count{ 0 } {
}

bool ReadableACNameWithPriorityArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableACNameWithPriorityArray::Deserialize elements count exceeds");
        return false;
    }

    auto item = ACNameWithPriorityHeader::Deserialize(raw_data);
    if (item == nullptr) {
        return false;
    }
    items[count] = item;
    count++;
    return true;
}

nonstd::span<const ACNameWithPriorityHeader *const> ReadableACNameWithPriorityArray::Get() const {
    nonstd::span span(items.data(), count);
    return span;
}

void ReadableACNameWithPriorityArray::Log() const {
    log_i("ME ACNameWithPriorityHeader count:%zu", count);
    for (size_t i = 0; i < count; i++) {
        log_i("     #%zu: priority: %u, :%.*s",
              i,
              items[i]->GetPriority(),
              items[i]->GetNameLenght(),
              (char *)items[i]->name);
    }
}

ElementHeader::ElementType ReadableACNameWithPriorityArray::GetElementType() const {
    return ElementHeader::ACNameWithPriority;
}

bool ReadableACNameWithPriorityArray::IsPresent() const {
    return count > 0;
}