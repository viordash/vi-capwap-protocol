
#include "WTPRadioInformation.h"
#include "Logging.h"
#include "lassert.h"
#include <cstring>
#include <sstream>

WTPRadioInformation::WTPRadioInformation(uint8_t radio_id,
                                         bool b,
                                         bool a,
                                         bool g,
                                         bool n,
                                         bool ac,
                                         bool ax,
                                         bool be)
    : ElementHeader(ElementHeader::WTPRadioInformation,
                    sizeof(WTPRadioInformation) - sizeof(ElementHeader)),
      RadioID{ radio_id }, Reservd_0{ 0 }, Reservd_1{ 0 }, Reservd_2{ 0 }, B{ b }, A{ a }, G{ g },
      N{ n }, AC{ ac }, AX{ ax }, BE{ be }, Reservd_3{ 0 } {
}
bool WTPRadioInformation::Validate() const {
    static_assert(sizeof(WTPRadioInformation) == 9);
    return ElementHeader::GetElementType() == ElementHeader::WTPRadioInformation
        && ElementHeader::GetLength() == (sizeof(WTPRadioInformation) - sizeof(ElementHeader)) //
        && RadioID <= 31 && Reservd_0 == 0;
}
uint16_t WTPRadioInformation::GetTotalLength() const {
    return GetLength() + sizeof(ElementHeader);
}

std::string WTPRadioInformation::ToString() const {
    std::stringstream ss;

    if (B) {
        ss << " B";
    }
    if (A) {
        ss << " A";
    }
    if (G) {
        ss << " G";
    }
    if (N) {
        ss << " N";
    }
    if (AC) {
        ss << " AC";
    }
    if (AX) {
        ss << " AX";
    }
    if (BE) {
        ss << " BE";
    }

    return ss.str();
}

WritableWTPRadioInformationArray::WritableWTPRadioInformationArray() {
    static_assert(sizeof(items[0]) == 9);
    items.reserve(ReadableWTPRadioInformationArray::max_count);
}

void WritableWTPRadioInformationArray::Add(WTPRadioInformation radio_info) {
    ASSERT(items.size() + 1 <= ReadableWTPRadioInformationArray::max_count);

    auto it_exists =
        std::find_if(items.begin(), items.end(), [&radio_info](const WTPRadioInformation &item) {
            return item.RadioID == radio_info.RadioID;
        });
    if (it_exists != items.end()) {
        *it_exists = std::move(radio_info);
        log_i("WTPRadioInformation: replace RadioID: {}", (*it_exists).RadioID);
    } else {
        items.emplace_back(std::move(radio_info));
    }
}

bool WritableWTPRadioInformationArray::Empty() const {
    return items.empty();
}

void WritableWTPRadioInformationArray::Clear() {
    items.clear();
}

size_t WritableWTPRadioInformationArray::Size() {
    return items.size();
}

void WritableWTPRadioInformationArray::Serialize(RawData *raw_data) const {
    for (const auto &item : items) {
        ASSERT(raw_data->current + sizeof(item) <= raw_data->end);
        std::memcpy(raw_data->current, &item, sizeof(item));
        raw_data->current += sizeof(item);
    }
}
uint16_t WritableWTPRadioInformationArray::GetTotalLength() const {
    uint16_t size = 0;
    for (const auto &elem : items) {
        size += elem.GetTotalLength();
    }
    return size;
}

void WritableWTPRadioInformationArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME WTPRadioInfo #{} RadioID:{}, 802.11 ({} )",
              i,
              items[i].RadioID,
              items[i].ToString());
    }
}

ReadableWTPRadioInformationArray::ReadableWTPRadioInformationArray() : count{ 0 } {
}

ElementHeader::ElementType ReadableWTPRadioInformationArray::GetElementType() const {
    return ElementHeader::WTPRadioInformation;
}

bool ReadableWTPRadioInformationArray::IsPresent() const {
    return count > 0;
}

bool ReadableWTPRadioInformationArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableWTPRadioInformationArray::Deserialize elements count exceeds");
        return false;
    }

    if (raw_data->current + sizeof(WTPRadioInformation) > raw_data->end) {
        return false;
    }

    auto item = (WTPRadioInformation *)raw_data->current;
    if (!item->Validate()) {
        return false;
    }

    raw_data->current += sizeof(WTPRadioInformation);
    items[count] = item;
    count++;
    return true;
}

nonstd::span<const WTPRadioInformation *const> ReadableWTPRadioInformationArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableWTPRadioInformationArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME WTPRadioInfo #{} RadioID:{}, 802.11 ({} )",
              i,
              items[i]->RadioID,
              items[i]->ToString());
    }
}
