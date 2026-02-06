#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ResetResponse.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(ResetResponseTestsGroup){ //
                                     TEST_SETUP(){}

                                     TEST_TEARDOWN(){}
};

TEST(ResetResponseTestsGroup, ResetResponse_serialize) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    WritableVendorSpecificPayloadArray vendor_specific_payloads;
    vendor_specific_payloads.Add(123456, 789, "01234567890ABCDEF0123");

    WritableResetResponse write_data(ResultCode::MessageUnexpected_InvalidInCurrentState,
                                     vendor_specific_payloads);

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 39, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x21, 0x00, 0x04, 0x00, 0x00, 0x00, 0x12, 0x00, 0x25,
                                  0x00, 0x1B, 0x00, 0x01, 0xE2, 0x40, 0x03, 0x15, 0x30, 0x31,
                                  0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41,
                                  0x42, 0x43, 0x44, 0x45, 0x46, 0x30, 0x31, 0x32, 0x33 };

    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + 39 };
    ReadableResetResponse read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(ResultCode::MessageUnexpected_InvalidInCurrentState, read_data.result_code->type);

    CHECK_EQUAL(1, read_data.vendor_specific_payloads.Get().size());
    CHECK_EQUAL(123456, read_data.vendor_specific_payloads.Get()[0]->GetVendorIdentifier());
    CHECK_EQUAL(789, read_data.vendor_specific_payloads.Get()[0]->GetElementId());
    STRNCMP_EQUAL("01234567890ABCDEF0123",
                  (char *)read_data.vendor_specific_payloads.Get()[0]->value,
                  21);
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(ResetResponseTestsGroup, ResetResponse_deserialize_image_data) {
    // clang-format off
    uint8_t data[] = {
        // ===================================================================
        // 1. CAPWAP Header (8 байт)
        //    (HLEN=2, RID=0, WBID=0, Not a fragment)
        // ===================================================================
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
        0x00, 0x12,     // Message Type: 18 (ResetResponse)
        0x1A,           // Sequence Number: 0x1A (26) - ДОЛЖЕН СОВПАДАТЬ с номером в запросе
        0x00, 19,     // Message Element Length: 19
        0x00,           // Flags: 0

        // =================================================================
        // 3. Message Elements (33 байта)
        // =================================================================

        // --- Элемент 1: Result Code (Type 33) - 8 байт (ОБЯЗАТЕЛЬНЫЙ) ---
        // Назначение: Сообщить результат операции.
        0x00, 0x21,     // Type: 33
        0x00, 0x04,     // Length: 4
        0x00, 0x00,
        0x00, 0x00,     // Value: 0 (Success) - Все изменения применены успешно.      

        // --- Элемент 4: Vendor Specific Payload (Type 37) - 11 байт (ОПЦИОНАЛЬНЫЙ) ---
        // Назначение: Отправить специфичный для производителя ответ.
        0x00, 0x25,     // Type: 37
        0x00, 0x07,     // Length: 7 (4 байта Vendor ID + 2 байта Element ID + 1 байт данных)
        0x00, 0x00, 0x39, 0xE7, // Value: Vendor Identifier = 14823 (Aruba)
        0x01, 0x01,     // Value: Element ID = 0x0101 (тот же, что в запросе)
        0x01            // Value: Данные ответа, например, 0x01 = "ACK"
    };
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableResetResponse read_data;

    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);

    CHECK_EQUAL(ResultCode::Success, read_data.result_code->type);

    CHECK_EQUAL(1, read_data.vendor_specific_payloads.Get().size());
    CHECK_EQUAL(14823, read_data.vendor_specific_payloads.Get()[0]->GetVendorIdentifier());
    CHECK_EQUAL(0x0101, read_data.vendor_specific_payloads.Get()[0]->GetElementId());
    CHECK_EQUAL(7, read_data.vendor_specific_payloads.Get()[0]->GetLength());
    CHECK_EQUAL(0x01, read_data.vendor_specific_payloads.Get()[0]->value[0]);
    CHECK_EQUAL(0, read_data.unknown_elements);
}

TEST(ResetResponseTestsGroup, ResetResponse_deserialize_handle_unknown_element) {
    // clang-format off
    uint8_t data[] = {
        // ===================================================================
        // 1. CAPWAP Header (8 байт)
        //    (HLEN=2, RID=0, WBID=0, Not a fragment)
        // ===================================================================
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
        0x00, 0x12,     // Message Type: 18 (ResetResponse)
        0x1A,           // Sequence Number: 0x1A (26) - ДОЛЖЕН СОВПАДАТЬ с номером в запросе
        0x00, 18,     // Message Element Length: 19
        0x00,           // Flags: 0

        // =================================================================
        // 3. Message Elements (8 байт)
        // =================================================================

        // --- Элемент 1: Result Code (Type 33) - 8 байт (ОБЯЗАТЕЛЬНЫЙ) ---
        // Назначение: Сообщить результат операции.
        0x00, 0x21,     // Type: 33
        0x00, 0x04,     // Length: 4
        0x00, 0x00,
        0x00, 0x00,      // Value: 0 (Success) - Все изменения применены успешно.

        // 6. Unknown (5 байт)
        0xFF, 0xFF, 0x00, 0x01, 0x00,
        // 7. Unknown (5 байт)
        0xFF, 0xFE, 0x00, 0x01, 0x00,
    };
    // clang-format on
    RawData raw_data{ data + (sizeof(ClearHeader) + sizeof(ControlHeader)), data + sizeof(data) };

    ReadableResetResponse read_data;

    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, read_data.unknown_elements);
}
