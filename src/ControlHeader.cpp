#include "ControlHeader.h"
#include "Logging.h"
#include "lassert.h"

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
    if (flags != 0) {
        return false;
    }
    switch (type) {
        case DiscoveryRequest:
        case DiscoveryResponse:
        case JoinRequest:
        case JoinResponse:
        case ConfigurationStatusRequest:
        case ConfigurationStatusResponse:
        case ConfigurationUpdateRequest:
        case ConfigurationUpdateResponse:
        case WTPEventRequest:
        case WTPEventResponse:
        case ChangeStateEventRequest:
        case ChangeStateEventResponse:
        case EchoRequest:
        case EchoResponse:
        case ImageDataRequest:
        case ImageDataResponse:
        case ResetRequest:
        case ResetResponse:
        case PrimaryDiscoveryRequest:
        case PrimaryDiscoveryResponse:
        case DataTransferRequest:
        case DataTransferResponse:
        case ClearConfigurationRequest:
        case ClearConfigurationResponse:
        case StationConfigurationRequest:
        case StationConfigurationResponse:
        case WlanConfigurationRequest:
        case WlanConfigurationResponse:
            return true;
        default:
            break;
    }
    return false;
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
    log_i("Control header Type:{}, SeqNum:{}, MsgElementLength:{}",
          (unsigned)GetType(),
          GetSeqNum(),
          GetMsgElementLength());
}