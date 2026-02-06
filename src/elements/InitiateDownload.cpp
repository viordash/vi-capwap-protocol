#include "InitiateDownload.h"
#include "lassert.h"
#include "logging.h"
#include <string.h>

InitiateDownload::InitiateDownload()
    : ElementHeader(ElementHeader::InitiateDownload,
                    sizeof(InitiateDownload) - sizeof(ElementHeader)) {
}
bool InitiateDownload::Validate() const {
    static_assert(sizeof(InitiateDownload) == 4);
    return ElementHeader::GetElementType() == ElementHeader::InitiateDownload
        && ElementHeader::GetLength() == (sizeof(InitiateDownload) - sizeof(ElementHeader));
}
void InitiateDownload::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(InitiateDownload) <= raw_data->end);
    InitiateDownload *dst = (InitiateDownload *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(InitiateDownload);
}
InitiateDownload *InitiateDownload::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(InitiateDownload) > raw_data->end) {
        return nullptr;
    }

    auto res = (InitiateDownload *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += sizeof(InitiateDownload);
    return res;
}
uint16_t InitiateDownload::GetTotalLength() const {
    return GetLength() + sizeof(ElementHeader);
}

void InitiateDownload::Log() const {
    log_i("ME InitiateDownload");
}