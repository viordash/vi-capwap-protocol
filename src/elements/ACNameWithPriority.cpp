
#include "ACNameWithPriority.h"
#include "Logging.h"
#include "lassert.h"
#include <algorithm>
#include <cstring>

ACNameWithPriority::ACNameWithPriority(uint8_t priority, uint16_t length)
    : ElementHeader(ElementHeader::ACNameWithPriority,
                    (sizeof(ACNameWithPriority) - sizeof(ElementHeader)) + length),
      priority{ priority } {
}

uint8_t ACNameWithPriority::GetPriority() const {
    return priority;
}

uint16_t ACNameWithPriority::GetNameLenght() const {
    return GetLength() - (sizeof(ACNameWithPriority) - sizeof(ElementHeader));
}

bool ACNameWithPriority::Validate() const {
    static_assert(sizeof(ACNameWithPriority) == 5);
    if (ElementHeader::GetElementType() != ElementHeader::ACNameWithPriority) {
        return false;
    }
    if (GetNameLenght() > ACNameWithPriority::max_data_size) {
        return false;
    }
    return true;
}

WritableACNameWithPriorityArray::WritableACNameWithPriorityArray() {
    static_assert(sizeof(Item::header) == 5);
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
        log_i("ACNameWithPriority: replace Priority: %u, Name: '%.*s'",
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

    for (const auto &item : items) {
        ASSERT(raw_data->current + sizeof(item.header) <= raw_data->end);
        std::memcpy(raw_data->current, &item.header, sizeof(item.header));
        raw_data->current += sizeof(item.header);
        uint16_t data_size =
            item.header.GetLength() - (sizeof(item.header) - sizeof(ElementHeader));
        std::memcpy(raw_data->current, item.name.data(), data_size);
        raw_data->current += data_size;
    }
}

void WritableACNameWithPriorityArray::Log() const {
    log_i("ME ACNameWithPriority count:%zu", items.size());
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

    if (raw_data->current + sizeof(ACNameWithPriority) > raw_data->end) {
        return false;
    }

    auto item = (ReadableACNameWithPriorityArray::Item *)raw_data->current;
    if (!item->Validate()) {
        return false;
    }

    uint8_t *last = raw_data->current + sizeof(ElementHeader) + item->GetLength();
    if (last > raw_data->end) {
        return false;
    }

    raw_data->current = last;
    items[count] = item;
    count++;
    return true;
}

nonstd::span<const ReadableACNameWithPriorityArray::Item *const>
ReadableACNameWithPriorityArray::Get() const {
    nonstd::span span(items.data(), count);
    return span;
}

void ReadableACNameWithPriorityArray::Log() const {
    log_i("ME ACNameWithPriority count:%zu", count);
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