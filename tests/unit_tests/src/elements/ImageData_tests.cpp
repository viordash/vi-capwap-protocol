#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/ImageData.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(ImageDataTestsGroup){ //
                                 TEST_SETUP(){}

                                 TEST_TEARDOWN(){}
};

TEST(ImageDataTestsGroup, ImageData_serialize) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    const uint8_t data[] = { 0xF8, 0x1D, 0x4F, 0xAE, 0x7D, 0xEC, 0x11, 0xD0,
                             0xA7, 0x65, 0x00, 0xA0, 0xC9, 0x1E, 0x6B, 0xF6 };

    WritableImageData image_data{ ImageDataHeader::Type::ImageDataIsIncluded, data };

    image_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 21, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x18, 0x00, 0x11, 0x01, 0xF8, 0x1D, 0x4F, 0xAE, 0x7D, 0xEC,
                                  0x11, 0xD0, 0xA7, 0x65, 0x00, 0xA0, 0xC9, 0x1E, 0x6B, 0xF6 };

    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto element = ReadableImageData::Deserialize(&raw_data);
    CHECK(element != nullptr);

    CHECK_EQUAL(&buffer[0] + sizeof(reference), raw_data.current);
    CHECK_EQUAL(17, element->GetLength());
    CHECK_EQUAL(ImageDataHeader::Type::ImageDataIsIncluded, element->type);
    MEMCMP_EQUAL(data, element->data, 16);
    CHECK_EQUAL(16, element->GetDataLenght());
}

TEST(ImageDataTestsGroup, ImageData_deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Message Element Header ----
        0x00, 0x18, // Type: Image Data (24)
        0x00, 0x1B, // Length: 27 байт (1 байт Data Type + 26 байт Data)

        // ---- Message Element Value ----
        0x01,       // Data Type: Image data is included (1)
        
        // Data: "This is a test data chunk." (26 байт)
        0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x61, 0x20, 0x74, 0x65,
        0x73, 0x74, 0x20, 0x64, 0x61, 0x74, 0x61, 0x20, 0x63, 0x68, 0x75, 0x6E,
        0x6B, 0x2E
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };

    auto element = ReadableImageData::Deserialize(&raw_data);
    CHECK(element != nullptr);

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(27, element->GetLength());
    CHECK_EQUAL(ImageDataHeader::Type::ImageDataIsIncluded, element->type);

    const uint8_t image_data[] = { 0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x61,
                                   0x20, 0x74, 0x65, 0x73, 0x74, 0x20, 0x64, 0x61, 0x74,
                                   0x61, 0x20, 0x63, 0x68, 0x75, 0x6E, 0x6B, 0x2E };

    MEMCMP_EQUAL(image_data, element->data, 26);
    CHECK_EQUAL(26, element->GetDataLenght());
}
