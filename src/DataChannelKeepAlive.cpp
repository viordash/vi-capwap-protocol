#include "DataChannelKeepAlive.h"
#include "lassert.h"
#include "logging.h"

const SessionId &DataChannelKeepAlive::GetSessionId() const {
    return session_id;
}

DataChannelKeepAlive::DataChannelKeepAlive(const AlignedSessionId &id)
    : msgElementLength{ (uint16_t)sizeof(session_id) }, session_id{ id } {
}

bool DataChannelKeepAlive::Validate() const {
    static_assert(sizeof(DataChannelKeepAlive) == 22);
    return msgElementLength.Get() == sizeof(session_id) && session_id.Validate();
}

void DataChannelKeepAlive::Serialize(RawData *raw_data) const {
    ASSERT(msgElementLength.Get() == sizeof(session_id))
    ASSERT(raw_data->current + sizeof(DataChannelKeepAlive) <= raw_data->end);
    DataChannelKeepAlive *dst = (DataChannelKeepAlive *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(DataChannelKeepAlive);
}

DataChannelKeepAlive *DataChannelKeepAlive::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(DataChannelKeepAlive) > raw_data->end) {
        return nullptr;
    }

    auto res = (DataChannelKeepAlive *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += sizeof(DataChannelKeepAlive);
    return res;
}

void DataChannelKeepAlive::Log() const {
    log_i("Control MsgElementLength:%u", msgElementLength.Get());
    session_id.Log();
}