#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/ImageInformation.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(ImageInformationTestsGroup){ //
                                        TEST_SETUP(){}

                                        TEST_TEARDOWN(){}
};

TEST(ImageInformationTestsGroup, ImageInformation_deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Element Header (4 байта) ----
        0x00, 0x1A, // Element Type: Image Information (26)
        0x00, 0x14, // Element Length: 20 байт

        // ---- Element Value (20 байт) ----
        // File Size: 1048576 (0x00100000) в big-endian
        0x00, 0x10, 0x00, 0x00,

        // Hash: MD5 (16 байт) от "This is my firmware image."
        0xf1, 0x60, 0x4f, 0x5a, 0x43, 0x21, 0x03, 0xf8,
        0x88, 0x06, 0x2b, 0xf6, 0x9d, 0x6b, 0x2c, 0x68
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = ImageInformation::Deserialize(&raw_data);

    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::ImageInformation, element->GetElementType());
    CHECK_EQUAL(1048576, element->GetFileSize());
    CHECK_EQUAL(0xf1, element->file_hash[0]);
    CHECK_EQUAL(0x60, element->file_hash[1]);
}

TEST(ImageInformationTestsGroup, ImageInformation_serialize) {
    uint8_t buffer[256] = {};
    const uint8_t hash[] = { 0xF8, 0x1D, 0x4F, 0xAE, 0x7D, 0xEC, 0x11, 0xD0,
                             0xA7, 0x65, 0x00, 0xA0, 0xC9, 0x1E, 0x6B, 0xF6 };
    ImageInformation element_0{ 12345, hash };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 24, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x1A, 0x00, 0x14, 0x00, 0x00, 0x30, 0x39,
                                  0xF8, 0x1D, 0x4F, 0xAE, 0x7D, 0xEC, 0x11, 0xD0,
                                  0xA7, 0x65, 0x00, 0xA0, 0xC9, 0x1E, 0x6B, 0xF6 };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto element = ImageInformation::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(&buffer[0] + 24, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::ImageInformation, element->GetElementType());
    CHECK_EQUAL(12345, element->GetFileSize());
    MEMCMP_EQUAL(hash, element->file_hash, 16);
}
