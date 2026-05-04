#include "CapwapTransportProtocol.h"
#include "Logging.h"
#include "lassert.h"
#include <cstring>

CapwapTransportProtocol::CapwapTransportProtocol(Type type)
    : ElementHeader(ElementHeader::CAPWAPTransportProtocol,
                    sizeof(CapwapTransportProtocol) - sizeof(ElementHeader)),
      type{ type } {
}
bool CapwapTransportProtocol::Validate() const {
    static_assert(sizeof(CapwapTransportProtocol) == 5);
    return ElementHeader::GetElementType() == ElementHeader::CAPWAPTransportProtocol
        && ElementHeader::GetLength()
               == (sizeof(CapwapTransportProtocol) - sizeof(ElementHeader)) //
        && type >= UDPLite && type <= UDP;
}

void CapwapTransportProtocol::Log() const {
    log_i("ME CapwapTransportProtocol Type:%u", (unsigned)type);
}

WritableCapwapTransportProtocol::WritableCapwapTransportProtocol(CapwapTransportProtocol::Type type)
    : element{ type } {
    static_assert(sizeof(element) == 5);
}

void WritableCapwapTransportProtocol::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(CapwapTransportProtocol) <= raw_data->end);
    std::memcpy(raw_data->current, &element, sizeof(element));
    raw_data->current += sizeof(element);
}

void WritableCapwapTransportProtocol::Log() const {
    element.Log();
}

bool ReadableCapwapTransportProtocol::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(CapwapTransportProtocol) > raw_data->end) {
        return false;
    }

    auto res = (CapwapTransportProtocol *)raw_data->current;
    if (!res->Validate()) {
        return false;
    }
    raw_data->current += sizeof(CapwapTransportProtocol);

    element = res;
    is_present = true;
    return true;
}

const CapwapTransportProtocol *const ReadableCapwapTransportProtocol::Get() const {
    return element;
}

void ReadableCapwapTransportProtocol::Log() const {
    ASSERT(element != nullptr);
    element->Log();
}

ElementHeader::ElementType ReadableCapwapTransportProtocol::GetElementType() const {
    return ElementHeader::CAPWAPTransportProtocol;
}

bool ReadableCapwapTransportProtocol::IsPresent() const {
    return is_present;
}