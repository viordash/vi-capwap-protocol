#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/ResultCode.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(ResultCodeTestsGroup){ //
                                  TEST_SETUP(){}

                                  TEST_TEARDOWN(){}
};

TEST(ResultCodeTestsGroup, ResultCode_deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Element Header (4 bytes) ----
        0x00, 0x21,       // Element Type: Result Code (33)
        0x00, 0x04,       // Element Length: 4 bytes

        // Result Code: JoinFailure_ResourceDepletion (0x04000000)
        0x00, 0x00, 0x00, 0x04,
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    ReadableResultCode read_data;
    CHECK_FALSE(read_data.IsPresent());
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::ResultCode, read_data.GetElementType());
    CHECK_EQUAL(ResultCode::Type::JoinFailure_ResourceDepletion, read_data.Get()->type);
    CHECK_TRUE(read_data.IsPresent());
}

TEST(ResultCodeTestsGroup, ResultCode_serialize) {
    uint8_t buffer[256] = {};
    WritableResultCode element_0{ ResultCode::Type::ImageDataError_InvalidChecksum };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 8, raw_data.current);
    const uint8_t reference[] = {
        0x00, 0x21, 0x00, 0x04, 0x00, 0x00, 0x00, 0x0E,
    };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    ReadableResultCode read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(&buffer[0] + 8, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::ResultCode, read_data.GetElementType());
    CHECK_EQUAL(ResultCode::Type::ImageDataError_InvalidChecksum, read_data.Get()->type);
}

TEST(ResultCodeTestsGroup, ResultCode_vendor_specific_serialize) {
    uint8_t buffer[256] = {};
    WritableResultCode element_0{ ResultCode::Type::VendorSpecific_1 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 8, raw_data.current);
    const uint8_t reference[] = {
        0x00, 0x21, 0x00, 0x04, 0x00, 0x00, 0xF0, 0x00,
    };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    ReadableResultCode read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(ResultCode::Type::VendorSpecific_1, read_data.Get()->type);
}

TEST(ResultCodeTestsGroup, ResultCode_vendor_specific_deserialize_last) {
    // clang-format off
    uint8_t data[] = {
        0x00, 0x21,             // Element Type: Result Code (33)
        0x00, 0x04,             // Element Length: 4 bytes
        0x00, 0x00, 0xF0, 0x09, // Result Code: VendorSpecific_10 (0xF009)
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    ReadableResultCode read_data;

    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ResultCode::Type::VendorSpecific_10, read_data.Get()->type);
}

TEST(ResultCodeTestsGroup, ResultCode_deserialize_rejects_codes_outside_both_ranges) {
    // clang-format off
    uint8_t above_rfc5415[] = {
        0x00, 0x21, 0x00, 0x04,
        0x00, 0x00, 0x00, 0x17, // 23, one past DataTransferError
    };
    uint8_t below_vendor_specific[] = {
        0x00, 0x21, 0x00, 0x04,
        0x00, 0x00, 0xEF, 0xFF, // 0xEFFF, one before VendorSpecific_1
    };
    uint8_t above_vendor_specific[] = {
        0x00, 0x21, 0x00, 0x04,
        0x00, 0x00, 0xF0, 0x0A, // 0xF00A, one past VendorSpecific_10
    };
    // clang-format on

    RawData raw_data{ above_rfc5415, above_rfc5415 + sizeof(above_rfc5415) };
    ReadableResultCode read_data;
    CHECK_FALSE(read_data.Deserialize(&raw_data));

    raw_data = { below_vendor_specific, below_vendor_specific + sizeof(below_vendor_specific) };
    CHECK_FALSE(read_data.Deserialize(&raw_data));

    raw_data = { above_vendor_specific, above_vendor_specific + sizeof(above_vendor_specific) };
    CHECK_FALSE(read_data.Deserialize(&raw_data));
}
