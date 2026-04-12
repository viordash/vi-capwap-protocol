
#include "WTPRadioInformation.h"
#include "lassert.h"
#include "logging.h"
#include <sstream>
#include <string.h>

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
void WTPRadioInformation::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(WTPRadioInformation) <= raw_data->end);
    WTPRadioInformation *dst = (WTPRadioInformation *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(WTPRadioInformation);
}
WTPRadioInformation *WTPRadioInformation::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(WTPRadioInformation) > raw_data->end) {
        return nullptr;
    }

    auto res = (WTPRadioInformation *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += sizeof(WTPRadioInformation);
    return res;
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

WritableWTPRadioInformationArray::WritableWTPRadioInformationArray(
    const nonstd::span<const WTPRadioInformation> &items)
    : items(items) {
    ASSERT(items.size() <= ReadableWTPRadioInformationArray::max_count);
}

void WritableWTPRadioInformationArray::Serialize(RawData *raw_data) const {
    for (const auto &elem : items) {
        elem.Serialize(raw_data);
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
        log_i("ME WTPRadioInfo #%zu RadioID:%u, 802.11 (%s )",
              i,
              items[i].RadioID,
              items[i].ToString().c_str());
    }
}

ReadableWTPRadioInformationArray::ReadableWTPRadioInformationArray() : count{ 0 } {
}

bool ReadableWTPRadioInformationArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableWTPRadioInformationArray::Deserialize elements count exceeds");
        return false;
    }

    auto radio_info = WTPRadioInformation::Deserialize(raw_data);
    if (radio_info == nullptr) {
        return false;
    }
    items[count] = radio_info;
    count++;
    return true;
}

nonstd::span<const WTPRadioInformation *const> ReadableWTPRadioInformationArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableWTPRadioInformationArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME WTPRadioInfo #%zu RadioID:%u, 802.11 (%s )",
              i,
              items[i]->RadioID,
              items[i]->ToString().c_str());
    }
}