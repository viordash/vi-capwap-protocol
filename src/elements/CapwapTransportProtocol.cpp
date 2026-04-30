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
void CapwapTransportProtocol::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(CapwapTransportProtocol) <= raw_data->end);
#pragma GCC diagnostic push
#if __GNUC__ >= 8
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif
    std::memcpy(raw_data->current, this, sizeof(CapwapTransportProtocol));
#pragma GCC diagnostic pop

    raw_data->current += sizeof(CapwapTransportProtocol);
}
CapwapTransportProtocol *CapwapTransportProtocol::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(CapwapTransportProtocol) > raw_data->end) {
        return nullptr;
    }

    auto res = (CapwapTransportProtocol *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += sizeof(CapwapTransportProtocol);
    return res;
}
void CapwapTransportProtocol::Log() const {
    log_i("ME CapwapTransportProtocol Type:%u", (unsigned)type);
}

WritableCapwapTransportProtocol::WritableCapwapTransportProtocol(CapwapTransportProtocol::Type type)
    : element{ type } {
}

void WritableCapwapTransportProtocol::Serialize(RawData *raw_data) const {
    element.Serialize(raw_data);
}

void WritableCapwapTransportProtocol::Log() const {
    element.Log();
}

bool ReadableCapwapTransportProtocol::Deserialize(RawData *raw_data) {
    element = CapwapTransportProtocol::Deserialize(raw_data);
    is_present = element != nullptr;
    return is_present;
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