#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/ImageIdentifier.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(ImageIdentifierTestsGroup){ //
                                       TEST_SETUP(){}

                                       TEST_TEARDOWN(){}
};

TEST(ImageIdentifierTestsGroup, ImageIdentifier_serialize) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    WritableImageIdentifier image_id{ 123456, "abcdef Привет 1234" };

    image_id.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 32, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x19, 0x00, 0x1C, 0x00, 0x01, 0xE2, 0x40, 0x61, 0x62, 0x63,
                                  0x64, 0x65, 0x66, 0x20, 0xD0, 0x9F, 0xD1, 0x80, 0xD0, 0xB8, 0xD0,
                                  0xB2, 0xD0, 0xB5, 0xD1, 0x82, 0x20, 0x31, 0x32, 0x33, 0x34 };

    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    ReadableImageIdentifier read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(&buffer[0] + sizeof(reference), raw_data.current);
    CHECK_EQUAL(24, read_data.GetData().size());
    CHECK_EQUAL(123456, read_data.GetVendorIdentifier());
    STRNCMP_EQUAL("abcdef Привет 1234", (char *)read_data.GetData().data(), 24);
}

TEST(ImageIdentifierTestsGroup, ImageIdentifier_take_ownership) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    std::string str("abcdef Привет 1234");

    WritableImageIdentifier image_id{ 123456, str };
    str.clear();

    image_id.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 32, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x19, 0x00, 0x1C, 0x00, 0x01, 0xE2, 0x40, 0x61, 0x62, 0x63,
                                  0x64, 0x65, 0x66, 0x20, 0xD0, 0x9F, 0xD1, 0x80, 0xD0, 0xB8, 0xD0,
                                  0xB2, 0xD0, 0xB5, 0xD1, 0x82, 0x20, 0x31, 0x32, 0x33, 0x34 };

    MEMCMP_EQUAL(buffer, reference, sizeof(reference));
}

TEST(ImageIdentifierTestsGroup, ImageIdentifier_deserialize) {
    // clang-format off
    uint8_t data[] = {
        //=========================================================================
        // 3. Message Element: Image Identifier (40 байт) - RFC 5415, 4.6.27
        //-------------------------------------------------------------------------
        0x00, 0x19,             // Type = 25 (Image Identifier)
        0x00, 0x24,             // Length = 36 (4 байта Vendor ID + 32 байта Data)
        // --- Value (36 байт) ---
        0x00, 0x00, 0x00, 0x09, // Vendor Identifier = 9 (Cisco Systems)
        // Data: "c1140-k9w7-mx.153-3.JD.bin" (32 символа)
        0x63, 0x31, 0x31, 0x34, 0x30, 0x2d, 0x6b, 0x39, 0x77, 0x37, 0x2d, 
        0x6d, 0x78, 0x2e, 0x31, 0x35, 0x33, 0x2d, 0x33, 0x2e, 0x4a, 0x44, 
        0x2e, 0x62, 0x69, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };

    ReadableImageIdentifier read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(9, read_data.GetVendorIdentifier());
    CHECK_EQUAL(32, read_data.GetData().size());
    STRNCMP_EQUAL("c1140-k9w7-mx.153-3.JD.bin", (char *)read_data.GetData().data(), 32);
}
