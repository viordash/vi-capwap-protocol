#include "WTPRadioConfiguration.h"
#include "Logging.h"
#include "lassert.h"
#include <algorithm>
#include <cstring>

WTPRadioConfiguration::WTPRadioConfiguration(uint8_t radio_id,
                                             uint8_t short_preamble,
                                             uint8_t num_bssids,
                                             uint8_t dtim_period,
                                             const uint8_t *bssid,
                                             uint16_t beacon_period,
                                             const char *country)
    : ElementHeader(ElementHeader::WTPRadioConfiguration,
                    sizeof(WTPRadioConfiguration) - sizeof(ElementHeader)),
      RadioID{ radio_id }, ShortPreamble{ short_preamble }, NumBSSIDs{ num_bssids },
      DTIMPeriod{ dtim_period }, BeaconPeriod{ beacon_period } {
    memcpy(BSSID, bssid, bssid_size);
    memset(CountryString, 0, country_string_size);
    if (country != nullptr) {
        size_t len = strlen(country);
        if (len > country_string_size - 1) {
            len = country_string_size - 1;
        }
        memcpy(CountryString, country, len);
    }
}

bool WTPRadioConfiguration::Validate() const {
    static_assert(sizeof(WTPRadioConfiguration) == 20); // 4 header + 16 value
    if (ElementHeader::GetElementType() != ElementHeader::WTPRadioConfiguration) {
        return false;
    }
    if (ElementHeader::GetLength() != (sizeof(WTPRadioConfiguration) - sizeof(ElementHeader))) {
        return false;
    }
    if (RadioID > 31) {
        return false;
    }
    if (ShortPreamble > 1) {
        return false;
    }
    if (NumBSSIDs < min_NumBSSIDs || NumBSSIDs > max_NumBSSIDs) {
        return false;
    }
    return true;
}

WritableWTPRadioConfigurationArray::WritableWTPRadioConfigurationArray() {
    static_assert(sizeof(items[0]) == 20);
    items.reserve(ReadableWTPRadioConfigurationArray::max_count);
}

void WritableWTPRadioConfigurationArray::Add(WTPRadioConfiguration element) {
    ASSERT(items.size() + 1 <= ReadableWTPRadioConfigurationArray::max_count);

    auto it_exists =
        std::find_if(items.begin(),
                     items.end(),
                     [&element](const WTPRadioConfiguration &item) {
                         return item.RadioID == element.RadioID;
                     });

    if (it_exists != items.end()) {
        *it_exists = std::move(element);
        log_i("WTPRadioConfiguration: replace RadioID: %u", (*it_exists).RadioID);
    } else {
        items.emplace_back(std::move(element));
    }
}

bool WritableWTPRadioConfigurationArray::Empty() const {
    return items.empty();
}

void WritableWTPRadioConfigurationArray::Clear() {
    items.clear();
}

void WritableWTPRadioConfigurationArray::Serialize(RawData *raw_data) const {
    for (const auto &item : items) {
        ASSERT(raw_data->current + sizeof(item) <= raw_data->end);
        std::memcpy(raw_data->current, &item, sizeof(item));
        raw_data->current += sizeof(item);
    }
}

void WritableWTPRadioConfigurationArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME WTPRadioConfiguration #%zu RadioID:%u, ShortPreamble:%u, NumBSSIDs:%u, "
              "BeaconPeriod:%u",
              i,
              items[i].RadioID,
              items[i].ShortPreamble,
              items[i].NumBSSIDs,
              items[i].BeaconPeriod.Get());
    }
}

ReadableWTPRadioConfigurationArray::ReadableWTPRadioConfigurationArray() : count{ 0 } {
}

ElementHeader::ElementType ReadableWTPRadioConfigurationArray::GetElementType() const {
    return ElementHeader::WTPRadioConfiguration;
}

bool ReadableWTPRadioConfigurationArray::IsPresent() const {
    return count > 0;
}


bool ReadableWTPRadioConfigurationArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableWTPRadioConfigurationArray::Deserialize elements count exceeds");
        return false;
    }

    if (raw_data->current + sizeof(WTPRadioConfiguration) > raw_data->end) {
        return false;
    }

    auto item = (WTPRadioConfiguration *)raw_data->current;
    if (!item->Validate()) {
        return false;
    }

    raw_data->current += sizeof(WTPRadioConfiguration);
    items[count] = item;
    count++;
    return true;
}

nonstd::span<const WTPRadioConfiguration *const> ReadableWTPRadioConfigurationArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableWTPRadioConfigurationArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME WTPRadioConfiguration #%zu RadioID:%u, ShortPreamble:%u, NumBSSIDs:%u, "
              "BeaconPeriod:%u",
              i,
              items[i]->RadioID,
              items[i]->ShortPreamble,
              items[i]->NumBSSIDs,
              items[i]->BeaconPeriod.Get());
    }
}
