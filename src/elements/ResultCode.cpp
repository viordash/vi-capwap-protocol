#include "ResultCode.h"
#include "lassert.h"
#include "logging.h"

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
void ResultCode::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(ResultCode) <= raw_data->end);
    ResultCode *dst = (ResultCode *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(ResultCode);
}
ResultCode *ResultCode::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ResultCode) > raw_data->end) {
        return nullptr;
    }

    auto res = (ResultCode *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += sizeof(ResultCode);
    return res;
}
uint16_t ResultCode::GetTotalLength() const {
    return GetLength() + sizeof(ElementHeader);
}

void ResultCode::Log() const {
    log_i("ME ResultCode Type: 0x%08X", (unsigned)type);
}