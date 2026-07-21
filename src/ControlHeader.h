#pragma once

#include "Helpers.h"
#include <cstdint>

struct __attribute__((packed)) ControlHeader {
  public:
    enum MessageType : uint32_t {
        None = 0,
        DiscoveryRequest = ToNetworkOrder32(1),
        DiscoveryResponse = ToNetworkOrder32(2),
        JoinRequest = ToNetworkOrder32(3),
        JoinResponse = ToNetworkOrder32(4),
        ConfigurationStatusRequest = ToNetworkOrder32(5),
        ConfigurationStatusResponse = ToNetworkOrder32(6),
        ConfigurationUpdateRequest = ToNetworkOrder32(7),
        ConfigurationUpdateResponse = ToNetworkOrder32(8),
        WTPEventRequest = ToNetworkOrder32(9),
        WTPEventResponse = ToNetworkOrder32(10),
        ChangeStateEventRequest = ToNetworkOrder32(11),
        ChangeStateEventResponse = ToNetworkOrder32(12),
        EchoRequest = ToNetworkOrder32(13),
        EchoResponse = ToNetworkOrder32(14),
        ImageDataRequest = ToNetworkOrder32(15),
        ImageDataResponse = ToNetworkOrder32(16),
        ResetRequest = ToNetworkOrder32(17),
        ResetResponse = ToNetworkOrder32(18),
        PrimaryDiscoveryRequest = ToNetworkOrder32(19),
        PrimaryDiscoveryResponse = ToNetworkOrder32(20),
        DataTransferRequest = ToNetworkOrder32(21),
        DataTransferResponse = ToNetworkOrder32(22),
        ClearConfigurationRequest = ToNetworkOrder32(23),
        ClearConfigurationResponse = ToNetworkOrder32(24),
        StationConfigurationRequest = ToNetworkOrder32(25),
        StationConfigurationResponse = ToNetworkOrder32(26),
        WlanConfigurationRequest = ToNetworkOrder32(3398913),
        WlanConfigurationResponse = ToNetworkOrder32(3398914),
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