#include "DTLSHeader.h"
#include "lassert.h"
#include "logging.h"

DTLSHeader::DTLSHeader() : preamble{ Preamble::DTLSCrypted, Preamble::RFC_5415 }, reserved{} {};

bool DTLSHeader::Validate() {
    static_assert(sizeof(DTLSHeader) == 4);
    return reserved == 0;
}
void DTLSHeader::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(DTLSHeader) <= raw_data->end);
    DTLSHeader *dst = (DTLSHeader *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(DTLSHeader);
}
DTLSHeader *DTLSHeader::Deserialize(RawData *raw_data) {
    auto preamble = Preamble::Deserialize(raw_data);
    if (preamble == nullptr || preamble->type != Preamble::DTLSCrypted) {
        return nullptr;
    }
    if (raw_data->current + sizeof(DTLSHeader) > raw_data->end) {
        return nullptr;
    }

    auto res = (DTLSHeader *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += sizeof(DTLSHeader);
    return res;
}