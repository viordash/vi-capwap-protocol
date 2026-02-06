#include "ControlHeader.h"
#include "lassert.h"
#include "logging.h"

ControlHeader::MessageType ControlHeader::GetType() const {
    return type;
}
uint8_t ControlHeader::GetSeqNum() const {
    return seqnum;
}
uint16_t ControlHeader::GetMsgElementLength() const {
    return msgElementLength.Get();
}

ControlHeader::ControlHeader(ControlHeader::MessageType type,
                             uint8_t seqnum,
                             uint16_t msgElementLength)
    : type{ type }, seqnum{ seqnum }, msgElementLength{ msgElementLength } {
}

bool ControlHeader::Validate() const {
    static_assert(sizeof(ControlHeader) == 8);
    return type >= DiscoveryRequest && type <= StationConfigurationResponse && flags == 0;
}

void ControlHeader::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(ControlHeader) <= raw_data->end);
    ControlHeader *dst = (ControlHeader *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(ControlHeader);
}
ControlHeader *ControlHeader::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ControlHeader) > raw_data->end) {
        return nullptr;
    }

    auto res = (ControlHeader *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += sizeof(ControlHeader);
    return res;
}

void ControlHeader::Log() const {
    log_i("Control header Type:%u, SeqNum:%u, MsgElementLength:%u",
          (unsigned)GetType(),
          GetSeqNum(),
          GetMsgElementLength());
}