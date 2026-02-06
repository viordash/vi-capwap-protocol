#pragma once

#include "Helpers.h"
#include "elements/SessionId.h"
#include <cstdint>

struct __attribute__((packed)) DataChannelKeepAlive {
  public:
  private:
    NetworkU16 msgElementLength;
    SessionId session_id;

  public:
    const SessionId &GetSessionId() const;

    DataChannelKeepAlive(const DataChannelKeepAlive &) = delete;
    DataChannelKeepAlive(const AlignedSessionId &id);
    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static DataChannelKeepAlive *Deserialize(RawData *raw_data);
    void Log() const;
};