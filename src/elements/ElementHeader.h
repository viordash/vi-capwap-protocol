#pragma once

#include "Helpers.h"
#include <cstdint>

struct __attribute__((packed)) ElementHeader {
  public:
    enum ElementType : uint16_t {
        ACDescriptor = ToNetworkOrder16(1),
        ACIPv4List = ToNetworkOrder16(2),
        ACIPv6List = ToNetworkOrder16(3),
        ACName = ToNetworkOrder16(4),
        ACNameWithPriority = ToNetworkOrder16(5),
        ACTimestamp = ToNetworkOrder16(6),
        AddMACACLEntry = ToNetworkOrder16(7),
        AddStation = ToNetworkOrder16(8),
        // Reserved                    = 9
        CAPWAPControlIPv4Address = ToNetworkOrder16(10),
        CAPWAPControlIPV6Address = ToNetworkOrder16(11),
        CAPWAPTimers = ToNetworkOrder16(12),
        DataTransferData = ToNetworkOrder16(13),
        DataTransferMode = ToNetworkOrder16(14),
        DecryptionErrorReport = ToNetworkOrder16(15),
        DecryptionErrorReportPeriod = ToNetworkOrder16(16),
        DeleteMACACLEntry = ToNetworkOrder16(17),
        DeleteStation = ToNetworkOrder16(18),
        // Reserved                    = 19
        DiscoveryType = ToNetworkOrder16(20),
        DuplicateIPv4Address = ToNetworkOrder16(21),
        DuplicateIPv6Address = ToNetworkOrder16(22),
        IdleTimeout = ToNetworkOrder16(23),
        ImageData = ToNetworkOrder16(24),
        ImageIdentifier = ToNetworkOrder16(25),
        ImageInformation = ToNetworkOrder16(26),
        InitiateDownload = ToNetworkOrder16(27),
        LocationData = ToNetworkOrder16(28),
        MaximumMessageLength = ToNetworkOrder16(29),
        CAPWAPLocalIPv4Address = ToNetworkOrder16(30),
        RadioAdministrativeState = ToNetworkOrder16(31),
        RadioOperationalState = ToNetworkOrder16(32),
        ResultCode = ToNetworkOrder16(33),
        ReturnedMessageElement = ToNetworkOrder16(34),
        SessionID = ToNetworkOrder16(35),
        StatisticsTimer = ToNetworkOrder16(36),
        VendorSpecificPayload = ToNetworkOrder16(37),
        WTPBoardData = ToNetworkOrder16(38),
        WTPDescriptor = ToNetworkOrder16(39),
        WTPFallback = ToNetworkOrder16(40),
        WTPFrameTunnelMode = ToNetworkOrder16(41),
        // Reserved                    = 42
        // Reserved                    = 43
        WTPMACType = ToNetworkOrder16(44),
        WTPName = ToNetworkOrder16(45),
        // Unused/Reserved             = 46
        WTPRadioStatistics = ToNetworkOrder16(47),
        WTPRebootStatistics = ToNetworkOrder16(48),
        WTPStaticIPAddressInformation = ToNetworkOrder16(49),
        CAPWAPLocalIPV6Address = ToNetworkOrder16(50),
        CAPWAPTransportProtocol = ToNetworkOrder16(51),
        MTUDiscoveryPadding = ToNetworkOrder16(52),
        ECNSupport = ToNetworkOrder16(53),
        AddWlan = ToNetworkOrder16(1024),
        Antenna = ToNetworkOrder16(1025),
        AssignedWtpBssid = ToNetworkOrder16(1026),
        DeleteWlan = ToNetworkOrder16(1027),
        DirectSequenceControl = ToNetworkOrder16(1028),
        InformationElement = ToNetworkOrder16(1029),
        MACOperation = ToNetworkOrder16(1030),
        MICCountermeasures = ToNetworkOrder16(1031),
        MultiDomainCapability = ToNetworkOrder16(1032),
        OFDMControl = ToNetworkOrder16(1033),
        RateSet = ToNetworkOrder16(1034),
        RSNAErrorReportFromStation = ToNetworkOrder16(1035),
        Station = ToNetworkOrder16(1036),
        StationQoSProfile = ToNetworkOrder16(1037),
        StationSessionKey = ToNetworkOrder16(1038),
        Statistics = ToNetworkOrder16(1039),
        SupportedRates = ToNetworkOrder16(1040),
        TxPower = ToNetworkOrder16(1041),
        TxPowerLevel = ToNetworkOrder16(1042),
        UpdateStationQoS = ToNetworkOrder16(1043),
        UpdateWlan = ToNetworkOrder16(1044),
        WTPQualityOfService = ToNetworkOrder16(1045),
        WTPRadioConfiguration = ToNetworkOrder16(1046),
        WTPRadioFailAlarmIndication = ToNetworkOrder16(1047),
        WTPRadioInformation = ToNetworkOrder16(1048)
    };

  private:
    // identifies the information carried in the Value field
    ElementType element_type;
    // indicates the number of bytes in the Value field
    NetworkU16 element_length;

  public:
    ElementType GetElementType() const;
    uint16_t GetLength() const;

    ElementHeader(const ElementHeader &) = default;
    ElementHeader(ElementType element_type, uint16_t element_length);
};