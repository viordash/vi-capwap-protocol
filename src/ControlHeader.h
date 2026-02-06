#pragma once

#include "Helpers.h"
#include <cstdint>

struct __attribute__((packed)) ControlHeader {
  public:
    enum MessageType : uint32_t {
        None = 0,
        DiscoveryRequest = 0x01000000,
        DiscoveryResponse = 0x02000000,
        JoinRequest = 0x03000000,
        JoinResponse = 0x04000000,
        ConfigurationStatusRequest = 0x05000000,
        ConfigurationStatusResponse = 0x06000000,
        ConfigurationUpdateRequest = 0x07000000,
        ConfigurationUpdateResponse = 0x08000000,
        WTPEventRequest = 0x09000000,
        WTPEventResponse = 0x0A000000,
        ChangeStateEventRequest = 0x0B000000,
        ChangeStateEventResponse = 0x0C000000,
        EchoRequest = 0x0D000000,
        EchoResponse = 0x0E000000,
        ImageDataRequest = 0x0F000000,
        ImageDataResponse = 0x10000000,
        ResetRequest = 0x11000000,
        ResetResponse = 0x12000000,
        PrimaryDiscoveryRequest = 0x13000000,
        PrimaryDiscoveryResponse = 0x14000000,
        DataTransferRequest = 0x15000000,
        DataTransferResponse = 0x16000000,
        ClearConfigurationRequest = 0x17000000,
        ClearConfigurationResponse = 0x18000000,
        StationConfigurationRequest = 0x19000000,
        StationConfigurationResponse = 0x1A000000,
    };

  private:
    MessageType type;
    uint8_t seqnum;
    NetworkU16 msgElementLength;
    uint8_t flags = 0;

  public:
    MessageType GetType() const;
    uint8_t GetSeqNum() const;
    uint16_t GetMsgElementLength() const;

    ControlHeader(const ControlHeader &) = delete;
    ControlHeader(MessageType type, uint8_t seqnum, uint16_t msgElementLength);
    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static ControlHeader *Deserialize(RawData *raw_data);
    void Log() const;
};