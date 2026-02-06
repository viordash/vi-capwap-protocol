#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "DiscoveryResponse.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(DiscoveryResponseTestsGroup){ //
                                         TEST_SETUP(){}

                                         TEST_TEARDOWN(){}
};

TEST(DiscoveryResponseTestsGroup, DiscoveryResponse_serialize) {
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
        { 0, false, false, false, false },
        { 1, true, true, false, false },
        { 2, false, false, false, false },
    };

    CAPWAPControlIPv4Address ip_addresses[] = { { inet_addr("192.168.100.10"), 19 },
                                                { inet_addr("192.168.100.11"), 20 } };

    WritableDiscoveryResponse write_data(ac_descriptor,
                                         "Corporate-AC-1",
                                         radio_infos,
                                         ip_addresses,
                                         VendorSpecificPayload::Dummy);

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 190 - (sizeof(ClearHeader) + sizeof(ControlHeader)), raw_data.current);
    const uint8_t reference[] = {
        0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x2A, 0x00, 0xAE,
        0x00, 0x00, 0x01, 0x00, 0x69, 0x00, 0x64, 0x00, 0xC8, 0x03, 0xE8, 0x04, 0xD2, 0x02, 0x01,
        0x00, 0x02, 0x00, 0x00, 0x04, 0xD2, 0x00, 0x04, 0x00, 0x44, 0x30, 0x31, 0x32, 0x33, 0x34,
        0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x30, 0x31, 0x32,
        0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x30,
        0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45,
        0x46, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43,
        0x44, 0x45, 0x46, 0x00, 0x00, 0x16, 0x2E, 0x00, 0x05, 0x00, 0x09, 0x65, 0x66, 0x67, 0x68,
        0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x00, 0x04, 0x00, 0x0E, 0x43, 0x6F, 0x72, 0x70, 0x6F, 0x72,
        0x61, 0x74, 0x65, 0x2D, 0x41, 0x43, 0x2D, 0x31, 0x04, 0x18, 0x00, 0x05, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x04, 0x18, 0x00, 0x05, 0x01, 0x00, 0x00, 0x00, 0x03, 0x04, 0x18, 0x00, 0x05,
        0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x06, 0xC0, 0xA8, 0x64, 0x0A, 0x00, 0x13,
        0x00, 0x0A, 0x00, 0x06, 0xC0, 0xA8, 0x64, 0x0B, 0x00, 0x14
    };

    MEMCMP_EQUAL(buffer,
                 reference + (sizeof(ClearHeader) + sizeof(ControlHeader)),
                 sizeof(reference) - (sizeof(ClearHeader) + sizeof(ControlHeader)));

    raw_data = { buffer, buffer + 190 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };
    ReadableDiscoveryResponse read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(3, read_data.wtp_radio_informations.Get().size());
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

    CHECK_EQUAL(2, read_data.ip_addresses.Get().size());
    CHECK_EQUAL(inet_addr("192.168.100.10"), read_data.ip_addresses.Get()[0]->GetIPAddress());
    CHECK_EQUAL(19, read_data.ip_addresses.Get()[0]->GetWTPCount());
    CHECK_EQUAL(inet_addr("192.168.100.11"), read_data.ip_addresses.Get()[1]->GetIPAddress());
    CHECK_EQUAL(20, read_data.ip_addresses.Get()[1]->GetWTPCount());

    STRNCMP_EQUAL("Corporate-AC-1", read_data.ac_name->name, 14);
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(DiscoveryResponseTestsGroup, DiscoveryResponse_serialize_with_VendorSpecificPayload) {
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

    const char *ac_name = "Corporate-AC-1";

    WTPRadioInformation radio_infos[] = { { 0, false, false, false, false } };

    CAPWAPControlIPv4Address ip_addresses[] = { { inet_addr("192.168.100.10"), 19 } };

    WritableVendorSpecificPayloadArray vendor_specific_payloads;
    vendor_specific_payloads.Add(123456, 789, "01234567890ABCDEF0123");
    vendor_specific_payloads.Add(1, 2, "01234567890A");

    WritableDiscoveryResponse write_data(ac_descriptor,
                                         ac_name,
                                         radio_infos,
                                         ip_addresses,
                                         vendor_specific_payloads);

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 215 - (sizeof(ClearHeader) + sizeof(ControlHeader)), raw_data.current);
    const uint8_t reference[] = {
        0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x2A, 0x00, 0xC7,
        0x00, 0x00, 0x01, 0x00, 0x69, 0x00, 0x64, 0x00, 0xC8, 0x03, 0xE8, 0x04, 0xD2, 0x02, 0x01,
        0x00, 0x02, 0x00, 0x00, 0x04, 0xD2, 0x00, 0x04, 0x00, 0x44, 0x30, 0x31, 0x32, 0x33, 0x34,
        0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x30, 0x31, 0x32,
        0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x30,
        0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45,
        0x46, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43,
        0x44, 0x45, 0x46, 0x00, 0x00, 0x16, 0x2E, 0x00, 0x05, 0x00, 0x09, 0x65, 0x66, 0x67, 0x68,
        0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x00, 0x04, 0x00, 0x0E, 0x43, 0x6F, 0x72, 0x70, 0x6F, 0x72,
        0x61, 0x74, 0x65, 0x2D, 0x41, 0x43, 0x2D, 0x31, 0x04, 0x18, 0x00, 0x05, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x0A, 0x00, 0x06, 0xC0, 0xA8, 0x64, 0x0A, 0x00, 0x13, 0x00, 0x25, 0x00,
        0x1B, 0x00, 0x01, 0xE2, 0x40, 0x03, 0x15, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x30, 0x31, 0x32, 0x33, 0x00, 0x25,
        0x00, 0x12, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
        0x37, 0x38, 0x39, 0x30, 0x41
    };

    MEMCMP_EQUAL(buffer,
                 reference + (sizeof(ClearHeader) + sizeof(ControlHeader)),
                 sizeof(reference) - (sizeof(ClearHeader) + sizeof(ControlHeader)));

    raw_data = { buffer, buffer + 215 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };
    ReadableDiscoveryResponse read_data;
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

TEST(DiscoveryResponseTestsGroup, DiscoveryResponse_deserialize) {
    // clang-format off
// Юнит-тест: CAPWAP Discovery Response Message
// Общая длина UDP Payload: 8(Hdr) + 8(Ctrl) + 101(MEs) = 117 байт
uint8_t data[] = {
    // --- CAPWAP Header (8 байт) ---
    // HLEN=2, RID=0 (неприменимо), WBID=1 (802.11)
    0x00, 0x10, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,

    // --- Control Header (8 байт) ---
    // Message Type = 2 (Discovery Response), Seq Num = 42, ME Length = 101
    0x00, 0x00, 0x00, 0x02, 0x2A, 0x00, 0x65, 0x00,

    // --- Сообщения-элементы (Message Elements, 101 байт) ---

    // 1. AC Descriptor (Тип: 1, Длина: 49)
    0x00, 0x01, 0x00, 0x31,
    // Value:
    0x00, 0x96,             // Stations: 150
    0x03, 0xE8,             // Limit: 1000
    0x00, 0x23,             // Active WTPs: 35
    0x00, 0x64,             // Max WTPs: 100
    0x06,                   // Security: PSK + X.509
    0x01,                   // R-MAC Field: Supported
    0x00,                   // Reserved1
    0x06,                   // DTLS Policy: DTLS + Cleartext
    // Sub-Element: Hardware Version
    0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x09,
    'A', 'C', '-', 'H', 'W', '-', 'v', '3', '.', // "AC-HW-v3."
    // Sub-Element: Software Version
    0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x0C,
    'F', 'i', 'r', 'm', 'w', 'a', 'r', 'e', '-', '8', '.', '5', // "Firmware-8.5"

    // 2. AC Name (Тип: 4, Длина: 16)
    0x00, 0x04, 0x00, 0x10,
    // Value: "Corp-AC-Floor15"
    0x43, 0x6F, 0x72, 0x70, 0x2D, 0x41, 0x43, 0x2D,
    0x46, 0x6C, 0x6F, 0x6F, 0x72, 0x31, 0x35, 0x00,

    // 3. CAPWAP Control IPv4 Address (Тип: 10, Длина: 6)
    0x00, 0x0A, 0x00, 0x06,
    // Value:
    0x0A, 0x14, 0x1E, 0x28, // IP: 10.20.30.40
    0x00, 0x23,             // WTP Count: 35

    // 4. IEEE 802.11 WTP Radio Information #1 (Тип: 1048, Длина: 5)
    0x04, 0x18, 0x00, 0x05,
    // Value:
    0x01,                   // Radio ID: 1
    0x00, 0x00, 0x00, 0x0D, // Radio Type: 0x1A (b/g/n)

    // 5. IEEE 802.11 WTP Radio Information #2 (Тип: 1048, Длина: 5)
    0x04, 0x18, 0x00, 0x05,
    // Value:
    0x02,                   // Radio ID: 2
    0x00, 0x00, 0x00, 0x0A  // Radio Type: 0x30 (a/n)
};
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableDiscoveryResponse read_data;

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);

    CHECK_EQUAL(150, read_data.ac_descriptor.header->GetStations());
    CHECK_EQUAL(1000, read_data.ac_descriptor.header->GetLimit());
    CHECK_EQUAL(35, read_data.ac_descriptor.header->GetActiveWTPs());
    CHECK_EQUAL(100, read_data.ac_descriptor.header->GetMaxWTPs());
    CHECK_TRUE(read_data.ac_descriptor.header->X509Cert());
    CHECK_TRUE(read_data.ac_descriptor.header->PresharedSecret());
    CHECK_EQUAL(ACDescriptorHeader::RMACField::Supported,
                read_data.ac_descriptor.header->GetRMAC());
    CHECK_TRUE(read_data.ac_descriptor.header->ClearDataChannel());
    CHECK_TRUE(read_data.ac_descriptor.header->DTLSDataChannel());
    CHECK_EQUAL(2, read_data.ac_descriptor.Get().size());

    CHECK_EQUAL(1, read_data.ip_addresses.Get().size());
    CHECK_EQUAL(inet_addr("10.20.30.40"), read_data.ip_addresses.Get()[0]->GetIPAddress());
    CHECK_EQUAL(35, read_data.ip_addresses.Get()[0]->GetWTPCount());

    STRNCMP_EQUAL("AC-HW-v3.", (char *)read_data.ac_descriptor.Get()[0]->data, 9);
    CHECK_EQUAL(ACInformationSubElementHeader::Type::HardwareVersion,
                read_data.ac_descriptor.Get()[0]->GetType());
    CHECK_EQUAL(9, read_data.ac_descriptor.Get()[0]->GetLength());
    CHECK_EQUAL(0, read_data.ac_descriptor.Get()[0]->GetVendorIdentifier());

    STRNCMP_EQUAL("Firmware-8.5", (char *)read_data.ac_descriptor.Get()[1]->data, 12);
    CHECK_EQUAL(ACInformationSubElementHeader::Type::SoftwareVersion,
                read_data.ac_descriptor.Get()[1]->GetType());
    CHECK_EQUAL(12, read_data.ac_descriptor.Get()[1]->GetLength());
    CHECK_EQUAL(0, read_data.ac_descriptor.Get()[1]->GetVendorIdentifier());

    CHECK_EQUAL(16, read_data.ac_name->GetLength());
    STRNCMP_EQUAL("Corp-AC-Floor15", (char *)read_data.ac_name->name, 16);

    CHECK_EQUAL(2, read_data.wtp_radio_informations.Get().size());
    CHECK_EQUAL(1, read_data.wtp_radio_informations.Get()[0]->RadioID);
    CHECK_TRUE(read_data.wtp_radio_informations.Get()[0]->B);
    CHECK_FALSE(read_data.wtp_radio_informations.Get()[0]->A);
    CHECK_TRUE(read_data.wtp_radio_informations.Get()[0]->G);
    CHECK_TRUE(read_data.wtp_radio_informations.Get()[0]->N);
    CHECK_EQUAL(2, read_data.wtp_radio_informations.Get()[1]->RadioID);
    CHECK_FALSE(read_data.wtp_radio_informations.Get()[1]->B);
    CHECK_TRUE(read_data.wtp_radio_informations.Get()[1]->A);
    CHECK_FALSE(read_data.wtp_radio_informations.Get()[1]->G);
    CHECK_TRUE(read_data.wtp_radio_informations.Get()[1]->N);
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(DiscoveryResponseTestsGroup, DiscoveryResponse_deserialize_handle_unknown_element) {
    // clang-format off
// Юнит-тест: CAPWAP Discovery Response Message
// Общая длина UDP Payload: 8(Hdr) + 8(Ctrl) + 101(MEs) = 117+5+5 байт
uint8_t data[] = {
    // --- CAPWAP Header (8 байт) ---
    // HLEN=2, RID=0 (неприменимо), WBID=1 (802.11)
    0x00, 0x10, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,

    // --- Control Header (8 байт) ---
    // Message Type = 2 (Discovery Response), Seq Num = 42, ME Length = 101
    0x00, 0x00, 0x00, 0x02, 0x2A, 0x00, 0x65+5+5, 0x00,

    // --- Сообщения-элементы (Message Elements, 101+5+5 байт) ---

    // 1. AC Descriptor (Тип: 1, Длина: 49)
    0x00, 0x01, 0x00, 0x31,
    // Value:
    0x00, 0x96,             // Stations: 150
    0x03, 0xE8,             // Limit: 1000
    0x00, 0x23,             // Active WTPs: 35
    0x00, 0x64,             // Max WTPs: 100
    0x06,                   // Security: PSK + X.509
    0x01,                   // R-MAC Field: Supported
    0x00,                   // Reserved1
    0x06,                   // DTLS Policy: DTLS + Cleartext
    // Sub-Element: Hardware Version
    0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x09,
    'A', 'C', '-', 'H', 'W', '-', 'v', '3', '.', // "AC-HW-v3."
    // Sub-Element: Software Version
    0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x0C,
    'F', 'i', 'r', 'm', 'w', 'a', 'r', 'e', '-', '8', '.', '5', // "Firmware-8.5"

    // 2. AC Name (Тип: 4, Длина: 16)
    0x00, 0x04, 0x00, 0x10,
    // Value: "Corp-AC-Floor15"
    0x43, 0x6F, 0x72, 0x70, 0x2D, 0x41, 0x43, 0x2D,
    0x46, 0x6C, 0x6F, 0x6F, 0x72, 0x31, 0x35, 0x00,

    // 3. CAPWAP Control IPv4 Address (Тип: 10, Длина: 6)
    0x00, 0x0A, 0x00, 0x06,
    // Value:
    0x0A, 0x14, 0x1E, 0x28, // IP: 10.20.30.40
    0x00, 0x23,             // WTP Count: 35

    // 4. IEEE 802.11 WTP Radio Information #1 (Тип: 1048, Длина: 5)
    0x04, 0x18, 0x00, 0x05,
    // Value:
    0x01,                   // Radio ID: 1
    0x00, 0x00, 0x00, 0x0D, // Radio Type: 0x1A (b/g/n)

    // 5. IEEE 802.11 WTP Radio Information #2 (Тип: 1048, Длина: 5)
    0x04, 0x18, 0x00, 0x05,
    // Value:
    0x02,                   // Radio ID: 2
    0x00, 0x00, 0x00, 0x0A,  // Radio Type: 0x30 (a/n)

    // 6. Unknown (5 байт)
    0xFF, 0xFF, 0x00, 0x01, 0x00, 
    // 7. Unknown (5 байт)
    0xFF, 0xFE, 0x00, 0x01, 0x00, 
};
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableDiscoveryResponse read_data;

    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, read_data.unknown_elements);
}
