
#include "ReturnedMessageElement.h"
#include "lassert.h"
#include "logging.h"
#include <algorithm>
#include <string.h>

ReturnedMessageElement::ReturnedMessageElement(Reasons reason, uint16_t data_length)
    : ElementHeader(ElementHeader::ReturnedMessageElement,
                    (sizeof(ReturnedMessageElement) - sizeof(ElementHeader)) + data_length),
      reason{ reason }, data_length{ (uint8_t)data_length } {
}

ReturnedMessageElement::Reasons ReturnedMessageElement::GetReason() const {
    return reason;
}

uint8_t ReturnedMessageElement::GetDataLength() const {
    return data_length;
}

bool ReturnedMessageElement::Validate() const {
    static_assert(sizeof(ReturnedMessageElement) == 6);
    if (ElementHeader::GetElementType() != ElementHeader::ReturnedMessageElement) {
        return false;
    }
    if (GetLength() - (sizeof(ReturnedMessageElement) - sizeof(ElementHeader))
        > ReturnedMessageElement::max_data_size) {
        log_e("ReturnedMessageElement: element length exceeds limit in : %u",
              (unsigned)ReturnedMessageElement::max_data_size);
        return false;
    }
    switch (reason) {
        case ReturnedMessageElement::Reasons::Reserved:
        case ReturnedMessageElement::Reasons::UnknownMessageElement:
        case ReturnedMessageElement::Reasons::UnsupportedMessageElement:
        case ReturnedMessageElement::Reasons::UnknownMessageElementValue:
        case ReturnedMessageElement::Reasons::UnsupportedMessageElementValue:
            break;

        default:
            return false;
    }
    return true;
}

void ReturnedMessageElement::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(ReturnedMessageElement) <= raw_data->end);
    ReturnedMessageElement *dst = (ReturnedMessageElement *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(ReturnedMessageElement);
}

ReturnedMessageElement *ReturnedMessageElement::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ReturnedMessageElement) > raw_data->end) {
        return nullptr;
    }

    auto res = (ReturnedMessageElement *)raw_data->current;
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

WritableReturnedMessageElementArray::WritableReturnedMessageElementArray(
    const nonstd::span<const Item> &items)
    : items(items.begin(), items.end()) {
    ASSERT(items.size() <= ReadableReturnedMessageElementArray::max_count);
}

WritableReturnedMessageElementArray::WritableReturnedMessageElementArray() {
    items.reserve(ReadableReturnedMessageElementArray::max_count);
}

void WritableReturnedMessageElementArray::Add(ReturnedMessageElement::Reasons reason,
                                              std::vector<uint8_t> val) {
    ASSERT(items.size() + 1 <= ReadableReturnedMessageElementArray::max_count);

    auto it_exists =
        std::find_if(items.begin(),
                     items.end(),
                     [&reason, &val](const WritableReturnedMessageElementArray::Item &item) {
                         return item.header.GetReason() == reason && item.data == val;
                     });

    if (it_exists != items.end()) {
        *it_exists = WritableReturnedMessageElementArray::Item{ reason, std::move(val) };
        log_i("ReturnedMessageElement: replace Reason: %u, Data size: %zu",
              reason,
              (*it_exists).data.size());
    } else {
        items.emplace_back(reason, std::move(val));
    }
}

bool WritableReturnedMessageElementArray::Empty() const {
    return items.empty();
}

void WritableReturnedMessageElementArray::Clear() {
    items = {};
}

void WritableReturnedMessageElementArray::Serialize(RawData *raw_data) const {
    for (const auto &elem : items) {
        elem.header.Serialize(raw_data);
        uint16_t data_size =
            elem.header.GetLength() - (sizeof(ReturnedMessageElement) - sizeof(ElementHeader));
        memcpy(raw_data->current, elem.data.data(), data_size);
        raw_data->current += data_size;
    }
}

bool WritableReturnedMessageElementArray::Validate() const {
    for (const auto &elem : items) {
        if (!elem.header.Validate()) {
            return false;
        }
    }
    return true;
}

void WritableReturnedMessageElementArray::Log() const {
    log_i("ME ReturnedMessageElement count:%lu", items.size());
    for (size_t i = 0; i < items.size(); i++) {
        log_i("     #%lu: Reason: %u, Length: %u",
              i,
              items[i].header.GetReason(),
              items[i].header.GetDataLength());
    }
}

ReadableReturnedMessageElementArray::ReadableReturnedMessageElementArray() : count{ 0 } {
}

bool ReadableReturnedMessageElementArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableReturnedMessageElementArray::Deserialize elements count exceeds");
        return false;
    }

    if (raw_data->current + sizeof(ReturnedMessageElement) > raw_data->end) {
        return false;
    }

    auto item = (ReturnedMessageElement *)raw_data->current;
    if (!item->Validate()) {
        return false;
    }
    raw_data->current += item->GetLength() + sizeof(ElementHeader);

    items[count] = item;
    count++;
    return true;
}

nonstd::span<const ReturnedMessageElement *const> ReadableReturnedMessageElementArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableReturnedMessageElementArray::Log() const {
    log_i("ME ReturnedMessageElement count:%lu", count);
    for (size_t i = 0; i < count; i++) {
        log_i("     #%lu: Reason: %u, Length: %u",
              i,
              items[i]->GetReason(),
              items[i]->GetDataLength());
    }
}