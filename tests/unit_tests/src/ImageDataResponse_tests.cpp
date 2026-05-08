#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ImageDataResponse.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(ImageDataResponseTestsGroup){ //
                                         TEST_SETUP(){}

                                         TEST_TEARDOWN(){}
};

TEST(ImageDataResponseTestsGroup, ImageDataResponse_serialize) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };
    const uint8_t hash[] = { 0xF8, 0x1D, 0x4F, 0xAE, 0x7D, 0xEC, 0x11, 0xD0,
                             0xA7, 0x65, 0x00, 0xA0, 0xC9, 0x1E, 0x6B, 0xF6 };
    {
        WritableVendorSpecificPayloadArray vendor_specific_payloads;
        vendor_specific_payloads.Add(123456, 789, "01234567890ABCDEF0123");

        WritableImageInformation image_information{ 12345, hash };

        IWritableImageDataResponseOptionalElement *const elems_1[] = { &vendor_specific_payloads, &image_information };
        WritableImageDataResponse write_data(ResultCode::Type::Success,
            elems_1);

        write_data.Serialize(&raw_data);
    }
    CHECK_EQUAL(&buffer[0] + 79 - (sizeof(ClearHeader) + sizeof(ControlHeader)), raw_data.current);
    const uint8_t reference[] = { 0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x10, 0x2A, 0x00, 0x3F, 0x00, 0x00, 0x21, 0x00, 0x04,
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x25, 0x00, 0x1B, 0x00, 0x01,
                                  0xE2, 0x40, 0x03, 0x15, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
                                  0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45,
                                  0x46, 0x30, 0x31, 0x32, 0x33, 0x00, 0x1A, 0x00, 0x14, 0x00,
                                  0x00, 0x30, 0x39, 0xF8, 0x1D, 0x4F, 0xAE, 0x7D, 0xEC, 0x11,
                                  0xD0, 0xA7, 0x65, 0x00, 0xA0, 0xC9, 0x1E, 0x6B, 0xF6 };

    MEMCMP_EQUAL(buffer,
                 reference + (sizeof(ClearHeader) + sizeof(ControlHeader)),
                 sizeof(reference) - (sizeof(ClearHeader) + sizeof(ControlHeader)));

    raw_data = { buffer, buffer + 79 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };

    ReadableVendorSpecificPayloadArray vendor_specific_payloads;
    ReadableImageInformation image_information;
    IReadableImageDataResponseOptionalElement *const elems_2[] = { &vendor_specific_payloads, &image_information };
    ReadableImageDataResponse read_data(elems_2);

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(ResultCode::Type::Success, read_data.result_code.Get()->type);

    CHECK_TRUE(vendor_specific_payloads.IsPresent());
    CHECK_EQUAL(1, vendor_specific_payloads.Get().size());
    CHECK_EQUAL(123456, vendor_specific_payloads.Get()[0]->GetVendorIdentifier());
    CHECK_EQUAL(789, vendor_specific_payloads.Get()[0]->GetElementId());
    STRNCMP_EQUAL("01234567890ABCDEF0123", (char *)vendor_specific_payloads.Get()[0]->value, 21);

    CHECK_TRUE(image_information.IsPresent());
    CHECK_EQUAL(12345, image_information.Get()->GetFileSize());
    MEMCMP_EQUAL(hash, image_information.Get()->file_hash, 16);
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(ImageDataResponseTestsGroup, ImageDataResponse_deserialize_after_initiate_download) {
    // clang-format off
    uint8_t data[] = {
        // ---- CAPWAP Header (8 байт) ----
        0x00,       // Preamble: Version 0, Type 0 (CAPWAP Header follows)
        0x10,       // HLEN: 2 (8 bytes), RID high bits
        0x42,       // RID low bits (RID=1), WBID: 1 (802.11), T=0
        0x00,       // F=0, L=0, W=0, M=0, K=0, Flags=0
        0x00, 0x00, // Fragment ID
        0x00, 0x00, // Fragment Offset

        // ---- Control Header (8 байт) ----
        0x00, 0x00, 0x00, 0x10, // Message Type: Image Data Response (16)
        0x0A,                   // Sequence Number: 10 (должен соответствовать запросу от WTP)
        0x00, 0x20,             // Message Element Length: 32 байта (8 байт Result Code + 24 байта Image Info)
        0x00,                   // Flags: 0

        // ---- Message Elements (32 байта) ----

        // 1. Result Code (Type 33, Length 4) - Обязательный
        0x00, 0x21, // Element Type: Result Code (33)
        0x00, 0x04, // Element Length: 4
        0x00, 0x00, 0x00, 0x00, // Value: Success (0)

        // 2. Image Information (Type 26, Length 20)
        0x00, 0x1A, // Element Type: Image Information (26)
        0x00, 0x14, // Element Length: 20
        // File Size: 1048576 (0x00100000)
        0x00, 0x10, 0x00, 0x00,
        // Hash: MD5 от строки "This is my firmware image."
        0xf1, 0x60, 0x4f, 0x5a, 0x43, 0x21, 0x03, 0xf8,
        0x88, 0x06, 0x2b, 0xf6, 0x9d, 0x6b, 0x2c, 0x68
    };
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableVendorSpecificPayloadArray vendor_specific_payloads;
    ReadableImageInformation image_information;
    IReadableImageDataResponseOptionalElement *const elems_3[] = { &vendor_specific_payloads, &image_information };
    ReadableImageDataResponse read_data(elems_3);

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);

    CHECK_EQUAL(ResultCode::Type::Success, read_data.result_code.Get()->type);

    CHECK_TRUE(image_information.IsPresent());
    CHECK_EQUAL(1048576, image_information.Get()->GetFileSize());
    CHECK_EQUAL(0xf1, image_information.Get()->file_hash[0]);
    CHECK_EQUAL(0x60, image_information.Get()->file_hash[1]);

    CHECK_FALSE(vendor_specific_payloads.IsPresent());
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(ImageDataResponseTestsGroup, ImageDataResponse_deserialize_handle_unknown_element) {
    // clang-format off
    uint8_t data[] = {
        // ---- CAPWAP Header (8 байт) ----
        0x00,       // Preamble: Version 0, Type 0 (CAPWAP Header follows)
        0x10,       // HLEN: 2 (8 bytes), RID high bits
        0x42,       // RID low bits (RID=1), WBID: 1 (802.11), T=0
        0x00,       // F=0, L=0, W=0, M=0, K=0, Flags=0
        0x00, 0x00, // Fragment ID
        0x00, 0x00, // Fragment Offset

        // ---- Control Header (8 байт) ----
        0x00, 0x00, 0x00, 0x10, // Message Type: Image Data Response (16)
        0x51,                   // Sequence Number: 81 (должен соответствовать запросу от AC)
        0x00, 0x08 + 5 + 5,             // Message Element Length: 8 + 5 + 5 байт
        0x00,                   // Flags: 0

        // ---- Message Element (8 байт) ----

        // 1. Result Code (Type 33, Length 4) - Обязательный
        0x00, 0x21, // Element Type: Result Code (33)
        0x00, 0x04, // Element Length: 4
        0x00, 0x00, 0x00, 0x00,  // Value: Success (0)

        // 6. Unknown (5 байт)
        0xFF, 0xFF, 0x00, 0x01, 0x00,
        // 7. Unknown (5 байт)
        0xFF, 0xFE, 0x00, 0x01, 0x00,
    };
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableVendorSpecificPayloadArray vendor_specific_payloads;
    ReadableImageInformation image_information;
    IReadableImageDataResponseOptionalElement *const elems_4[] = { &vendor_specific_payloads, &image_information };
    ReadableImageDataResponse read_data(elems_4);

    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_FALSE(vendor_specific_payloads.IsPresent());
    CHECK_FALSE(image_information.IsPresent());
    CHECK_EQUAL(2, read_data.unknown_elements);
}
TEST(ImageDataResponseTestsGroup, GetOptionalElement) {
    ReadableVendorSpecificPayloadArray vendor_specific_payloads;
    IReadableImageDataResponseOptionalElement *const elems_5[] = { &vendor_specific_payloads };
    ReadableImageDataResponse read_data(elems_5);

    CHECK_EQUAL(&vendor_specific_payloads,
                read_data.GetOptionalElement<ReadableVendorSpecificPayloadArray>(
                    ElementHeader::VendorSpecificPayload));

    CHECK(read_data.GetOptionalElement<IReadableElement>((ElementHeader::ElementType)0xFFFF) ==
          nullptr);
}

TEST(ImageDataResponseTestsGroup, MessageTypeIdentification) {
    WritableImageDataResponse write_data(ResultCode::Type::Success, nonstd::span<IWritableImageDataResponseOptionalElement *const>{});

    CHECK_EQUAL(ControlHeader::ImageDataResponse, write_data.GetMessageType());
    CHECK_EQUAL(ControlHeader::ImageDataRequest, write_data.GetRequestMessageType());
}
