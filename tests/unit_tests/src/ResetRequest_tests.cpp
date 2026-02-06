#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ResetRequest.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(ResetRequestTestsGroup){ //
                                    TEST_SETUP(){}

                                    TEST_TEARDOWN(){}
};

TEST(ResetRequestTestsGroup, ResetRequest_serialize) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    WritableImageIdentifier image_identifier{ 123456, "abcdef Привет 1234" };

    WritableVendorSpecificPayloadArray vendor_specific_payloads;
    vendor_specific_payloads.Add(123456, 789, "01234567890ABCDEF0123");

    WritableResetRequest write_data(image_identifier, vendor_specific_payloads);

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 63, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x19, 0x00, 0x1C, 0x00, 0x01, 0xE2, 0x40, 0x61, 0x62, 0x63,
                                  0x64, 0x65, 0x66, 0x20, 0xD0, 0x9F, 0xD1, 0x80, 0xD0, 0xB8, 0xD0,
                                  0xB2, 0xD0, 0xB5, 0xD1, 0x82, 0x20, 0x31, 0x32, 0x33, 0x34, 0x00,
                                  0x25, 0x00, 0x1B, 0x00, 0x01, 0xE2, 0x40, 0x03, 0x15, 0x30, 0x31,
                                  0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42,
                                  0x43, 0x44, 0x45, 0x46, 0x30, 0x31, 0x32, 0x33 };

    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + 63 };
    ReadableResetRequest read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(24, read_data.image_identifier.GetData().size());
    CHECK_EQUAL(123456, read_data.image_identifier.GetVendorIdentifier());
    STRNCMP_EQUAL("abcdef Привет 1234", (char *)read_data.image_identifier.GetData().data(), 24);

    CHECK_EQUAL(1, read_data.vendor_specific_payloads.Get().size());
    CHECK_EQUAL(123456, read_data.vendor_specific_payloads.Get()[0]->GetVendorIdentifier());
    CHECK_EQUAL(789, read_data.vendor_specific_payloads.Get()[0]->GetElementId());
    STRNCMP_EQUAL("01234567890ABCDEF0123",
                  (char *)read_data.vendor_specific_payloads.Get()[0]->value,
                  21);
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(ResetRequestTestsGroup, ResetRequest_deserialize_image_data) {
    // clang-format off
    uint8_t data[] = {
        // ---- CAPWAP Header (8 байт) ----
        0x00,       // Preamble: Version 0, Type 0 (CAPWAP Header follows)
        0x10,       // HLEN: 2 (8 bytes), RID high bits
        0x42,       // RID low bits, WBID: 1 (802.11), T=0
        0x00,       // F=0, L=0, W=0, M=0, K=0, Flags=0
        0x00, 0x00, // Fragment ID
        0x00, 0x00, // Fragment Offset

        // =================================================================
        // 2. Control Message Header (8 байт)
        // =================================================================
        0x00, 0x00,
        0x00, 0x11,     // Message Type: 17 (ResetRequest)
        0x1A,           // Sequence Number: 0x1A (26)
        0x00, 40,     // Message Element Length: 40 байт
        0x00,           // Flags: 0

        
        // --- Элемент 10: Image Identifier (Type 25) - 24 байта ---
        0x00, 0x19,     // Type: 25
        0x00, 0x14,     // Length: 20 (4 байта Vendor ID + 16 байт имя)
        0x00, 0x00, 0x00, 0x09, // Value: Vendor Identifier = 9 (Cisco)
        'c', 'a', 'p', 'w', 'a', 'p', '-', 'f', // Value: "capwap-fw-v2.3.4"
        'w', '-', 'v', '2', '.', '3', '.', '4',

        // --- Элемент 12: Vendor Specific Payload (Type 37) - 14 байт ---
        0x00, 0x25,     // Type: 37
        0x00, 0x0A,     // Length: 10 (4 байта Vendor ID + 2 байта Element ID + 4 байта данных)
        0x00, 0x00, 0x39, 0xE7, // Value: Vendor Identifier = 14823 (Aruba)
        0x01, 0x01,     // Value: Element ID = 0x0101 (условный ID)
        0xDE, 0xAD, 0xBE, 0xEF  // Value: Произвольные данные
    };
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableResetRequest read_data;

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);

    CHECK_EQUAL(16, read_data.image_identifier.GetData().size());
    CHECK_EQUAL(9, read_data.image_identifier.GetVendorIdentifier());
    STRNCMP_EQUAL("capwap-fw-v2.3.4", (char *)read_data.image_identifier.GetData().data(), 24);

    CHECK_EQUAL(1, read_data.vendor_specific_payloads.Get().size());
    CHECK_EQUAL(14823, read_data.vendor_specific_payloads.Get()[0]->GetVendorIdentifier());
    CHECK_EQUAL(0x0101, read_data.vendor_specific_payloads.Get()[0]->GetElementId());
    const uint8_t vendor_payload[] = { 0xDE, 0xAD, 0xBE, 0xEF };
    MEMCMP_EQUAL(vendor_payload,
                 read_data.vendor_specific_payloads.Get()[0]->value,
                 sizeof(vendor_payload));
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(ResetRequestTestsGroup, ResetRequest_deserialize_handle_unknown_element) {
    // clang-format off
    uint8_t data[] = {
        // ---- CAPWAP Header (8 байт) ----
        0x00,       // Preamble: Version 0, Type 0 (CAPWAP Header follows)
        0x10,       // HLEN: 2 (8 bytes), RID high bits
        0x42,       // RID low bits, WBID: 1 (802.11), T=0
        0x00,       // F=0, L=0, W=0, M=0, K=0, Flags=0
        0x00, 0x00, // Fragment ID
        0x00, 0x00, // Fragment Offset

        // =================================================================
        // 2. Control Message Header (8 байт)
        // =================================================================
        0x00, 0x00,
        0x00, 0x11,     // Message Type: 17 (ResetRequest)
        0x1A,           // Sequence Number: 0x1A (26)
        0x00, 36,     // Message Element Length: 36 байт
        0x00,           // Flags: 0

        // --- Элемент 10: Image Identifier (Type 25) - 24 байта ---
        0x00, 0x19,     // Type: 25
        0x00, 0x14,     // Length: 20 (4 байта Vendor ID + 16 байт имя)
        0x00, 0x00, 0x00, 0x09, // Value: Vendor Identifier = 9 (Cisco)
        'c', 'a', 'p', 'w', 'a', 'p', '-', 'f', // Value: "capwap-fw-v2.3.4"
        'w', '-', 'v', '2', '.', '3', '.', '4',

        // 6. Unknown (5 байт)
        0xFF, 0xFF, 0x00, 0x01, 0x00,
        // 7. Unknown (5 байт)
        0xFF, 0xFE, 0x00, 0x01, 0x00,
    };
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableResetRequest read_data;

    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, read_data.unknown_elements);
}
