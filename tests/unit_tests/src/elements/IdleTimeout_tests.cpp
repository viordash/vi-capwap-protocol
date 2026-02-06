#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IdleTimeout.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(IdleTimeoutTestsGroup){ //
                                   TEST_SETUP(){}

                                   TEST_TEARDOWN(){}
};

TEST(IdleTimeoutTestsGroup, IdleTimeout_deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Element Header (4 байта) ----
        0x00, 0x17, // Element Type: Idle Timeout (23)
        0x00, 0x04, // Element Length: 4 байт

        // IdleTimeout: 1048576 (0x00100000) в big-endian
        0x00, 0x10, 0x00, 0x00,
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = IdleTimeout::Deserialize(&raw_data);

    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::IdleTimeout, element->GetElementType());
    CHECK_EQUAL(1048576, element->GetTimeout());
}

TEST(IdleTimeoutTestsGroup, IdleTimeout_serialize) {
    uint8_t buffer[256] = {};
    IdleTimeout element_0{ 12345 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 8, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x17, 0x00, 0x04, 0x00, 0x00, 0x30, 0x39 };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto element = IdleTimeout::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(&buffer[0] + 8, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::IdleTimeout, element->GetElementType());
    CHECK_EQUAL(12345, element->GetTimeout());
}
