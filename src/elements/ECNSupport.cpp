#include "ECNSupport.h"
#include "lassert.h"
#include "logging.h"

ECNSupport::ECNSupport(Type type)
    : ElementHeader(ElementHeader::ECNSupport, sizeof(ECNSupport) - sizeof(ElementHeader)),
      type{ type } {
}
bool ECNSupport::Validate() const {
    static_assert(sizeof(ECNSupport) == 5);
    return ElementHeader::GetElementType() == ElementHeader::ECNSupport
        && ElementHeader::GetLength() == (sizeof(ECNSupport) - sizeof(ElementHeader)) //
        && type >= LimitedECN && type <= FullAndLimitedECN;
}
void ECNSupport::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(ECNSupport) <= raw_data->end);
    ECNSupport *dst = (ECNSupport *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(ECNSupport);
}
ECNSupport *ECNSupport::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ECNSupport) > raw_data->end) {
        return nullptr;
    }

    auto res = (ECNSupport *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += sizeof(ECNSupport);
    return res;
}
uint16_t ECNSupport::GetTotalLength() const {
    return GetLength() + sizeof(ElementHeader);
}

void ECNSupport::Log() const {
    log_i("ME ECNSupport Type:%u", (unsigned)type);
}