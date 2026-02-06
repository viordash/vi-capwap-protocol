#pragma once

#include "ControlHeader.h"
#include "Helpers.h"

struct CapwapMessage {
    virtual ControlHeader::MessageType GetMessageType() const = 0;
};

struct WritableCapwapMessage : CapwapMessage {
    virtual void Serialize(RawData *raw_data) const = 0;
};
struct WritableCapwapRequest : WritableCapwapMessage {
    virtual ControlHeader::MessageType GetResponseMessageType() const = 0;
};
struct WritableCapwapResponse : WritableCapwapMessage {
    virtual ControlHeader::MessageType GetRequestMessageType() const = 0;
};

struct ReadableCapwapMessage : CapwapMessage {
    virtual bool Deserialize(RawData *raw_data) = 0;
};
struct ReadableCapwapRequest : ReadableCapwapMessage {};
struct ReadableCapwapResponse : ReadableCapwapMessage {};