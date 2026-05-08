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
