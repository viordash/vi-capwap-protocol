#include "InitiateDownload.h"
#include "Logging.h"
#include "lassert.h"
#include <cstring>

InitiateDownload::InitiateDownload()
    : ElementHeader(ElementHeader::InitiateDownload,
                    sizeof(InitiateDownload) - sizeof(ElementHeader)) {
}

bool InitiateDownload::Validate() const {
    static_assert(sizeof(InitiateDownload) == 4);
    return ElementHeader::GetElementType() == ElementHeader::InitiateDownload
        && ElementHeader::GetLength() == (sizeof(InitiateDownload) - sizeof(ElementHeader));
}

void InitiateDownload::Log() const {
    log_i("ME InitiateDownload");
}

void WritableInitiateDownload::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(InitiateDownload) <= raw_data->end);
    std::memcpy(raw_data->current, &element, sizeof(element));
    raw_data->current += sizeof(element);
}

void WritableInitiateDownload::Log() const {
    element.Log();
}

bool ReadableInitiateDownload::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(InitiateDownload) > raw_data->end) {
        return false;
    }

    auto res = (InitiateDownload *)raw_data->current;
    if (!res->Validate()) {
        return false;
    }
    raw_data->current += sizeof(InitiateDownload);

    element = res;
    is_present = true;
    return true;
}

const InitiateDownload *const ReadableInitiateDownload::Get() const {
    return element;
}

void ReadableInitiateDownload::Log() const {
    ASSERT(element != nullptr);
    element->Log();
}

ElementHeader::ElementType ReadableInitiateDownload::GetElementType() const {
    return ElementHeader::InitiateDownload;
}

bool ReadableInitiateDownload::IsPresent() const {
    return is_present;
}