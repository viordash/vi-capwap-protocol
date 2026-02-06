#pragma once

#include "Helpers.h"
#include <cstdint>

struct __attribute__((packed)) ElementHeader {
  public:
    enum ElementType : uint16_t {
        ACDescriptor = 0x0100,       // 1
        ACIPv4List = 0x0200,         // 2
        ACIPv6List = 0x0300,         // 3
        ACName = 0x0400,             // 4
        ACNameWithPriority = 0x0500, // 5
        ACTimestamp = 0x0600,        // 6
        AddMACACLEntry = 0x0700,     // 7
        AddStation = 0x0800,         // 8
        // Reserved                    = 9
        CAPWAPControlIPv4Address = 0x0A00,    // 10
        CAPWAPControlIPV6Address = 0x0B00,    // 11
        CAPWAPTimers = 0x0C00,                // 12
        DataTransferData = 0x0D00,            // 13
        DataTransferMode = 0x0E00,            // 14
        DecryptionErrorReport = 0x0F00,       // 15
        DecryptionErrorReportPeriod = 0x1000, // 16
        DeleteMACACLEntry = 0x1100,           // 17
        DeleteStation = 0x1200,               // 18
        // Reserved                    = 19
        DiscoveryType = 0x1400,            // 20
        DuplicateIPv4Address = 0x1500,     // 21
        DuplicateIPv6Address = 0x1600,     // 22
        IdleTimeout = 0x1700,              // 23
        ImageData = 0x1800,                // 24
        ImageIdentifier = 0x1900,          // 25
        ImageInformation = 0x1A00,         // 26
        InitiateDownload = 0x1B00,         // 27
        LocationData = 0x1C00,             // 28
        MaximumMessageLength = 0x1D00,     // 29
        CAPWAPLocalIPv4Address = 0x1E00,   // 30
        RadioAdministrativeState = 0x1F00, // 31
        RadioOperationalState = 0x2000,    // 32
        ResultCode = 0x2100,               // 33
        ReturnedMessageElement = 0x2200,   // 34
        SessionID = 0x2300,                // 35
        StatisticsTimer = 0x2400,          // 36
        VendorSpecificPayload = 0x2500,    // 37
        WTPBoardData = 0x2600,             // 38
        WTPDescriptor = 0x2700,            // 39
        WTPFallback = 0x2800,              // 40
        WTPFrameTunnelMode = 0x2900,       // 41
        // Reserved                    = 42
        // Reserved                    = 43
        WTPMACType = 0x2C00, // 44
        WTPName = 0x2D00,    // 45
        // Unused/Reserved             = 46
        WTPRadioStatistics = 0x2F00,            // 47
        WTPRebootStatistics = 0x3000,           // 48
        WTPStaticIPAddressInformation = 0x3100, // 49
        CAPWAPLocalIPV6Address = 0x3200,        // 50
        CAPWAPTransportProtocol = 0x3300,       // 51
        MTUDiscoveryPadding = 0x3400,           // 52
        ECNSupport = 0x3500,                    // 53
        WTPRadioInformation = 0x1804            // 1048
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