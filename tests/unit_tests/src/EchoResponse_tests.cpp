#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "EchoResponse.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(EchoResponseTestsGroup){ //
                                    TEST_SETUP(){}

                                    TEST_TEARDOWN(){}
};

TEST(EchoResponseTestsGroup, EchoResponse_serialize) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    WritableEchoResponse write_data(VendorSpecificPayload::Dummy);

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 16 - (sizeof(ClearHeader) + sizeof(ControlHeader)), raw_data.current);
    const uint8_t reference[] = { 0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x0E, 0x2A, 0x00, 0x00, 0x00 };

    MEMCMP_EQUAL(buffer,
                 reference + (sizeof(ClearHeader) + sizeof(ControlHeader)),
                 sizeof(reference) - (sizeof(ClearHeader) + sizeof(ControlHeader)));

    raw_data = { buffer, buffer + 16 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };
    ReadableEchoResponse read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(EchoResponseTestsGroup, EchoResponse_serialize_with_VendorSpecificPayload) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    WritableVendorSpecificPayloadArray vendor_specific_payloads;
    vendor_specific_payloads.Add(123456, 789, "01234567890ABCDEF0123");

    WritableEchoResponse write_data(vendor_specific_payloads);

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 47 - (sizeof(ClearHeader) + sizeof(ControlHeader)), raw_data.current);

    raw_data = { buffer, buffer + 47 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };
    ReadableEchoResponse read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(1, read_data.vendor_specific_payloads.Get().size());
    CHECK_EQUAL(123456, read_data.vendor_specific_payloads.Get()[0]->GetVendorIdentifier());
    CHECK_EQUAL(789, read_data.vendor_specific_payloads.Get()[0]->GetElementId());
    STRNCMP_EQUAL("01234567890ABCDEF0123",
                  (char *)read_data.vendor_specific_payloads.Get()[0]->value,
                  21);
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(EchoResponseTestsGroup, EchoResponse_deserialize_image_data) {
    // clang-format off
    uint8_t data[] = {
        // ======== CAPWAP Header (8 байт) ========
        // Спецификация: RFC 5415, Section 4.3
        0x00,              // Preamble: Version 0, Type 0 (CAPWAP Header follows)
        0x10,              // HLEN (5 bits) = 2 (8 bytes), RID (top 3 bits) = 0
        0x42,              // RID (low 2 bits) = 1, WBID (5 bits) = 1 (802.11), T = 0
        0x00,              // F=0, L=0, W=0, M=0, K=0, Flags=0
        0x00, 0x00,        // Fragment ID = 0 (not fragmented)
        0x00, 0x00,        // Fragment Offset = 0, Rsvd = 0 (not fragmented)

        // ======== CAPWAP Control Header (8 байт) ========
        // Спецификация: RFC 5415, Section 4.5.1
        0x00, 0x00, 0x00, 0x0E, // 
        0x05,                   // Sequence Number = 5 (в ответ на запрос с тем же номером)
        0x00, 0x00,             // Message Element Length = 0 (нет элементов в этом сообщении)
        0x00,                   // Flags = 0
    };
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableEchoResponse read_data;

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);

    CHECK_EQUAL(0, read_data.vendor_specific_payloads.Get().size());
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(EchoResponseTestsGroup, EchoResponse_deserialize_handle_unknown_element) {
    // clang-format off
    uint8_t data[] = {
        // ======== CAPWAP Header (8 байт) ========
        // Спецификация: RFC 5415, Section 4.3
        0x00,              // Preamble: Version 0, Type 0 (CAPWAP Header follows)
        0x10,              // HLEN (5 bits) = 2 (8 bytes), RID (top 3 bits) = 0
        0x42,              // RID (low 2 bits) = 1, WBID (5 bits) = 1 (802.11), T = 0
        0x00,              // F=0, L=0, W=0, M=0, K=0, Flags=0
        0x00, 0x00,        // Fragment ID = 0 (not fragmented)
        0x00, 0x00,        // Fragment Offset = 0, Rsvd = 0 (not fragmented)

        // ======== CAPWAP Control Header (8 байт) ========
        // Спецификация: RFC 5415, Section 4.5.1
        0x00, 0x00, 0x00, 0x0E, // 
        0x05,                   // Sequence Number = 5 (в ответ на запрос с тем же номером)
        0x00, 0x00 + 10,             // Message Element Length = 0 (нет элементов в этом сообщении)
        0x00,                   // Flags = 0

        // 6. Unknown (5 байт)
        0xFF, 0xFF, 0x00, 0x01, 0x00,
        // 7. Unknown (5 байт)
        0xFF, 0xFE, 0x00, 0x01, 0x00,
    };
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableEchoResponse read_data;

    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, read_data.unknown_elements);
}
