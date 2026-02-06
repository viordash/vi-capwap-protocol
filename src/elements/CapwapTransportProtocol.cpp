#include "CapwapTransportProtocol.h"
#include "lassert.h"
#include "logging.h"

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
    CapwapTransportProtocol *dst = (CapwapTransportProtocol *)raw_data->current;
    *dst = *this;
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
uint16_t CapwapTransportProtocol::GetTotalLength() const {
    return GetLength() + sizeof(ElementHeader);
}

void CapwapTransportProtocol::Log() const {
    log_i("ME CapwapTransportProtocol Type:%u", (unsigned)type);
}