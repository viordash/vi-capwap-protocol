#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ImageDataRequest.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(ImageDataRequestTestsGroup){ //
                                        TEST_SETUP(){}

                                        TEST_TEARDOWN(){}
};

TEST(ImageDataRequestTestsGroup, ImageDataRequest_serialize) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    CapwapTransportProtocol capwap_transport_protocol{ CapwapTransportProtocol::Type::UDP };

    const uint8_t data[] = { 0xF8, 0x1D, 0x4F, 0xAE, 0x7D, 0xEC, 0x11, 0xD0,
                             0xA7, 0x65, 0x00, 0xA0, 0xC9, 0x1E, 0x6B, 0xF6 };

    WritableImageData image_data{ ImageDataHeader::Type::ImageDataIsIncluded, data };

    WritableVendorSpecificPayloadArray vendor_specific_payloads;
    vendor_specific_payloads.Add(123456, 789, "01234567890ABCDEF0123");

    WritableImageIdentifier image_identifier{ 123456, "1232344" };
    InitiateDownload initiate_download;

    WritableImageDataRequest write_data(&capwap_transport_protocol,
                                        &image_data,
                                        vendor_specific_payloads,
                                        &image_identifier,
                                        &initiate_download);

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 92 - (sizeof(ClearHeader) + sizeof(ControlHeader)), raw_data.current);
    const uint8_t reference[] = { 0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x0F, 0x2A, 0x00, 0x4C, 0x00, 0x00, 0x33, 0x00, 0x01, 0x02, 0x00,
                                  0x18, 0x00, 0x11, 0x01, 0xF8, 0x1D, 0x4F, 0xAE, 0x7D, 0xEC, 0x11,
                                  0xD0, 0xA7, 0x65, 0x00, 0xA0, 0xC9, 0x1E, 0x6B, 0xF6, 0x00, 0x25,
                                  0x00, 0x1B, 0x00, 0x01, 0xE2, 0x40, 0x03, 0x15, 0x30, 0x31, 0x32,
                                  0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43,
                                  0x44, 0x45, 0x46, 0x30, 0x31, 0x32, 0x33, 0x00, 0x19, 0x00, 0x0B,
                                  0x00, 0x01, 0xE2, 0x40, 0x31, 0x32, 0x33, 0x32, 0x33, 0x34, 0x34,
                                  0x00, 0x1B, 0x00, 0x00 };

    MEMCMP_EQUAL(buffer,
                 reference + (sizeof(ClearHeader) + sizeof(ControlHeader)),
                 sizeof(reference) - (sizeof(ClearHeader) + sizeof(ControlHeader)));

    raw_data = { buffer, buffer + 92 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };
    ReadableImageDataRequest read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(CapwapTransportProtocol::Type::UDP, read_data.capwap_transport_protocol->type);

    CHECK_EQUAL(17, read_data.image_data->GetLength());
    CHECK_EQUAL(ImageDataHeader::Type::ImageDataIsIncluded, read_data.image_data->type);
    MEMCMP_EQUAL(image_data.data, read_data.image_data->data, 16);

    CHECK_EQUAL(1, read_data.vendor_specific_payloads.Get().size());
    CHECK_EQUAL(123456, read_data.vendor_specific_payloads.Get()[0]->GetVendorIdentifier());
    CHECK_EQUAL(789, read_data.vendor_specific_payloads.Get()[0]->GetElementId());
    STRNCMP_EQUAL("01234567890ABCDEF0123",
                  (char *)read_data.vendor_specific_payloads.Get()[0]->value,
                  21);

    CHECK_EQUAL(7, read_data.image_identifier.GetData().size());
    CHECK_EQUAL(123456, read_data.image_identifier.GetVendorIdentifier());
    STRNCMP_EQUAL("1232344", (char *)read_data.image_identifier.GetData().data(), 7);

    CHECK(read_data.initiate_download != nullptr);
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(ImageDataRequestTestsGroup, ImageDataRequest_deserialize_initiate_download) {
    // clang-format off
uint8_t data[] = {
        // ---- CAPWAP Header (8 байт) ----
        0x00,       // Preamble: Version 0, Type 0 (CAPWAP Header follows)
        0x10,       // HLEN: 2 (8 bytes), RID high bits
        0x42,       // RID low bits, WBID: 1 (802.11), T=0
        0x00,       // F=0, L=0, W=0, M=0, K=0, Flags=0
        0x00, 0x00, // Fragment ID
        0x00, 0x00, // Fragment Offset

        // ---- Control Header (8 байт) ----
        0x00, 0x00, 0x00, 0x0F, // Message Type: Image Data Request (15)
        0x0A,                   // Sequence Number: 10
        0x00, 0x15,             // Message Element Length: 21 байт (17 + 4)
        0x00,                   // Flags: 0

        // ---- Message Elements (21 байт) ----
        
        // 1. Image Identifier (Type 25, Length 13)
        0x00, 0x19,             // Element Type: Image Identifier (25)
        0x00, 0x0D,             // Element Length: 13
        0x00, 0x00, 0x00, 0x00, // Vendor Identifier: 0 (IETF)
        // Value: "FW_v1.2.3" (9 байт)
        0x46, 0x57, 0x5F, 0x76, 0x31, 0x2E, 0x32, 0x2E, 0x33,

        // 2. Initiate Download (Type 27, Length 0)
        0x00, 0x1B,             // Element Type: Initiate Download (27)
        0x00, 0x00              // Element Length: 0  
    };
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableImageDataRequest read_data;

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);

    CHECK(read_data.capwap_transport_protocol == nullptr);
    CHECK(read_data.image_data == nullptr);
    CHECK_EQUAL(0, read_data.vendor_specific_payloads.Get().size());

    CHECK_EQUAL(9, read_data.image_identifier.GetData().size());
    CHECK_EQUAL(0, read_data.image_identifier.GetVendorIdentifier());
    STRNCMP_EQUAL("FW_v1.2.3", (char *)read_data.image_identifier.GetData().data(), 9);

    CHECK(read_data.initiate_download != nullptr);
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(ImageDataRequestTestsGroup, ImageDataRequest_deserialize_image_data) {
    // clang-format off
    uint8_t data[] = {
        // ---- CAPWAP Header (8 байт) ----
        0x00,       // Preamble: Version 0, Type 0 (CAPWAP Header follows)
        0x10,       // HLEN: 2 (8 bytes), RID high bits
        0x42,       // RID low bits, WBID: 1 (802.11), T=0
        0x00,       // F=0, L=0, W=0, M=0, K=0, Flags=0
        0x00, 0x00, // Fragment ID
        0x00, 0x00, // Fragment Offset

        // ---- Control Header (8 байт) ----
        0x00, 0x00, 0x00, 0x0F, // Message Type: Image Data Request (15)
        0x51,                   // Sequence Number: 81
        0x00, 0x1F,             // Message Element Length: 31 байт
        0x00,                   // Flags: 0

        // ---- Message Element (31 байт) ----
        
        // 1. Image Data (Type 24, Length 27)
        0x00, 0x18, // Element Type: Image Data (24)
        0x00, 0x1B, // Element Length: 27
        0x01,       // Data Type: Image data is included (1)
        // Value: "This is a test data chunk." (26 байт)
        0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x61, 0x20, 0x74, 0x65,
        0x73, 0x74, 0x20, 0x64, 0x61, 0x74, 0x61, 0x20, 0x63, 0x68, 0x75, 0x6E,
        0x6B, 0x2E        
    };
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableImageDataRequest read_data;

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);

    CHECK(read_data.capwap_transport_protocol == nullptr);

    CHECK_EQUAL(27, read_data.image_data->GetLength());
    CHECK_EQUAL(ImageDataHeader::Type::ImageDataIsIncluded, read_data.image_data->type);
    const uint8_t image_data[] = { 0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x61,
                                   0x20, 0x74, 0x65, 0x73, 0x74, 0x20, 0x64, 0x61, 0x74,
                                   0x61, 0x20, 0x63, 0x68, 0x75, 0x6E, 0x6B, 0x2E };
    MEMCMP_EQUAL(image_data, read_data.image_data->data, 26);

    CHECK_EQUAL(0, read_data.vendor_specific_payloads.Get().size());
    CHECK_EQUAL(0, read_data.image_identifier.GetData().size());
    CHECK(read_data.initiate_download == nullptr);
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(ImageDataRequestTestsGroup, ImageDataRequest_deserialize_handle_unknown_element) {
    // clang-format off
    uint8_t data[] = {
        // ---- CAPWAP Header (8 байт) ----
        0x00,       // Preamble: Version 0, Type 0 (CAPWAP Header follows)
        0x10,       // HLEN: 2 (8 bytes), RID high bits
        0x42,       // RID low bits, WBID: 1 (802.11), T=0
        0x00,       // F=0, L=0, W=0, M=0, K=0, Flags=0
        0x00, 0x00, // Fragment ID
        0x00, 0x00, // Fragment Offset

        // ---- Control Header (8 байт) ----
        0x00, 0x00, 0x00, 0x0F, // Message Type: Image Data Request (15)
        0x0A,                   // Sequence Number: 10
        0x00, 0x15 + 5 + 5,             // Message Element Length: 21 байт (17 + 4 + 5 + 5)
        0x00,                   // Flags: 0

        // ---- Message Elements (21 байт) ----
        
        // 1. Image Identifier (Type 25, Length 13)
        0x00, 0x19,             // Element Type: Image Identifier (25)
        0x00, 0x0D,             // Element Length: 13
        0x00, 0x00, 0x00, 0x00, // Vendor Identifier: 0 (IETF)
        // Value: "FW_v1.2.3" (9 байт)
        0x46, 0x57, 0x5F, 0x76, 0x31, 0x2E, 0x32, 0x2E, 0x33,

        // 2. Initiate Download (Type 27, Length 0)
        0x00, 0x1B,             // Element Type: Initiate Download (27)
        0x00, 0x00,              // Element Length: 0  

        // 6. Unknown (5 байт)
        0xFF, 0xFF, 0x00, 0x01, 0x00,
        // 7. Unknown (5 байт)
        0xFF, 0xFE, 0x00, 0x01, 0x00,
    };
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableImageDataRequest read_data;

    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, read_data.unknown_elements);
}
