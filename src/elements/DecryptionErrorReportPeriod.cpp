#include "DecryptionErrorReportPeriod.h"
#include "lassert.h"
#include "logging.h"
#include <algorithm>

DecryptionErrorReportPeriod::DecryptionErrorReportPeriod(uint8_t radio_id, uint16_t report_interval)
    : ElementHeader(ElementHeader::DecryptionErrorReportPeriod,
                    sizeof(DecryptionErrorReportPeriod) - sizeof(ElementHeader)),
      radio_id{ radio_id }, report_interval{ report_interval } {
}
bool DecryptionErrorReportPeriod::Validate() const {
    static_assert(sizeof(DecryptionErrorReportPeriod) == 7);
    return ElementHeader::GetElementType() == ElementHeader::DecryptionErrorReportPeriod
        && ElementHeader::GetLength()
               == (sizeof(DecryptionErrorReportPeriod) - sizeof(ElementHeader))
        && radio_id <= 31;
}
void DecryptionErrorReportPeriod::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(DecryptionErrorReportPeriod) <= raw_data->end);
    DecryptionErrorReportPeriod *dst = (DecryptionErrorReportPeriod *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(DecryptionErrorReportPeriod);
}
DecryptionErrorReportPeriod *DecryptionErrorReportPeriod::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(DecryptionErrorReportPeriod) > raw_data->end) {
        return nullptr;
    }

    auto res = (DecryptionErrorReportPeriod *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += sizeof(DecryptionErrorReportPeriod);
    return res;
}

uint8_t DecryptionErrorReportPeriod::RadioID() const {
    return radio_id;
}

uint16_t DecryptionErrorReportPeriod::ReportInterval() const {
    return report_interval.Get();
}

WritableDecryptionErrorReportPeriodArray::WritableDecryptionErrorReportPeriodArray() {
    items.reserve(ReadableDecryptionErrorReportPeriodArray::max_count);
}

void WritableDecryptionErrorReportPeriodArray::Add(DecryptionErrorReportPeriod period) {
    ASSERT(items.size() + 1 <= ReadableDecryptionErrorReportPeriodArray::max_count);

    auto it_exists = std::find_if(items.begin(),
                                  items.end(),
                                  [&period](const DecryptionErrorReportPeriod &item) {
                                      return item.RadioID() == period.RadioID();
                                  });
    if (it_exists != items.end()) {
        *it_exists = std::move(period);
        log_i("DecryptionErrorReportPeriod: replace RadioID: %u", (*it_exists).RadioID());
    } else {
        items.emplace_back(std::move(period));
    }
}

bool WritableDecryptionErrorReportPeriodArray::Empty() const {
    return items.empty();
}

void WritableDecryptionErrorReportPeriodArray::Clear() {
    items.clear();
}

void WritableDecryptionErrorReportPeriodArray::Serialize(RawData *raw_data) const {
    for (const auto &elem : items) {
        elem.Serialize(raw_data);
    }
}

void WritableDecryptionErrorReportPeriodArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME DecryptionErrorReportPeriod #%lu RadioID:%u, Report interval:%u",
              i,
              items[i].RadioID(),
              items[i].ReportInterval());
    }
}

ReadableDecryptionErrorReportPeriodArray::ReadableDecryptionErrorReportPeriodArray() : count{ 0 } {
}

bool ReadableDecryptionErrorReportPeriodArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableDecryptionErrorReportPeriodArray::Deserialize elements count exceeds");
        return false;
    }

    auto item = DecryptionErrorReportPeriod::Deserialize(raw_data);
    if (item == nullptr) {
        return false;
    }
    items[count] = item;
    count++;
    return true;
}

nonstd::span<const DecryptionErrorReportPeriod *const>
ReadableDecryptionErrorReportPeriodArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableDecryptionErrorReportPeriodArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME DecryptionErrorReportPeriod #%lu RadioID:%u, Report interval:%u",
              i,
              items[i]->RadioID(),
              items[i]->ReportInterval());
    }
}