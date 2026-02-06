#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/ACTimestamp.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(ACTimestampTestsGroup){ //
                                   TEST_SETUP(){}

                                   TEST_TEARDOWN(){}
};

TEST(ACTimestampTestsGroup, ACTimestamp_deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Element Header (4 байта) ----
        0x00, 0x06, // Element Type: Idle Timestamp (6)
        0x00, 0x04, // Element Length: 4 байт

        // ACTimestamp: 1048576 (0x00100000) в big-endian
        0x00, 0x10, 0x00, 0x00,
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = ACTimestamp::Deserialize(&raw_data);

    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::ACTimestamp, element->GetElementType());
    CHECK_EQUAL(1048576, element->GetTimestamp());
}

TEST(ACTimestampTestsGroup, ACTimestamp_serialize) {
    uint8_t buffer[256] = {};
    ACTimestamp element_0{ 12345 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 8, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x06, 0x00, 0x04, 0x00, 0x00, 0x30, 0x39 };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto element = ACTimestamp::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(&buffer[0] + 8, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::ACTimestamp, element->GetElementType());
    CHECK_EQUAL(12345, element->GetTimestamp());
}
