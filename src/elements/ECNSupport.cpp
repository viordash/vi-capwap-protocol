#include "ECNSupport.h"
#include "Logging.h"
#include "lassert.h"
#include <cstring>

ECNSupport::ECNSupport(Type type)
    : ElementHeader(ElementHeader::ECNSupport, sizeof(ECNSupport) - sizeof(ElementHeader)),
      type{ type } {
}

bool ECNSupport::Validate() const {
    static_assert(sizeof(ECNSupport) == 5);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
    return GetElementType() == ElementHeader::ECNSupport
        && GetLength() == (sizeof(ECNSupport) - sizeof(ElementHeader)) //
        && type >= LimitedECN && type <= FullAndLimitedECN;
#pragma GCC diagnostic pop
}

void ECNSupport::Log() const {
    log_i("ME ECNSupport Type:{}", (unsigned)type);
}

WritableECNSupport::WritableECNSupport(ECNSupport::Type type) : element{ type } {
    static_assert(sizeof(element) == 5);
}

void WritableECNSupport::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(ECNSupport) <= raw_data->end);
    std::memcpy(raw_data->current, &element, sizeof(element));
    raw_data->current += sizeof(element);
}

void WritableECNSupport::Log() const {
    element.Log();
}

bool ReadableECNSupport::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ECNSupport) > raw_data->end) {
        return false;
    }

    auto res = (ECNSupport *)raw_data->current;
    if (!res->Validate()) {
        return false;
    }
    raw_data->current += sizeof(ECNSupport);

    element = res;
    is_present = true;
    return true;
}

const ECNSupport *ReadableECNSupport::Get() const {
    return element;
}

void ReadableECNSupport::Log() const {
    ASSERT(element != nullptr);
    element->Log();
}

ElementHeader::ElementType ReadableECNSupport::GetElementType() const {
    return ElementHeader::ECNSupport;
}

bool ReadableECNSupport::IsPresent() const {
    return is_present;
}