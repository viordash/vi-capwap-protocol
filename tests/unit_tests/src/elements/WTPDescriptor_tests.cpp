#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/WTPDescriptor.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(WTPDescriptorTestsGroup){ //
                                     TEST_SETUP(){}

                                     TEST_TEARDOWN(){}
};

TEST(WTPDescriptorTestsGroup, WTPDescriptor_serialize) {
    uint8_t buffer[256] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    EncryptionSubElement encr_elements[] = { { 0xAA55 } };

    WritableWTPDescriptor::SubElement descr_elements[] = {
        { 1234, DescriptorSubElementHeader::Type::ActiveSoftwareVersion, "abcd" },
        { 5678, DescriptorSubElementHeader::Type::BootVersion, "efghijklm" },
        { 9012,
          DescriptorSubElementHeader::Type::OtherSoftwareVersion,
          "01234567890ABCDEF01234567890ABCDEF01234567890ABCDEF01234567890ABCDEF" },
        { 3456, DescriptorSubElementHeader::Type::HardwareVersion, "1" }
    };

    WritableWTPDescriptor write_data{ 3, 2, encr_elements, descr_elements };

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 124, raw_data.current);
    const uint8_t reference[] = {
        0x00, 0x27, 0x00, 0x78, 0x03, 0x02, 0x01, 0x01, 0x55, 0xAA, 0x00, 0x00, 0x04, 0xD2,
        0x00, 0x01, 0x00, 0x04, 0x61, 0x62, 0x63, 0x64, 0x00, 0x00, 0x16, 0x2E, 0x00, 0x02,
        0x00, 0x09, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x00, 0x00, 0x23,
        0x34, 0x00, 0x03, 0x00, 0x44, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
        0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
        0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x30, 0x31, 0x32,
        0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43,
        0x44, 0x45, 0x46, 0x00, 0x00, 0x0D, 0x80, 0x00, 0x00, 0x00, 0x01, 0x31
    };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    ReadableWTPDescriptor read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(&buffer[0] + sizeof(reference), raw_data.current);
    CHECK_EQUAL(3, read_data.header->MaxRadios);
    CHECK_EQUAL(2, read_data.header->RadiosInUse);

    CHECK_EQUAL(1, read_data.header->NumEncrypt);
    CHECK_EQUAL(1, read_data.GetEncryptions().size());
    CHECK_EQUAL(WBIDType::IEEE_80211, read_data.GetEncryptions()[0]->WBID);
    CHECK_EQUAL(0xAA55, read_data.GetEncryptions()[0]->EncryptionCapabilities);

    CHECK_EQUAL(4, read_data.GetDescriptors().size());
    STRNCMP_EQUAL("abcd", (char *)read_data.GetDescriptors()[0]->utf8_value, 4);
    CHECK_EQUAL(DescriptorSubElementHeader::Type::ActiveSoftwareVersion,
                read_data.GetDescriptors()[0]->GetType());
    CHECK_EQUAL(4, read_data.GetDescriptors()[0]->GetLength());

    STRNCMP_EQUAL("efghijklm", (char *)read_data.GetDescriptors()[1]->utf8_value, 9);
    CHECK_EQUAL(DescriptorSubElementHeader::Type::BootVersion,
                read_data.GetDescriptors()[1]->GetType());
    CHECK_EQUAL(9, read_data.GetDescriptors()[1]->GetLength());

    STRNCMP_EQUAL("01234567890ABCDEF01234567890ABCDEF01234567890ABCDEF01234567890ABCDEF",
                  (char *)read_data.GetDescriptors()[2]->utf8_value,
                  68);
    CHECK_EQUAL(DescriptorSubElementHeader::Type::OtherSoftwareVersion,
                read_data.GetDescriptors()[2]->GetType());
    CHECK_EQUAL(68, read_data.GetDescriptors()[2]->GetLength());

    STRNCMP_EQUAL("1", (char *)read_data.GetDescriptors()[3]->utf8_value, 1);
    CHECK_EQUAL(DescriptorSubElementHeader::Type::HardwareVersion,
                read_data.GetDescriptors()[3]->GetType());
    CHECK_EQUAL(1, read_data.GetDescriptors()[3]->GetLength());
}

TEST(WTPDescriptorTestsGroup, WTPDescriptor_deserialize) {
    uint8_t data[] = {
        // 4. WTP Descriptor (Тип: 39, Длина: 57 (0x39))
        // ---- Заголовок Message Element (4 байта) ----
        0x00,
        0x27, // Type: 39
        0x00,
        0x3A, // Length: 58

        // ---- Поле Value (58 байт) ----
        // Основные поля
        0x01, // Max Radios: 1
        0x01, // Radios in use: 1
        0x01, // Num Encrypt: 1

        // Encryption Sub-Element (3 байта)
        0x01,
        0x00,
        0x00,

        // -- Суб-элемент #1 (Hardware Version) --
        // Заголовок (8 байт)
        0x00,
        0x00,
        0x00,
        0x00, // Vendor ID
        0x00,
        0x00, // Type
        0x00,
        0x05, // Length
        // Значение (5 байт)
        0x50,
        0x52,
        0x4F,
        0x44,
        0x32, // ASCII: "PROD2"

        // -- Суб-элемент #2 (Active Software Version) --
        // Заголовок (8 байт)
        0x00,
        0x00,
        0x00,
        0x00, // Vendor ID
        0x00,
        0x01, // Type
        0x00,
        0x10, // Length: 16
        // Значение (16 байт)
        0x38,
        0x2E,
        0x35,
        0x2E,
        0x31,
        0x30,
        0x35,
        0x2E,
        0x30,
        0x2C,
        0x20,
        0x53,
        0x50,
        0x45,
        0x43,
        0x4C, // ASCII: "8.5.105.0, SPECL"

        // -- Суб-элемент #3 (Boot Version) --
        // Заголовок (8 байт)
        0x00,
        0x00,
        0x00,
        0x00, // Vendor ID
        0x00,
        0x02, // Type
        0x00,
        0x07, // Length
        // Значение (7 байт)
        0x62,
        0x6F,
        0x6F,
        0x74,
        0x6C,
        0x64,
        0x72 // ASCII: "bootldr"
    };
    RawData raw_data{ data, data + sizeof(data) };

    ReadableWTPDescriptor read_data;

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);

    CHECK_EQUAL(1, read_data.header->MaxRadios);
    CHECK_EQUAL(1, read_data.header->RadiosInUse);
    CHECK_EQUAL(1, read_data.header->NumEncrypt);
    CHECK_EQUAL(1, read_data.GetEncryptions().size());
    CHECK_EQUAL(WBIDType::IEEE_80211, read_data.GetEncryptions()[0]->WBID);
    CHECK_EQUAL(0, read_data.GetEncryptions()[0]->EncryptionCapabilities);

    CHECK_EQUAL(3, read_data.GetDescriptors().size());
    STRNCMP_EQUAL("PROD2", (char *)read_data.GetDescriptors()[0]->utf8_value, 5);
    CHECK_EQUAL(DescriptorSubElementHeader::Type::HardwareVersion,
                read_data.GetDescriptors()[0]->GetType());
    CHECK_EQUAL(5, read_data.GetDescriptors()[0]->GetLength());

    STRNCMP_EQUAL("8.5.105.0, SPECL", (char *)read_data.GetDescriptors()[1]->utf8_value, 16);
    CHECK_EQUAL(DescriptorSubElementHeader::Type::ActiveSoftwareVersion,
                read_data.GetDescriptors()[1]->GetType());
    CHECK_EQUAL(16, read_data.GetDescriptors()[1]->GetLength());

    STRNCMP_EQUAL("bootldr", (char *)read_data.GetDescriptors()[2]->utf8_value, 7);
    CHECK_EQUAL(DescriptorSubElementHeader::Type::BootVersion,
                read_data.GetDescriptors()[2]->GetType());
    CHECK_EQUAL(7, read_data.GetDescriptors()[2]->GetLength());
}
