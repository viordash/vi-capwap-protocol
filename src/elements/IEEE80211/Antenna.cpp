#include "Antenna.h"
#include "Logging.h"
#include "lassert.h"
#include <algorithm>
#include <cstring>

Antenna::Antenna(uint8_t radio_id, Diversity diversity, Combiner combiner, uint8_t antenna_count)
    : ElementHeader(ElementHeader::Antenna,
                    (sizeof(Antenna) - sizeof(ElementHeader))
                        + antenna_count * sizeof(Antenna::AntennaSelection)),
      radio_id{ radio_id }, diversity{ diversity }, combiner{ combiner },
      antenna_count{ antenna_count } {
}

uint8_t Antenna::GetRadioID() const {
    return radio_id;
}

Antenna::Diversity Antenna::GetDiversity() const {
    return diversity;
}

Antenna::Combiner Antenna::GetCombiner() const {
    return combiner;
}

uint8_t Antenna::GetAntennaCount() const {
    return antenna_count;
}

bool Antenna::Validate() const {
    static_assert(sizeof(Antenna) == 8);
    if (ElementHeader::GetElementType() != ElementHeader::Antenna) {
        return false;
    }
    auto selection_count = GetLength() - (sizeof(Antenna) - sizeof(ElementHeader));
    if (selection_count > max_antenna_count) {
        log_e("Antenna: antenna selection count exceeds limit: %u", (unsigned)max_antenna_count);
        return false;
    }
    if (selection_count != antenna_count) {
        log_e("Antenna: antenna count mismatch");
        return false;
    }
    if (radio_id > 31) {
        return false;
    }
    switch (diversity) {
        case Antenna::Diversity::Disabled:
        case Antenna::Diversity::Enabled:
            break;
        default:
            return false;
    }
    switch (combiner) {
        case Antenna::Combiner::SectorizedLeft:
        case Antenna::Combiner::SectorizedRight:
        case Antenna::Combiner::Omni:
        case Antenna::Combiner::MIMO:
            break;
        default:
            return false;
    }
    return true;
}

WritableAntennaArray::WritableAntennaArray() {
    items.reserve(ReadableAntennaArray::max_count);
}

void WritableAntennaArray::Add(uint8_t radio_id,
                               Antenna::Diversity diversity,
                               Antenna::Combiner combiner,
                               nonstd::span<const Antenna::AntennaSelection> selections) {
    ASSERT(items.size() + 1 <= ReadableAntennaArray::max_count);

    auto it_exists = std::find_if(items.begin(),
                                  items.end(),
                                  [&radio_id](const WritableAntennaArray::Item &item) {
                                      return item.header.GetRadioID() == radio_id;
                                  });

    if (it_exists != items.end()) {
        *it_exists = WritableAntennaArray::Item{ radio_id, diversity, combiner, selections };
        log_i("Antenna: replace RadioID: %u", radio_id);
    } else {
        items.emplace_back(radio_id, diversity, combiner, selections);
    }
}

bool WritableAntennaArray::Empty() const {
    return items.empty();
}

void WritableAntennaArray::Clear() {
    items.clear();
}

void WritableAntennaArray::Serialize(RawData *raw_data) const {
    for (const auto &elem : items) {
        ASSERT(raw_data->current + sizeof(Antenna) <= raw_data->end);
        std::memcpy(raw_data->current, &elem.header, sizeof(Antenna));
        raw_data->current += sizeof(Antenna);

        auto selection_count = elem.header.GetLength() - (sizeof(Antenna) - sizeof(ElementHeader));
        std::memcpy(raw_data->current, elem.selections.data(), selection_count);
        raw_data->current += selection_count;
    }
}

void WritableAntennaArray::Log() const {
    for (size_t i = 0; i < items.size(); i++) {
        log_i("ME Antenna #%zu RadioID:%u, Diversity:%u, Combiner:%u, AntennaCount:%u",
              i,
              items[i].header.GetRadioID(),
              items[i].header.GetDiversity(),
              items[i].header.GetCombiner(),
              items[i].header.GetAntennaCount());
    }
}

ReadableAntennaArray::ReadableAntennaArray() : count{ 0 } {
}

bool ReadableAntennaArray::Deserialize(RawData *raw_data) {
    if (count >= max_count) {
        log_e("ReadableAntennaArray::Deserialize elements count exceeds");
        return false;
    }

    if (raw_data->current + sizeof(Antenna) > raw_data->end) {
        return false;
    }

    auto res = (ReadableAntennaArray::Item *)raw_data->current;
    if (!res->Validate()) {
        return false;
    }

    uint8_t *last = raw_data->current + sizeof(ElementHeader) + res->GetLength();
    if (last > raw_data->end) {
        return false;
    }

    raw_data->current = last;
    items[count] = res;
    count++;
    return true;
}

nonstd::span<const ReadableAntennaArray::Item *const> ReadableAntennaArray::Get() const {
    nonstd::span span(items.begin(), count);
    return span;
}

void ReadableAntennaArray::Log() const {
    for (size_t i = 0; i < count; i++) {
        log_i("ME Antenna #%zu RadioID:%u, Diversity:%u, Combiner:%u, AntennaCount:%u",
              i,
              items[i]->GetRadioID(),
              items[i]->GetDiversity(),
              items[i]->GetCombiner(),
              items[i]->GetAntennaCount());
    }
}

ElementHeader::ElementType ReadableAntennaArray::GetElementType() const {
    return ElementHeader::Antenna;
}

bool ReadableAntennaArray::IsPresent() const {
    return count > 0;
}
