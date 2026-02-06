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
    uint8_t data[] = {
        0x00, 0x21, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04,
    };
    RawData raw_data{ data, data + sizeof(data) };
    auto element = ResultCode::Deserialize(&raw_data);

    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::ResultCode, element->GetElementType());
    CHECK_EQUAL(ResultCode::Type::JoinFailure_ResourceDepletion, element->type);
}

TEST(ResultCodeTestsGroup, ResultCode_serialize) {
    uint8_t buffer[256] = {};
    ResultCode element_0{ ResultCode::Type::ImageDataError_InvalidChecksum };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 8, raw_data.current);
    const uint8_t reference[] = {
        0x00, 0x21, 0x00, 0x04, 0x00, 0x00, 0x00, 0x0E,
    };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto element = ResultCode::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(&buffer[0] + 8, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::ResultCode, element->GetElementType());
    CHECK_EQUAL(ResultCode::Type::ImageDataError_InvalidChecksum, element->type);
}
