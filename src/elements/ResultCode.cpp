#include "ResultCode.h"
#include "Logging.h"
#include "lassert.h"
#include <cstring>

ResultCode::ResultCode(Type type)
    : ElementHeader(ElementHeader::ResultCode, sizeof(ResultCode) - sizeof(ElementHeader)),
      type{ type } {
}

bool ResultCode::Validate() const {
    static_assert(sizeof(ResultCode) == 8);
    return ElementHeader::GetElementType() == ElementHeader::ResultCode
        && ElementHeader::GetLength() == (sizeof(ResultCode) - sizeof(ElementHeader)) //
        && type >= Type::Success && type <= Type::DataTransferError;
}

void ResultCode::Log() const {
    log_i("ME ResultCode Type: 0x{:08X}", (unsigned)type);
}

WritableResultCode::WritableResultCode(ResultCode::Type type) : element{ type } {
    static_assert(sizeof(element) == 8);
}

void WritableResultCode::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(ResultCode) <= raw_data->end);
    std::memcpy(raw_data->current, &element, sizeof(element));
    raw_data->current += sizeof(element);
}

void WritableResultCode::Log() const {
    element.Log();
}

bool ReadableResultCode::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ResultCode) > raw_data->end) {
        return false;
    }

    auto res = (ResultCode *)raw_data->current;
    if (!res->Validate()) {
        return false;
    }
    raw_data->current += sizeof(ResultCode);

    element = res;
    is_present = true;
    return true;
}

const ResultCode *ReadableResultCode::Get() const {
    return element;
}

void ReadableResultCode::Log() const {
    ASSERT(element != nullptr);
    element->Log();
}

ElementHeader::ElementType ReadableResultCode::GetElementType() const {
    return ElementHeader::ResultCode;
}

bool ReadableResultCode::IsPresent() const {
    return is_present;
}