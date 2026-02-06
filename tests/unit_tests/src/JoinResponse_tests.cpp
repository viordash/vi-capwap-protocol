#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "JoinResponse.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(JoinResponseTestsGroup){ //
                                    TEST_SETUP(){}

                                    TEST_TEARDOWN(){}
};

TEST(JoinResponseTestsGroup, JoinResponse_serialize) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    WritableACDescriptor::SubElement info_elements[] = {
        { 1234,
          ACInformationSubElementHeader::Type::HardwareVersion,
          "01234567890ABCDEF01234567890ABCDEF01234567890ABCDEF01234567890ABCDEF" },
        { 5678, ACInformationSubElementHeader::Type::SoftwareVersion, "efghijklm" },
    };

    WritableACDescriptor ac_descriptor{
        100,  200,   1000,         1234, true, false, ACDescriptorHeader::RMACField::Supported,
        true, false, info_elements
    };

    WTPRadioInformation radio_infos[] = {
        { 10, false, false, false, false },
        { 1, true, true, false, false },
        { 2, false, false, false, false },
    };

    CAPWAPControlIPv4Address control_ip_addresses[] = { { inet_addr("192.168.100.10"), 19 },
                                                        { inet_addr("192.168.100.11"), 20 } };

    CAPWAPLocalIPv4Address local_ip_addresses[] = { { inet_addr("192.168.100.10") } };

    uint32_t addresses[] = {
        { inet_addr("192.168.1.110") },
        { inet_addr("192.168.1.111") },
    };

    WritableACIPv4List ac_ipv4_list{
        addresses,
    };
    WritableImageIdentifier image_identifier{ 1232344, "1232344" };
    MaximumMessageLength maximum_message_length(4219);

    CapwapTransportProtocol capwap_transport_protocol{ CapwapTransportProtocol::Type::UDP };

    WritableJoinResponse write_data(ResultCode::Type::Success,
                                    ac_descriptor,
                                    "Corporate-AC-1",
                                    radio_infos,
                                    ECNSupport::Type::FullAndLimitedECN,
                                    control_ip_addresses,
                                    local_ip_addresses,
                                    &ac_ipv4_list,
                                    &capwap_transport_protocol,
                                    &image_identifier,
                                    &maximum_message_length,
                                    VendorSpecificPayload::Dummy);

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 249 - (sizeof(ClearHeader) + sizeof(ControlHeader)), raw_data.current);
    const uint8_t reference[] = {
        0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x2A, 0x00, 0xE9,
        0x00, 0x00, 0x21, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x69, 0x00, 0x64,
        0x00, 0xC8, 0x03, 0xE8, 0x04, 0xD2, 0x02, 0x01, 0x00, 0x02, 0x00, 0x00, 0x04, 0xD2, 0x00,
        0x04, 0x00, 0x44, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41,
        0x42, 0x43, 0x44, 0x45, 0x46, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
        0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
        0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x00, 0x00, 0x16, 0x2E,
        0x00, 0x05, 0x00, 0x09, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x00, 0x04,
        0x00, 0x0E, 0x43, 0x6F, 0x72, 0x70, 0x6F, 0x72, 0x61, 0x74, 0x65, 0x2D, 0x41, 0x43, 0x2D,
        0x31, 0x04, 0x18, 0x00, 0x05, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x04, 0x18, 0x00, 0x05, 0x01,
        0x00, 0x00, 0x00, 0x03, 0x04, 0x18, 0x00, 0x05, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x35,
        0x00, 0x01, 0x01, 0x00, 0x0A, 0x00, 0x06, 0xC0, 0xA8, 0x64, 0x0A, 0x00, 0x13, 0x00, 0x0A,
        0x00, 0x06, 0xC0, 0xA8, 0x64, 0x0B, 0x00, 0x14, 0x00, 0x1E, 0x00, 0x04, 0xC0, 0xA8, 0x64,
        0x0A, 0x00, 0x02, 0x00, 0x08, 0xC0, 0xA8, 0x01, 0x6E, 0xC0, 0xA8, 0x01, 0x6F, 0x00, 0x33,
        0x00, 0x01, 0x02, 0x00, 0x19, 0x00, 0x0B, 0x00, 0x12, 0xCD, 0xD8, 0x31, 0x32, 0x33, 0x32,
        0x33, 0x34, 0x34, 0x00, 0x1D, 0x00, 0x02, 0x10, 0x7B
    };

    MEMCMP_EQUAL(buffer,
                 reference + (sizeof(ClearHeader) + sizeof(ControlHeader)),
                 sizeof(reference) - (sizeof(ClearHeader) + sizeof(ControlHeader)));

    raw_data = { buffer, buffer + 249 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };
    ReadableJoinResponse read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(ResultCode::Type::Success, read_data.result_code->type);

    CHECK_EQUAL(100, read_data.ac_descriptor.header->GetStations());
    CHECK_EQUAL(200, read_data.ac_descriptor.header->GetLimit());
    CHECK_EQUAL(1000, read_data.ac_descriptor.header->GetActiveWTPs());
    CHECK_EQUAL(1234, read_data.ac_descriptor.header->GetMaxWTPs());
    CHECK_TRUE(read_data.ac_descriptor.header->X509Cert());
    CHECK_FALSE(read_data.ac_descriptor.header->PresharedSecret());
    CHECK_EQUAL(ACDescriptorHeader::RMACField::Supported,
                read_data.ac_descriptor.header->GetRMAC());
    CHECK_EQUAL(ACDescriptorHeader::RMACField::Supported,
                read_data.ac_descriptor.header->GetRMAC());
    CHECK_TRUE(read_data.ac_descriptor.header->ClearDataChannel());
    CHECK_FALSE(read_data.ac_descriptor.header->DTLSDataChannel());
    CHECK_EQUAL(2, read_data.ac_descriptor.Get().size());

    STRNCMP_EQUAL("01234567890ABCDEF01234567890ABCDEF01234567890ABCDEF01234567890ABCDEF",
                  (char *)read_data.ac_descriptor.Get()[0]->data,
                  68);
    CHECK_EQUAL(ACInformationSubElementHeader::Type::HardwareVersion,
                read_data.ac_descriptor.Get()[0]->GetType());
    CHECK_EQUAL(68, read_data.ac_descriptor.Get()[0]->GetLength());
    CHECK_EQUAL(1234, read_data.ac_descriptor.Get()[0]->GetVendorIdentifier());

    STRNCMP_EQUAL("efghijklm", (char *)read_data.ac_descriptor.Get()[1]->data, 9);
    CHECK_EQUAL(ACInformationSubElementHeader::Type::SoftwareVersion,
                read_data.ac_descriptor.Get()[1]->GetType());
    CHECK_EQUAL(9, read_data.ac_descriptor.Get()[1]->GetLength());
    CHECK_EQUAL(5678, read_data.ac_descriptor.Get()[1]->GetVendorIdentifier());

    STRNCMP_EQUAL("Corporate-AC-1", (char *)read_data.ac_name->name, 14);

    CHECK_EQUAL(3, read_data.wtp_radio_informations.Get().size());
    CHECK_EQUAL(10, read_data.wtp_radio_informations.Get()[0]->RadioID);
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

    CHECK_EQUAL(ECNSupport::Type::FullAndLimitedECN, read_data.ecn_support->type);

    CHECK_EQUAL(2, read_data.control_ip_addresses.Get().size());
    CHECK_EQUAL(inet_addr("192.168.100.10"),
                read_data.control_ip_addresses.Get()[0]->GetIPAddress());
    CHECK_EQUAL(19, read_data.control_ip_addresses.Get()[0]->GetWTPCount());
    CHECK_EQUAL(inet_addr("192.168.100.11"),
                read_data.control_ip_addresses.Get()[1]->GetIPAddress());
    CHECK_EQUAL(20, read_data.control_ip_addresses.Get()[1]->GetWTPCount());

    CHECK_EQUAL(1, read_data.local_ip_addresses.Get().size());
    CHECK_EQUAL(inet_addr("192.168.100.10"), read_data.local_ip_addresses.Get()[0]->GetIPAddress());

    CHECK(read_data.ac_ipv4_list != nullptr);
    CHECK_EQUAL(2, read_data.ac_ipv4_list->GetCount());
    CHECK_EQUAL(inet_addr("192.168.1.110"), read_data.ac_ipv4_list->addresses[0]);
    CHECK_EQUAL(inet_addr("192.168.1.111"), read_data.ac_ipv4_list->addresses[1]);

    CHECK(read_data.capwap_transport_protocol != nullptr);
    CHECK_EQUAL(CapwapTransportProtocol::Type::UDP, read_data.capwap_transport_protocol->type);

    CHECK_EQUAL(7, read_data.image_identifier.GetData().size());
    CHECK_EQUAL(1232344, read_data.image_identifier.GetVendorIdentifier());
    STRNCMP_EQUAL("1232344", (char *)read_data.image_identifier.GetData().data(), 7);

    CHECK(read_data.maximum_message_length != nullptr);
    CHECK_EQUAL(4219, read_data.maximum_message_length->GetValue());
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(JoinResponseTestsGroup, JoinResponse_serialize_with_VendorSpecificPayload) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    WritableACDescriptor::SubElement info_elements[] = {
        { 1234,
          ACInformationSubElementHeader::Type::HardwareVersion,
          "01234567890ABCDEF01234567890ABCDEF01234567890ABCDEF01234567890ABCDEF" },
        { 5678, ACInformationSubElementHeader::Type::SoftwareVersion, "efghijklm" },
    };

    WritableACDescriptor ac_descriptor{
        100,  200,   1000,         1234, true, false, ACDescriptorHeader::RMACField::Supported,
        true, false, info_elements
    };

    WTPRadioInformation radio_infos[] = { { 0, false, false, false, false } };

    CAPWAPControlIPv4Address control_ip_addresses[] = { { inet_addr("192.168.100.10"), 19 },
                                                        { inet_addr("192.168.100.11"), 20 } };

    CAPWAPLocalIPv4Address local_ip_addresses[] = { { inet_addr("192.168.100.10") } };

    WritableVendorSpecificPayloadArray vendor_specific_payloads;
    vendor_specific_payloads.Add(123456, 789, "01234567890ABCDEF0123");
    vendor_specific_payloads.Add(1, 2, "01234567890A");

    WritableJoinResponse write_data(ResultCode::Type::Success,
                                    ac_descriptor,
                                    "Corporate-AC-1",
                                    radio_infos,
                                    ECNSupport::Type::FullAndLimitedECN,
                                    control_ip_addresses,
                                    local_ip_addresses,
                                    nullptr,
                                    nullptr,
                                    nullptr,
                                    nullptr,
                                    vendor_specific_payloads);

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 246 - (sizeof(ClearHeader) + sizeof(ControlHeader)), raw_data.current);
    const uint8_t reference[] = {
        0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x2A, 0x00, 0xE6,
        0x00, 0x00, 0x21, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x69, 0x00, 0x64,
        0x00, 0xC8, 0x03, 0xE8, 0x04, 0xD2, 0x02, 0x01, 0x00, 0x02, 0x00, 0x00, 0x04, 0xD2, 0x00,
        0x04, 0x00, 0x44, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41,
        0x42, 0x43, 0x44, 0x45, 0x46, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
        0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
        0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x00, 0x00, 0x16, 0x2E,
        0x00, 0x05, 0x00, 0x09, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x00, 0x04,
        0x00, 0x0E, 0x43, 0x6F, 0x72, 0x70, 0x6F, 0x72, 0x61, 0x74, 0x65, 0x2D, 0x41, 0x43, 0x2D,
        0x31, 0x04, 0x18, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x35, 0x00, 0x01, 0x01,
        0x00, 0x0A, 0x00, 0x06, 0xC0, 0xA8, 0x64, 0x0A, 0x00, 0x13, 0x00, 0x0A, 0x00, 0x06, 0xC0,
        0xA8, 0x64, 0x0B, 0x00, 0x14, 0x00, 0x1E, 0x00, 0x04, 0xC0, 0xA8, 0x64, 0x0A, 0x00, 0x25,
        0x00, 0x1B, 0x00, 0x01, 0xE2, 0x40, 0x03, 0x15, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
        0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x30, 0x31, 0x32, 0x33, 0x00,
        0x25, 0x00, 0x12, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
        0x36, 0x37, 0x38, 0x39, 0x30, 0x41
    };

    MEMCMP_EQUAL(buffer,
                 reference + (sizeof(ClearHeader) + sizeof(ControlHeader)),
                 sizeof(reference) - (sizeof(ClearHeader) + sizeof(ControlHeader)));

    raw_data = { buffer, buffer + 246 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };
    ReadableJoinResponse read_data;
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

TEST(JoinResponseTestsGroup, JoinResponse_deserialize) {
    // clang-format off
uint8_t data[] = {
   //=========================================================================
    // 1. CAPWAP Header (8 байт)
    //-------------------------------------------------------------------------
    0x00,       // Preamble: Version=0, Type=0
    0x10,       // HLEN=2 (8 байт), RID (старшие биты)=0
    0xC2,       // RID (младшие биты)=1 -> RID=1, WBID=1 (802.11), T=0
    0x00,       // Флаги: все 0
    0x00, 0x00, // Fragment ID = 0
    0x00, 0x00, // Fragment Offset = 0

    //=========================================================================
    // 2. Control Header (8 байт)
    //-------------------------------------------------------------------------
    0x00, 0x00, 0x00, 0x04, // Message Type = 4 (Join Response)
    0x2A,                   // Sequence Number = 42
    0x00, 0xAD+4,             // 
    0x00,                   // Flags = 0

    //=========================================================================
    // 3. Message Elements (173 байта)
    //-------------------------------------------------------------------------

    // -- Result Code (MUST) -- (8 байт)
    0x00, 0x21,             // Type = 33
    0x00, 0x04,             // Length = 4
    0x00, 0x00, 0x00, 0x00, // Value = 0 (Success)

    // -- AC Descriptor (MUST) -- (42 байта)
    0x00, 0x01,             // Type = 1
    0x00, 0x26+4,             // Length = 38
    0x09, 0xC4,             // Stations = 2500
    0x27, 0x10,             // Limit = 10000
    0x00, 0x64,             // Active WTPs = 100
    0x01, 0xF4,             // Max WTPs = 500
    0x02,                   // Security: 0b00000010 (X.509 Certificate)
    0x01,                   // R-MAC Field: Supported
    0x00,                   // Reserved
    0x02,                   // DTLS Policy: 0b00000010 
    // Sub-element: Hardware Version (Type=4)
    0x00, 0x00, 0x24, 0x3F, 0x00, 0x04, 0x00, 0x04, 'v', '2', '.', '0',
    // Sub-element: Software Version (Type=5)
    0x00, 0x00, 0x24, 0x3F, 0x00, 0x05, 0x00, 0x0A, 'R', 'e', 'l', 'e', 'a', 's', 'e', '-', '1', '0',

    // -- AC Name (MUST) -- (20 байт)
    0x00, 0x04,             // Type = 4
    0x00, 0x0F,             // Length = 15
    'A', 'C', 'M', 'E', '-', 'C', 'o', 'r', 'p', '-', 'A', 'C', '-', '0', '1',

    // -- IEEE 802.11 WTP Radio Information (MUST) -- (9+9 = 18 байт)
    // Radio 1
    0x04, 0x18, 0x00, 0x05, 0x01, 0x00, 0x00, 0x00, 0x0E, // ID=1, Type=a/g/n
    // Radio 2
    0x04, 0x18, 0x00, 0x05, 0x02, 0x00, 0x00, 0x00, 0x06, // ID=2, Type=a/g

    // -- ECN Support (MUST) -- (5 байт)
    0x00, 0x35, 0x00, 0x01, 0x01, // Type=53, Len=1, Value=1 (Full Support)

    // -- CAPWAP Control IPv4 Address (MUST) -- (10 байт)
    0x00, 0x0A,             // Type = 10
    0x00, 0x06,             // Length = 6
    0xC0, 0x00, 0x02, 0x01, // IP Address: 192.0.2.1
    0x00, 0x32,             // WTP Count = 50

    // -- CAPWAP Local IPv4 Address (MUST) -- (8 байт)
    0x00, 0x1E,             // Type = 30
    0x00, 0x04,             // Length = 4
    0xC0, 0x00, 0x02, 0x01, // IP Address: 192.0.2.1

    // -- Image Identifier (OPTIONAL) -- (28 байт)
    0x00, 0x19,             // Type = 25
    0x00, 0x16,             // Length = 22 (4 Vendor + 18 Data)
    0x00, 0x00, 0x24, 0x3F, // Vendor ID = 9279 (Вымышленный ACME Corp)
    'A', 'C', 'M', 'E', '-', 'O', 'S', '-', 'v', '3', '.', '1', '.', '2', '-', 'W', 'A', 'P',

    // -- AC IPv4 List (OPTIONAL) -- (12 байт)
    0x00, 0x02,             // Type = 2
    0x00, 0x08,             // Length = 8 (2 адреса)
    0xC0, 0x00, 0x02, 0x02, // 192.0.2.2
    0xC0, 0x00, 0x02, 0x03, // 192.0.2.3

    // -- Maximum Message Length (OPTIONAL) -- (6 байт)
    0x00, 0x1D,             // Type = 29
    0x00, 0x02,             // Length = 2
    0x20, 0x00,             // Value: 8192

    // -- Vendor Specific Payload (OPTIONAL) -- (16 байт)
    0x00, 0x25,             // Type = 37
    0x00, 0x0F,             // Length = 15
    0x00, 0x00, 0x00, 0x09, // Vendor ID = 9 (Cisco)
    0x00, 0x64,             // Element ID = 100
    'd', 'i', 'a', 'g', '-', 'd', 'a', 't', 'a', // "diag-data"
};
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableJoinResponse read_data;

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);

    CHECK_EQUAL(2500, read_data.ac_descriptor.header->GetStations());
    CHECK_EQUAL(10000, read_data.ac_descriptor.header->GetLimit());
    CHECK_EQUAL(100, read_data.ac_descriptor.header->GetActiveWTPs());
    CHECK_EQUAL(500, read_data.ac_descriptor.header->GetMaxWTPs());
    CHECK_TRUE(read_data.ac_descriptor.header->X509Cert());
    CHECK_FALSE(read_data.ac_descriptor.header->PresharedSecret());
    CHECK_EQUAL(ACDescriptorHeader::RMACField::Supported,
                read_data.ac_descriptor.header->GetRMAC());
    CHECK_TRUE(read_data.ac_descriptor.header->ClearDataChannel());
    CHECK_FALSE(read_data.ac_descriptor.header->DTLSDataChannel());
    CHECK_EQUAL(2, read_data.ac_descriptor.Get().size());

    STRNCMP_EQUAL("v2.0", (char *)read_data.ac_descriptor.Get()[0]->data, 9);
    CHECK_EQUAL(ACInformationSubElementHeader::Type::HardwareVersion,
                read_data.ac_descriptor.Get()[0]->GetType());
    CHECK_EQUAL(4, read_data.ac_descriptor.Get()[0]->GetLength());
    CHECK_EQUAL(9279, read_data.ac_descriptor.Get()[0]->GetVendorIdentifier());

    STRNCMP_EQUAL("Release-10", (char *)read_data.ac_descriptor.Get()[1]->data, 10);
    CHECK_EQUAL(ACInformationSubElementHeader::Type::SoftwareVersion,
                read_data.ac_descriptor.Get()[1]->GetType());
    CHECK_EQUAL(10, read_data.ac_descriptor.Get()[1]->GetLength());
    CHECK_EQUAL(9279, read_data.ac_descriptor.Get()[1]->GetVendorIdentifier());

    CHECK_EQUAL(15, read_data.ac_name->GetLength());
    STRNCMP_EQUAL("ACME-Corp-AC-01", (char *)read_data.ac_name->name, 15);

    CHECK_EQUAL(2, read_data.wtp_radio_informations.Get().size());
    CHECK_EQUAL(1, read_data.wtp_radio_informations.Get()[0]->RadioID);
    CHECK_FALSE(read_data.wtp_radio_informations.Get()[0]->B);
    CHECK_TRUE(read_data.wtp_radio_informations.Get()[0]->A);
    CHECK_TRUE(read_data.wtp_radio_informations.Get()[0]->G);
    CHECK_TRUE(read_data.wtp_radio_informations.Get()[0]->N);
    CHECK_EQUAL(2, read_data.wtp_radio_informations.Get()[1]->RadioID);
    CHECK_FALSE(read_data.wtp_radio_informations.Get()[1]->B);
    CHECK_TRUE(read_data.wtp_radio_informations.Get()[1]->A);
    CHECK_TRUE(read_data.wtp_radio_informations.Get()[1]->G);
    CHECK_FALSE(read_data.wtp_radio_informations.Get()[1]->N);

    CHECK_EQUAL(1, read_data.control_ip_addresses.Get().size());
    CHECK_EQUAL(inet_addr("192.0.2.1"), read_data.control_ip_addresses.Get()[0]->GetIPAddress());
    CHECK_EQUAL(50, read_data.control_ip_addresses.Get()[0]->GetWTPCount());

    CHECK_EQUAL(1, read_data.local_ip_addresses.Get().size());
    CHECK_EQUAL(inet_addr("192.0.2.1"), read_data.local_ip_addresses.Get()[0]->GetIPAddress());

    CHECK_EQUAL(18, read_data.image_identifier.GetData().size());
    CHECK_EQUAL(9279, read_data.image_identifier.GetVendorIdentifier());
    STRNCMP_EQUAL("ACME-OS-v3.1.2-WAP", (char *)read_data.image_identifier.GetData().data(), 18);

    CHECK(read_data.ac_ipv4_list != nullptr);
    CHECK_EQUAL(2, read_data.ac_ipv4_list->GetCount());
    CHECK_EQUAL(inet_addr("192.0.2.2"), read_data.ac_ipv4_list->addresses[0]);
    CHECK_EQUAL(inet_addr("192.0.2.3"), read_data.ac_ipv4_list->addresses[1]);

    CHECK(read_data.maximum_message_length != nullptr);
    CHECK_EQUAL(8192, read_data.maximum_message_length->GetValue());

    CHECK_EQUAL(1, read_data.vendor_specific_payloads.Get().size());
    CHECK_EQUAL(9, read_data.vendor_specific_payloads.Get()[0]->GetVendorIdentifier());
    CHECK_EQUAL(100, read_data.vendor_specific_payloads.Get()[0]->GetElementId());
    STRNCMP_EQUAL("diag-data", (char *)read_data.vendor_specific_payloads.Get()[0]->value, 9);
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(JoinResponseTestsGroup, JoinResponse_deserialize_handle_unknown_element) {
    // clang-format off
uint8_t data[] = {
//=========================================================================
    // 1. CAPWAP Header (8 байт)
    //-------------------------------------------------------------------------
    0x00,       // Preamble: Version=0, Type=0
    0x10,       // HLEN=2 (8 байт), RID (старшие биты)=0
    0xC2,       // RID (младшие биты)=1 -> RID=1, WBID=1 (802.11), T=0
    0x00,       // Флаги: все 0
    0x00, 0x00, // Fragment ID = 0
    0x00, 0x00, // Fragment Offset = 0

    //=========================================================================
    // 2. Control Header (8 байт)
    //-------------------------------------------------------------------------
    0x00, 0x00, 0x00, 0x04, // Message Type = 4 (Join Response)
    0x2A,                   // Sequence Number = 42
    0x00, 124,             // 
    0x00,                   // Flags = 0

    //=========================================================================
    // 3. Message Elements (173 байта)
    //-------------------------------------------------------------------------

    // -- Result Code (MUST) -- (8 байт)
    0x00, 0x21,             // Type = 33
    0x00, 0x04,             // Length = 4
    0x00, 0x00, 0x00, 0x00, // Value = 0 (Success)

    // -- AC Descriptor (MUST) -- (42 байта)
    0x00, 0x01,             // Type = 1
    0x00, 0x26+4,             // Length = 38
    0x09, 0xC4,             // Stations = 2500
    0x27, 0x10,             // Limit = 10000
    0x00, 0x64,             // Active WTPs = 100
    0x01, 0xF4,             // Max WTPs = 500
    0x02,                   // Security: 0b00000010 (X.509 Certificate)
    0x01,                   // R-MAC Field: Supported
    0x00,                   // Reserved
    0x02,                   // DTLS Policy: 0b00000010 
    // Sub-element: Hardware Version (Type=4)
    0x00, 0x00, 0x24, 0x3F, 0x00, 0x04, 0x00, 0x04, 'v', '2', '.', '0',
    // Sub-element: Software Version (Type=5)
    0x00, 0x00, 0x24, 0x3F, 0x00, 0x05, 0x00, 0x0A, 'R', 'e', 'l', 'e', 'a', 's', 'e', '-', '1', '0',

    // -- AC Name (MUST) -- (20 байт)
    0x00, 0x04,             // Type = 4
    0x00, 0x0F,             // Length = 15
    'A', 'C', 'M', 'E', '-', 'C', 'o', 'r', 'p', '-', 'A', 'C', '-', '0', '1',

    // -- IEEE 802.11 WTP Radio Information (MUST) -- (9+9 = 18 байт)
    // Radio 1
    0x04, 0x18, 0x00, 0x05, 0x01, 0x00, 0x00, 0x00, 0x0E, // ID=1, Type=a/g/n
    // Radio 2
    0x04, 0x18, 0x00, 0x05, 0x02, 0x00, 0x00, 0x00, 0x06, // ID=2, Type=a/g

    // -- ECN Support (MUST) -- (5 байт)
    0x00, 0x35, 0x00, 0x01, 0x01, // Type=53, Len=1, Value=1 (Full Support)

    // -- CAPWAP Control IPv4 Address (MUST) -- (10 байт)
    0x00, 0x0A,             // Type = 10
    0x00, 0x06,             // Length = 6
    0xC0, 0x00, 0x02, 0x01, // IP Address: 192.0.2.1
    0x00, 0x32,             // WTP Count = 50

    // -- CAPWAP Local IPv4 Address (MUST) -- (8 байт)
    0x00, 0x1E,             // Type = 30
    0x00, 0x04,             // Length = 4
    0xC0, 0x00, 0x02, 0x01, // IP Address: 192.0.2.1

    // 6. Unknown (5 байт)
    0xFF, 0xFF, 0x00, 0x01, 0x00,
    // 7. Unknown (5 байт)
    0xFF, 0xFE, 0x00, 0x01, 0x00,
};
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableJoinResponse read_data;

    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, read_data.unknown_elements);
}
