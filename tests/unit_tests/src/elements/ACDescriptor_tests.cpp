#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/ACDescriptor.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(ACDescriptorTestsGroup){ //
                                    TEST_SETUP(){}

                                    TEST_TEARDOWN(){}
};

TEST(ACDescriptorTestsGroup, ACDescriptor_serialize) {
    uint8_t buffer[256] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    WritableACDescriptor::SubElement info_elements[] = {
        { 1234,
          ACInformationSubElementHeader::Type::HardwareVersion,
          "01234567890ABCDEF01234567890ABCDEF01234567890ABCDEF01234567890ABCDEF" },
        { 5678, ACInformationSubElementHeader::Type::SoftwareVersion, "efghijklm" },
    };

    WritableACDescriptor write_data{ 3,
                                     5,
                                     4,
                                     10,
                                     false,
                                     true,
                                     ACDescriptorHeader::RMACField::Supported,
                                     false,
                                     true,
                                     info_elements };

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 109, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x01, 0x00, 0x69, 0x00, 0x03, 0x00, 0x05, 0x00, 0x04, 0x00,
                                  0x0A, 0x04, 0x01, 0x00, 0x04, 0x00, 0x00, 0x04, 0xD2, 0x00, 0x04,
                                  0x00, 0x44, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
                                  0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x30, 0x31, 0x32,
                                  0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43,
                                  0x44, 0x45, 0x46, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                                  0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x30, 0x31,
                                  0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42,
                                  0x43, 0x44, 0x45, 0x46, 0x00, 0x00, 0x16, 0x2E, 0x00, 0x05, 0x00,
                                  0x09, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    ReadableACDescriptor read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(&buffer[0] + sizeof(reference), raw_data.current);
    CHECK_EQUAL(3, read_data.header->GetStations());
    CHECK_EQUAL(5, read_data.header->GetLimit());
    CHECK_EQUAL(4, read_data.header->GetActiveWTPs());
    CHECK_EQUAL(10, read_data.header->GetMaxWTPs());
    CHECK_FALSE(read_data.header->X509Cert());
    CHECK_TRUE(read_data.header->PresharedSecret());
    CHECK_EQUAL(ACDescriptorHeader::RMACField::Supported, read_data.header->GetRMAC());
    CHECK_FALSE(read_data.header->ClearDataChannel());
    CHECK_TRUE(read_data.header->DTLSDataChannel());
    CHECK_EQUAL(2, read_data.Get().size());

    STRNCMP_EQUAL("01234567890ABCDEF01234567890ABCDEF01234567890ABCDEF01234567890ABCDEF",
                  (char *)read_data.Get()[0]->data,
                  68);
    CHECK_EQUAL(ACInformationSubElementHeader::Type::HardwareVersion,
                read_data.Get()[0]->GetType());
    CHECK_EQUAL(68, read_data.Get()[0]->GetLength());
    CHECK_EQUAL(1234, read_data.Get()[0]->GetVendorIdentifier());

    STRNCMP_EQUAL("efghijklm", (char *)read_data.Get()[1]->data, 9);
    CHECK_EQUAL(ACInformationSubElementHeader::Type::SoftwareVersion,
                read_data.Get()[1]->GetType());
    CHECK_EQUAL(9, read_data.Get()[1]->GetLength());
    CHECK_EQUAL(5678, read_data.Get()[1]->GetVendorIdentifier());
}

TEST(ACDescriptorTestsGroup, ACDescriptor_deserialize) {
    // clang-format off
    uint8_t data[] = {           
     // --- TLV Header (4 байта) ---
    0x00, 0x01,       // Type = 1
    0x00, 0x4D,       // Length = 77

    // --- Value (77 байт) ---

    // -- Фиксированная часть (12 байт) --
    0x02, 0x26,       // Stations: 550
    0x07, 0xD0,       // Limit: 2000
    0x00, 0x78,       // Active WTPs: 120
    0x01, 0xF4,       // Max WTPs: 500
    0x06,             // Security: 0b00000110 (PSK + X.509)
    0x01,             // R-MAC Field: 1 (Supported)
    0x00,             // Reserved1
    0x06,             // DTLS Policy: 0b00000110 (DTLS + Cleartext)

    // -- Суб-элементы (65 байт) --

    // Суб-элемент #1: Hardware Version (18 байт)
    0x00, 0x00, 0x00, 0x00, // Vendor ID: 0 (IETF)
    0x00, 0x04,             // Type: 4 (Hardware Version)
    0x00, 0x0A,             // Length: 10
    'A', 'C', '-', 'H', 'W', '-', 'v', '2', '.', '5', // "AC-HW-v2.5"

    // Суб-элемент #2: Software Version (23 байта)
    0x00, 0x00, 0x00, 0x00, // Vendor ID: 0 (IETF)
    0x00, 0x05,             // Type: 5 (Software Version)
    0x00, 0x0F,             // Length: 15
    'C', 'a', 'p', 'w', 'a', 'p', 'O', 'S', ' ', '9', '.', '1', '.', '3',
    0x00,                   // <-- ИСПРАВЛЕНИЕ: Добавлен 1 байт для соответствия длине 15

    // Суб-элемент #3: Vendor-Specific Serial (24 байта)
    0x00, 0x01, 0x86, 0x9F, // Vendor ID: 99999 (Fictional)
    0x00, 0x01,             // Type: 1 (Vendor-defined: Serial Number)
    0x00, 0x10,             // Length: 16
    'A', 'C', '-', 'S', 'N', '-', '9', '8', '7', '6', '5', '4', '3', '2', '1',
    0x00                    // <-- ИСПРАВЛЕНИЕ: Добавлен 1 байт для соответствия длине 16
    };
    // clang-format on

    RawData raw_data{ data, data + sizeof(data) };

    ReadableACDescriptor read_data;

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);

    CHECK_EQUAL(550, read_data.header->GetStations());
    CHECK_EQUAL(2000, read_data.header->GetLimit());
    CHECK_EQUAL(120, read_data.header->GetActiveWTPs());
    CHECK_EQUAL(500, read_data.header->GetMaxWTPs());
    CHECK_TRUE(read_data.header->X509Cert());
    CHECK_TRUE(read_data.header->PresharedSecret());
    CHECK_EQUAL(ACDescriptorHeader::RMACField::Supported, read_data.header->GetRMAC());
    CHECK_TRUE(read_data.header->ClearDataChannel());
    CHECK_TRUE(read_data.header->DTLSDataChannel());
    CHECK_EQUAL(3, read_data.Get().size());

    STRNCMP_EQUAL("AC-HW-v2.5", (char *)read_data.Get()[0]->data, 10);
    CHECK_EQUAL(ACInformationSubElementHeader::Type::HardwareVersion,
                read_data.Get()[0]->GetType());
    CHECK_EQUAL(10, read_data.Get()[0]->GetLength());

    STRNCMP_EQUAL("CapwapOS 9.1.3", (char *)read_data.Get()[1]->data, 14);
    CHECK_EQUAL(ACInformationSubElementHeader::Type::SoftwareVersion,
                read_data.Get()[1]->GetType());
    CHECK_EQUAL(15, read_data.Get()[1]->GetLength());

    STRNCMP_EQUAL("AC-SN-987654321", (char *)read_data.Get()[2]->data, 15);
    CHECK_EQUAL(0x0100, read_data.Get()[2]->GetType());
    CHECK_EQUAL(16, read_data.Get()[2]->GetLength());
}
