
#include "ACNameWithPriority.h"
#include "lassert.h"
#include "logging.h"
#include <algorithm>
#include <string.h>

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

void ACNameWithPriority::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(ACNameWithPriority) <= raw_data->end);
    ACNameWithPriority *dst = (ACNameWithPriority *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(ACNameWithPriority);
}

ACNameWithPriority *ACNameWithPriority::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ACNameWithPriority) > raw_data->end) {
        return nullptr;
    }

    auto res = (ACNameWithPriority *)raw_data->current;
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
    std::vector<char> vec(str.begin(), str.end());

    auto it_exists =
        std::find_if(items.begin(),
                     items.end(),
                     [&priority, &vec](const WritableACNameWithPriorityArray::Item &item) {
                         return item.header.GetPriority() == priority && item.name == vec;
                     });
    if (it_exists != items.end()) {
        *it_exists = WritableACNameWithPriorityArray::Item{ priority, std::move(vec) };
        log_i("ACNameWithPriority: replace Priority: %u, Name: '%.*s'",
              priority,
              (*it_exists).header.GetNameLenght(),
              (*it_exists).name.data());
    } else {
        items.emplace_back(priority, std::move(vec));
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
            elem.header.GetLength() - (sizeof(ACNameWithPriority) - sizeof(ElementHeader));
        memcpy(raw_data->current, elem.name.data(), data_size);
        raw_data->current += data_size;
    }
}

void WritableACNameWithPriorityArray::Log() const {
    log_i("ME ACNameWithPriority count:%lu", items.size());
    for (size_t i = 0; i < items.size(); i++) {
        log_i("     #%lu: priority: %u, :%.*s",
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

    auto item = ACNameWithPriority::Deserialize(raw_data);
    if (item == nullptr) {
        return false;
    }
    items[count] = item;
    count++;
    return true;
}

nonstd::span<const ACNameWithPriority *const> ReadableACNameWithPriorityArray::Get() const {
    nonstd::span span(items.data(), count);
    return span;
}

void ReadableACNameWithPriorityArray::Log() const {
    log_i("ME ACNameWithPriority count:%lu", count);
    for (size_t i = 0; i < count; i++) {
        log_i("     #%lu: priority: %u, :%.*s",
              i,
              items[i]->GetPriority(),
              items[i]->GetNameLenght(),
              (char *)items[i]->name);
    }
}