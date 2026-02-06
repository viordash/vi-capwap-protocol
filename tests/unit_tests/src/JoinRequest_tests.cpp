#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "JoinRequest.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(JoinRequestTestsGroup){ //
                                   TEST_SETUP(){}

                                   TEST_TEARDOWN(){}
};

TEST(JoinRequestTestsGroup, JoinRequest_serialize) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    WritableWTPBoardData::SubElement wtpboarddata_elements[] = {
        { BoardDataSubElementHeader::Type::WTPModelNumber, "abcd" },
        { BoardDataSubElementHeader::Type::WTPSerialNumber, "efghijklm" },
    };
    WritableWTPBoardData wtpboarddata{ 1234, wtpboarddata_elements };

    EncryptionSubElement wtpdescriptor_encr_elements[] = { { 0 } };

    WritableWTPDescriptor::SubElement wtpdescriptor_descr_elements[] = {
        { 1234, DescriptorSubElementHeader::Type::ActiveSoftwareVersion, "abcd" },
        { 5678, DescriptorSubElementHeader::Type::BootVersion, "efghijklm" },
        { 9012,
          DescriptorSubElementHeader::Type::OtherSoftwareVersion,
          "01234567890ABCDEF01234567890ABCDEF01234567890ABCDEF01234567890ABCDEF" },
        { 3456, DescriptorSubElementHeader::Type::HardwareVersion, "1" }
    };

    WritableWTPDescriptor wtpdescriptor{ 10,
                                         4,
                                         wtpdescriptor_encr_elements,
                                         wtpdescriptor_descr_elements };

    WTPRadioInformation radio_infos[] = {
        { 0, false, false, false, false },
        { 1, true, true, false, false },
        { 2, false, false, false, false },
        { 3, true, true, false, true },
    };

    CAPWAPLocalIPv4Address ip_addresses[] = { { inet_addr("192.168.100.10") } };

    CapwapTransportProtocol capwap_transport_protocol{ CapwapTransportProtocol::Type::UDP };
    MaximumMessageLength maximum_message_length{ 65000 };
    WTPRebootStatistics wtp_reboot_statistics{
        100, 101, 1001, 1002, 333, 444, 555, WTPRebootStatistics::LastFailureType::LinkFailure
    };

    const uint8_t id[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                           0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
    SessionId session_id;
    std::memcpy(session_id.session_id, id, sizeof(session_id.session_id));

    WTPFrameTunnelMode wtp_frame_tunnel_mode(true, false, false);

    WritableJoinRequest write_data("location_data",
                                   wtpboarddata,
                                   wtpdescriptor,
                                   "wtp_name",
                                   session_id,
                                   wtp_frame_tunnel_mode,
                                   WTPMACType::Local_MAC,
                                   radio_infos,
                                   ECNSupport::Type::FullAndLimitedECN,
                                   ip_addresses,
                                   &capwap_transport_protocol,
                                   &maximum_message_length,
                                   &wtp_reboot_statistics,
                                   VendorSpecificPayload::Dummy);

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 307 - (sizeof(ClearHeader) + sizeof(ControlHeader)), raw_data.current);
    const uint8_t reference[] = {
        0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x2A, 0x01, 0x23,
        0x00, 0x00, 0x1C, 0x00, 0x0D, 0x6C, 0x6F, 0x63, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x5F, 0x64,
        0x61, 0x74, 0x61, 0x00, 0x26, 0x00, 0x19, 0x00, 0x00, 0x04, 0xD2, 0x00, 0x00, 0x00, 0x04,
        0x61, 0x62, 0x63, 0x64, 0x00, 0x01, 0x00, 0x09, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B,
        0x6C, 0x6D, 0x00, 0x27, 0x00, 0x78, 0x0A, 0x04, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x04,
        0xD2, 0x00, 0x01, 0x00, 0x04, 0x61, 0x62, 0x63, 0x64, 0x00, 0x00, 0x16, 0x2E, 0x00, 0x02,
        0x00, 0x09, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x00, 0x00, 0x23, 0x34,
        0x00, 0x03, 0x00, 0x44, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30,
        0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
        0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
        0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x30, 0x31, 0x32, 0x33, 0x34,
        0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x00, 0x00, 0x0D,
        0x80, 0x00, 0x00, 0x00, 0x01, 0x31, 0x00, 0x2D, 0x00, 0x08, 0x77, 0x74, 0x70, 0x5F, 0x6E,
        0x61, 0x6D, 0x65, 0x00, 0x23, 0x00, 0x10, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x00, 0x29, 0x00, 0x01, 0x02, 0x00, 0x2C,
        0x00, 0x01, 0x00, 0x04, 0x18, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x18, 0x00,
        0x05, 0x01, 0x00, 0x00, 0x00, 0x03, 0x04, 0x18, 0x00, 0x05, 0x02, 0x00, 0x00, 0x00, 0x00,
        0x04, 0x18, 0x00, 0x05, 0x03, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x35, 0x00, 0x01, 0x01, 0x00,
        0x1E, 0x00, 0x04, 0xC0, 0xA8, 0x64, 0x0A, 0x00, 0x33, 0x00, 0x01, 0x02, 0x00, 0x1D, 0x00,
        0x02, 0xFD, 0xE8, 0x00, 0x30, 0x00, 0x0F, 0x00, 0x64, 0x00, 0x65, 0x03, 0xE9, 0x03, 0xEA,
        0x01, 0x4D, 0x01, 0xBC, 0x02, 0x2B, 0x02
    };
    MEMCMP_EQUAL(buffer,
                 reference + (sizeof(ClearHeader) + sizeof(ControlHeader)),
                 sizeof(reference) - (sizeof(ClearHeader) + sizeof(ControlHeader)));

    raw_data = { buffer, buffer + 307 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };
    ReadableJoinRequest read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(4, read_data.wtp_radio_informations.Get().size());
    CHECK_EQUAL(0, read_data.wtp_radio_informations.Get()[0]->RadioID);
    CHECK_FALSE(read_data.wtp_radio_informations.Get()[0]->B);
    CHECK_FALSE(read_data.wtp_radio_informations.Get()[0]->A);
    CHECK_FALSE(read_data.wtp_radio_informations.Get()[0]->G);
    CHECK_FALSE(read_data.wtp_radio_informations.Get()[0]->N);
    CHECK_EQUAL(1, read_data.wtp_radio_informations.Get()[1]->RadioID);
    CHECK_TRUE(read_data.wtp_radio_informations.Get()[1]->B);
    CHECK_TRUE(read_data.wtp_radio_informations.Get()[1]->A);
    CHECK_FALSE(read_data.wtp_radio_informations.Get()[1]->G);
    CHECK_FALSE(read_data.wtp_radio_informations.Get()[1]->N);
    CHECK_EQUAL(2, read_data.wtp_radio_informations.Get()[2]->RadioID);
    CHECK_FALSE(read_data.wtp_radio_informations.Get()[2]->B);
    CHECK_FALSE(read_data.wtp_radio_informations.Get()[2]->A);
    CHECK_FALSE(read_data.wtp_radio_informations.Get()[2]->G);
    CHECK_FALSE(read_data.wtp_radio_informations.Get()[2]->N);
    CHECK_EQUAL(3, read_data.wtp_radio_informations.Get()[3]->RadioID);
    CHECK_TRUE(read_data.wtp_radio_informations.Get()[3]->B);
    CHECK_TRUE(read_data.wtp_radio_informations.Get()[3]->A);
    CHECK_FALSE(read_data.wtp_radio_informations.Get()[3]->G);
    CHECK_TRUE(read_data.wtp_radio_informations.Get()[3]->N);
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(JoinRequestTestsGroup, JoinRequest_serialize_with_VendorSpecificPayload) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    WritableWTPBoardData::SubElement wtpboarddata_elements[] = {
        { BoardDataSubElementHeader::Type::WTPModelNumber, "abcd" },
    };
    WritableWTPBoardData wtpboarddata{ 1234, wtpboarddata_elements };

    EncryptionSubElement wtpdescriptor_encr_elements[] = { { 0 } };

    WritableWTPDescriptor::SubElement wtpdescriptor_descr_elements[] = {
        { 1234, DescriptorSubElementHeader::Type::ActiveSoftwareVersion, "abcd" }
    };

    WritableWTPDescriptor wtpdescriptor{ 10,
                                         4,
                                         wtpdescriptor_encr_elements,
                                         wtpdescriptor_descr_elements };

    WTPRadioInformation radio_infos[] = { { 0, false, false, false, false } };

    CAPWAPLocalIPv4Address ip_addresses[] = { { inet_addr("192.168.100.10") } };

    const uint8_t id[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                           0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
    SessionId session_id;
    std::memcpy(session_id.session_id, id, sizeof(session_id.session_id));

    WTPFrameTunnelMode wtp_frame_tunnel_mode(true, false, false);

    WritableVendorSpecificPayloadArray vendor_specific_payloads;
    vendor_specific_payloads.Add(123456, 789, "01234567890ABCDEF0123");
    vendor_specific_payloads.Add(1, 2, "01234567890A");

    WritableJoinRequest write_data("location_data",
                                   wtpboarddata,
                                   wtpdescriptor,
                                   "wtp_name",
                                   session_id,
                                   wtp_frame_tunnel_mode,
                                   WTPMACType::Local_MAC,
                                   radio_infos,
                                   ECNSupport::Type::FullAndLimitedECN,
                                   ip_addresses,
                                   nullptr,
                                   nullptr,
                                   nullptr,
                                   vendor_specific_payloads);

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 188 - (sizeof(ClearHeader) + sizeof(ControlHeader)), raw_data.current);
    const uint8_t reference[] = {
        0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x2A, 0x00, 0xAC,
        0x00, 0x00, 0x1C, 0x00, 0x0D, 0x6C, 0x6F, 0x63, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x5F, 0x64,
        0x61, 0x74, 0x61, 0x00, 0x26, 0x00, 0x0C, 0x00, 0x00, 0x04, 0xD2, 0x00, 0x00, 0x00, 0x04,
        0x61, 0x62, 0x63, 0x64, 0x00, 0x27, 0x00, 0x12, 0x0A, 0x04, 0x01, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x04, 0xD2, 0x00, 0x01, 0x00, 0x04, 0x61, 0x62, 0x63, 0x64, 0x00, 0x2D, 0x00, 0x08,
        0x77, 0x74, 0x70, 0x5F, 0x6E, 0x61, 0x6D, 0x65, 0x00, 0x23, 0x00, 0x10, 0x00, 0x01, 0x02,
        0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x00, 0x29,
        0x00, 0x01, 0x02, 0x00, 0x2C, 0x00, 0x01, 0x00, 0x04, 0x18, 0x00, 0x05, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x35, 0x00, 0x01, 0x01, 0x00, 0x1E, 0x00, 0x04, 0xC0, 0xA8, 0x64, 0x0A,
        0x00, 0x25, 0x00, 0x1B, 0x00, 0x01, 0xE2, 0x40, 0x03, 0x15, 0x30, 0x31, 0x32, 0x33, 0x34,
        0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x30, 0x31, 0x32,
        0x33, 0x00, 0x25, 0x00, 0x12, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x30, 0x31, 0x32, 0x33,
        0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41
    };
    MEMCMP_EQUAL(buffer,
                 reference + (sizeof(ClearHeader) + sizeof(ControlHeader)),
                 sizeof(reference) - (sizeof(ClearHeader) + sizeof(ControlHeader)));

    raw_data = { buffer, buffer + 188 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };
    ReadableJoinRequest read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(2, read_data.vendor_specific_payloads.Get().size());
    CHECK_EQUAL(123456, read_data.vendor_specific_payloads.Get()[0]->GetVendorIdentifier());
    CHECK_EQUAL(789, read_data.vendor_specific_payloads.Get()[0]->GetElementId());
    STRNCMP_EQUAL("01234567890ABCDEF0123",
                  (char *)read_data.vendor_specific_payloads.Get()[0]->value,
                  21);
    CHECK_EQUAL(1, read_data.vendor_specific_payloads.Get()[1]->GetVendorIdentifier());
    CHECK_EQUAL(2, read_data.vendor_specific_payloads.Get()[1]->GetElementId());
    STRNCMP_EQUAL("01234567890A", (char *)read_data.vendor_specific_payloads.Get()[1]->value, 12);
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(JoinRequestTestsGroup, JoinRequest_deserialize) {
    // clang-format off
    // Окончательно исправленный и верифицированный дамп для CAPWAP Join Request
    // Общая длина полезной нагрузки CAPWAP (заголовок + элементы): 8 + 8 + 140 = 156 байт
    uint8_t data[] = {
    // === Заголовок CAPWAP (8 байт) ===
        0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 
    //=========================================================================
    // 2. Control Header (8 байт)
    //-------------------------------------------------------------------------
    0x00, 0x00, 0x00, 0x03, // Message Type = 3 (Join Request)
    0x01,                   // Sequence Number = 1
    0x00, 0x69 + 5 + 6 + 19 + 11,             // Message Element Length = 105 байт + 5 + 6 + 19 + 11
    0x00,                   // Flags = 0

    //=========================================================================
    // 3. Message Elements (105 байт)
    //-------------------------------------------------------------------------

    // -- Location Data (MUST) -- (7 байт)
    0x00, 0x1C,             // Type = 28
    0x00, 0x03,             // Length = 3
    'L', 'a', 'b',          // "Lab"

    // -- WTP Board Data (MUST) -- (17 байт)
    0x00, 0x26,             // Type = 38
    0x00, 0x0D,             // Length = 13 (4 VendorID + 9 Model TLV)
    0x00, 0x00, 0x24, 0x3F, // Vendor ID = 9279 (Вымышленный)
    // Sub-element: Model Number (Type=0), Length=5
    0x00, 0x00, 0x00, 0x05, 'W', 'A', 'P', '-', '1',

    // -- WTP Descriptor (MUST) -- (19 байт)
    0x00, 0x27,             // Type = 39
    0x00, 0x0F,             // Length = 15 (1+1+1+3 + (4+4+1))
    0x01,                   // Max Radios = 1
    0x01,                   // Radios in use = 1
    0x01,                   // Num Encrypt = 1
    0x01, 0x00, 0x01,       // Encrypt Sub-elem: WBID=1, Caps=AES
    // Sub-element: SW Version (Type=1), Length=1
    0x00, 0x00, 0x24, 0x3F, // Vendor ID
    0x00, 0x01,             // Descriptor Type
    0x00, 0x01,             // Descriptor Length
    '1',                    // Value "1"

    // -- WTP Name (MUST) -- (10 байт)
    0x00, 0x2D,             // Type = 45
    0x00, 0x06,             // Length = 6
    'W', 'A', 'P', '-', '0', '1',

    // -- Session ID (MUST) -- (20 байт)
    0x00, 0x23,             // Type = 35
    0x00, 0x10,             // Length = 16
    0xDE, 0xC0, 0xAD, 0xDE, 0x12, 0x34, 0x56, 0x78,
    0x90, 0xAB, 0xCD, 0xEF, 0xFE, 0xED, 0xDA, 0xBE,

    // -- WTP Frame Tunnel Mode (MUST) -- (5 байт)
    0x00, 0x29,             // Type = 41
    0x00, 0x01,             // Length = 1
    0x02,                   // Value = 2 (Local Bridging)

    // -- WTP MAC Type (MUST) -- (5 байт)
    0x00, 0x2C,             // Type = 44
    0x00, 0x01,             // Length = 1
    0x00,                   // Value = 0 (Local MAC)

    // -- IEEE 802.11 WTP Radio Information (MUST) -- (9 байт)
    0x04, 0x18,             // Type = 1048
    0x00, 0x05,             // Length = 5
    0x01,                   // Radio ID = 1 (валидный)
    0x00, 0x00, 0x00, 0x04, // Radio Type = 4 (802.11g)

    // -- ECN Support (MUST) -- (5 байт)
    0x00, 0x35,             // Type = 53
    0x00, 0x01,             // Length = 1
    0x00,                   // Value = 0 (Limited Support)

    // -- CAPWAP Local IPv4 Address (MUST) -- (8 байт)
    0x00, 0x1E,             // Type = 30
    0x00, 0x04,             // Length = 4
    0xAC, 0x10, 0x01, 0x0A, // Value: 172.16.1.10

    // CapwapTransportProtocol
    0x00, 0x33, 0x00, 0x01, 0x01,

    // Maximum Message Length
    0x00, 0x1D, 0x00, 0x02, 0x40, 0x00, // Value = 16384 (в Big Endian)

    // WTP Reboot Statistics
    0x00, 0x30, 0x00, 0x0F, 
    // --- Value (15 байт) ---
    0x01, 0x2C,       // Reboot Count (сбои): 300 (0x012C)
    0x00, 0x04,       // AC Initiated Count: 4
    0x00, 0x0C,       // Link Failure Count: 12
    0x01, 0x2A,       // SW Failure Count: 298 (0x012A)
    0x00, 0x01,       // HW Failure Count: 1
    0x00, 0x00,       // Other Failure Count: 0
    0x00, 0x01,       // Unknown Failure Count: 1
    0x03,              // Last Failure Type: 3 (Software Failure)

    // Vendor Specific Payload (Тип: 37, Длина: 7)
    0x00, 0x25, 0x00, 0x07,
    0x00, 0x00, 0x00, 0x09, // Vendor ID: 9 (Cisco)
    0xDE, 0xAD, // Element ID: 0xDEAD
    0x01  // Data: 1 байт (например, флаг отладки)
    };
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableJoinRequest read_data;

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);

    STRNCMP_EQUAL("Lab", (char *)read_data.location_data->data, 3);

    CHECK_EQUAL(1, read_data.wtp_board_data.Get().size());
    CHECK_EQUAL(9279, read_data.wtp_board_data.header->GetVendorIdentifier());
    STRNCMP_EQUAL("WAP-1", (char *)read_data.wtp_board_data.Get()[0]->value, 5);
    CHECK_EQUAL(BoardDataSubElementHeader::Type::WTPModelNumber,
                read_data.wtp_board_data.Get()[0]->GetType());
    CHECK_EQUAL(5, read_data.wtp_board_data.Get()[0]->GetLength());

    CHECK_EQUAL(1, read_data.wtp_descriptor.header->MaxRadios);
    CHECK_EQUAL(1, read_data.wtp_descriptor.header->RadiosInUse);

    CHECK_EQUAL(1, read_data.wtp_descriptor.header->NumEncrypt);
    CHECK_EQUAL(1, read_data.wtp_descriptor.GetEncryptions().size());
    CHECK_EQUAL(WBIDType::IEEE_80211, read_data.wtp_descriptor.GetEncryptions()[0]->WBID);
    CHECK_EQUAL(0x0100, read_data.wtp_descriptor.GetEncryptions()[0]->EncryptionCapabilities);

    CHECK_EQUAL(1, read_data.wtp_descriptor.GetDescriptors().size());
    STRNCMP_EQUAL("1", (char *)read_data.wtp_descriptor.GetDescriptors()[0]->utf8_value, 1);
    CHECK_EQUAL(DescriptorSubElementHeader::Type::ActiveSoftwareVersion,
                read_data.wtp_descriptor.GetDescriptors()[0]->GetType());
    CHECK_EQUAL(1, read_data.wtp_descriptor.GetDescriptors()[0]->GetLength());

    STRNCMP_EQUAL("WAP-01", (char *)read_data.wtp_name->name, 6);

    const uint8_t session_id[] = { 0xDE, 0xC0, 0xAD, 0xDE, 0x12, 0x34, 0x56, 0x78,
                                   0x90, 0xAB, 0xCD, 0xEF, 0xFE, 0xED, 0xDA, 0xBE };
    MEMCMP_EQUAL(session_id, read_data.session_id->session_id, sizeof(session_id));

    CHECK_TRUE(read_data.wtp_frame_tunnel_mode->L);
    CHECK_FALSE(read_data.wtp_frame_tunnel_mode->E);
    CHECK_FALSE(read_data.wtp_frame_tunnel_mode->N);

    CHECK_EQUAL(WTPMACType::Type::Local_MAC, read_data.wtp_mac_type->type);

    CHECK_EQUAL(1, read_data.wtp_radio_informations.Get().size());
    CHECK_EQUAL(1, read_data.wtp_radio_informations.Get()[0]->RadioID);
    CHECK_FALSE(read_data.wtp_radio_informations.Get()[0]->B);
    CHECK_FALSE(read_data.wtp_radio_informations.Get()[0]->A);
    CHECK_TRUE(read_data.wtp_radio_informations.Get()[0]->G);
    CHECK_FALSE(read_data.wtp_radio_informations.Get()[0]->N);

    CHECK_EQUAL(ECNSupport::Type::LimitedECN, read_data.ecn_support->type);

    CHECK_EQUAL(1, read_data.ip_addresses.Get().size());
    CHECK_EQUAL(inet_addr("172.16.1.10"), read_data.ip_addresses.Get()[0]->GetIPAddress());

    CHECK_EQUAL(CapwapTransportProtocol::Type::UDPLite, read_data.capwap_transport_protocol->type);
    CHECK_EQUAL(16384, read_data.maximum_message_length->GetValue());

    CHECK_EQUAL(300, read_data.wtp_reboot_statistics->GetRebootCount());
    CHECK_EQUAL(4, read_data.wtp_reboot_statistics->GetACInitiatedCount());
    CHECK_EQUAL(12, read_data.wtp_reboot_statistics->GetLinkFailureCount());
    CHECK_EQUAL(298, read_data.wtp_reboot_statistics->GetSWFailureCount());
    CHECK_EQUAL(1, read_data.wtp_reboot_statistics->GetHWFailureCount());
    CHECK_EQUAL(0, read_data.wtp_reboot_statistics->GetOtherFailureCount());
    CHECK_EQUAL(1, read_data.wtp_reboot_statistics->GetUnknownFailureCount());
    CHECK_EQUAL(WTPRebootStatistics::LastFailureType::SoftwareFailure,
                read_data.wtp_reboot_statistics->GetLastFailureType());

    CHECK_EQUAL(1, read_data.vendor_specific_payloads.Get().size());
    CHECK_EQUAL(9, read_data.vendor_specific_payloads.Get()[0]->GetVendorIdentifier());
    CHECK_EQUAL(0xDEAD, read_data.vendor_specific_payloads.Get()[0]->GetElementId());
    CHECK_EQUAL(1,
                read_data.vendor_specific_payloads.Get()[0]->GetLength()
                    - (sizeof(VendorSpecificPayload) - sizeof(ElementHeader)));
    CHECK_EQUAL(1, read_data.vendor_specific_payloads.Get()[0]->value[0]);
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(JoinRequestTestsGroup, JoinRequest_deserialize_handle_unknown_element) {
    // clang-format off
    // Окончательно исправленный и верифицированный дамп для CAPWAP Join Request
    // Общая длина полезной нагрузки CAPWAP (заголовок + элементы): 8 + 8 + 140 = 156+5+5 байт
    uint8_t data[] = {
    // === Заголовок CAPWAP (8 байт) ===
        0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 
    //=========================================================================
    // 2. Control Header (8 байт)
    //-------------------------------------------------------------------------
    0x00, 0x00, 0x00, 0x03, // Message Type = 3 (Join Request)
    0x01,                   // Sequence Number = 1
    0x00, 0x69 + 5 + 6 + 19 + 11+5+5,             // Message Element Length = 105 байт + 5 + 6 + 19 + 11+5+5
    0x00,                   // Flags = 0

    //=========================================================================
    // 3. Message Elements (105+5+5 байт)
    //-------------------------------------------------------------------------

    // -- Location Data (MUST) -- (7 байт)
    0x00, 0x1C,             // Type = 28
    0x00, 0x03,             // Length = 3
    'L', 'a', 'b',          // "Lab"

    // -- WTP Board Data (MUST) -- (17 байт)
    0x00, 0x26,             // Type = 38
    0x00, 0x0D,             // Length = 13 (4 VendorID + 9 Model TLV)
    0x00, 0x00, 0x24, 0x3F, // Vendor ID = 9279 (Вымышленный)
    // Sub-element: Model Number (Type=0), Length=5
    0x00, 0x00, 0x00, 0x05, 'W', 'A', 'P', '-', '1',

    // -- WTP Descriptor (MUST) -- (19 байт)
    0x00, 0x27,             // Type = 39
    0x00, 0x0F,             // Length = 15 (1+1+1+3 + (4+4+1))
    0x01,                   // Max Radios = 1
    0x01,                   // Radios in use = 1
    0x01,                   // Num Encrypt = 1
    0x01, 0x00, 0x01,       // Encrypt Sub-elem: WBID=1, Caps=AES
    // Sub-element: SW Version (Type=1), Length=1
    0x00, 0x00, 0x24, 0x3F, // Vendor ID
    0x00, 0x01,             // Descriptor Type
    0x00, 0x01,             // Descriptor Length
    '1',                    // Value "1"

    // -- WTP Name (MUST) -- (10 байт)
    0x00, 0x2D,             // Type = 45
    0x00, 0x06,             // Length = 6
    'W', 'A', 'P', '-', '0', '1',

    // -- Session ID (MUST) -- (20 байт)
    0x00, 0x23,             // Type = 35
    0x00, 0x10,             // Length = 16
    0xDE, 0xC0, 0xAD, 0xDE, 0x12, 0x34, 0x56, 0x78,
    0x90, 0xAB, 0xCD, 0xEF, 0xFE, 0xED, 0xDA, 0xBE,

    // -- WTP Frame Tunnel Mode (MUST) -- (5 байт)
    0x00, 0x29,             // Type = 41
    0x00, 0x01,             // Length = 1
    0x02,                   // Value = 2 (Local Bridging)

    // -- WTP MAC Type (MUST) -- (5 байт)
    0x00, 0x2C,             // Type = 44
    0x00, 0x01,             // Length = 1
    0x00,                   // Value = 0 (Local MAC)

    // -- IEEE 802.11 WTP Radio Information (MUST) -- (9 байт)
    0x04, 0x18,             // Type = 1048
    0x00, 0x05,             // Length = 5
    0x01,                   // Radio ID = 1 (валидный)
    0x00, 0x00, 0x00, 0x04, // Radio Type = 4 (802.11g)

    // -- ECN Support (MUST) -- (5 байт)
    0x00, 0x35,             // Type = 53
    0x00, 0x01,             // Length = 1
    0x00,                   // Value = 0 (Limited Support)

    // -- CAPWAP Local IPv4 Address (MUST) -- (8 байт)
    0x00, 0x1E,             // Type = 30
    0x00, 0x04,             // Length = 4
    0xAC, 0x10, 0x01, 0x0A, // Value: 172.16.1.10

    // CapwapTransportProtocol
    0x00, 0x33, 0x00, 0x01, 0x01,

    // Maximum Message Length
    0x00, 0x1D, 0x00, 0x02, 0x40, 0x00, // Value = 16384 (в Big Endian)

    // WTP Reboot Statistics
    0x00, 0x30, 0x00, 0x0F, 
    // --- Value (15 байт) ---
    0x01, 0x2C,       // Reboot Count (сбои): 300 (0x012C)
    0x00, 0x04,       // AC Initiated Count: 4
    0x00, 0x0C,       // Link Failure Count: 12
    0x01, 0x2A,       // SW Failure Count: 298 (0x012A)
    0x00, 0x01,       // HW Failure Count: 1
    0x00, 0x00,       // Other Failure Count: 0
    0x00, 0x01,       // Unknown Failure Count: 1
    0x03,              // Last Failure Type: 3 (Software Failure)

    // Vendor Specific Payload (Тип: 37, Длина: 7)
    0x00, 0x25, 0x00, 0x07,
    0x00, 0x00, 0x00, 0x09, // Vendor ID: 9 (Cisco)
    0xDE, 0xAD, // Element ID: 0xDEAD
    0x01,  // Data: 1 байт (например, флаг отладки)
    //  Unknown (5 байт)
    0xFF, 0xFF, 0x00, 0x01, 0x00, 
    //  Unknown (5 байт)
    0xFF, 0xFE, 0x00, 0x01, 0x00, 
    };
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableJoinRequest read_data;

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, read_data.unknown_elements);
}