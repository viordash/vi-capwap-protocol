#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/DiscoveryType.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(DiscoveryTypeTestsGroup){ //
                                     TEST_SETUP(){}

                                     TEST_TEARDOWN(){}
};

TEST(DiscoveryTypeTestsGroup, DiscoveryType_deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Element Header (4 bytes) ----
        0x00, 0x14, // Element Type: Discovery Type (20)
        0x00, 0x01, // Element Length: 1 byte

        // Discovery Type: Static Configuration (1)
        0x01,
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    ReadableDiscoveryType read_data;
    CHECK_FALSE(read_data.IsPresent());
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::DiscoveryType, read_data.GetElementType());
    CHECK_EQUAL(DiscoveryType::Type::StaticConfiguration, read_data.Get()->type);
    CHECK_TRUE(read_data.IsPresent());
}

TEST(DiscoveryTypeTestsGroup, DiscoveryType_serialize) {
    uint8_t buffer[256] = {};
    WritableDiscoveryType element_0{ DiscoveryType::Type::DNS };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 5, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x14, 0x00, 0x01, 0x03 };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    ReadableDiscoveryType read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(&buffer[0] + 5, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::DiscoveryType, read_data.GetElementType());
    CHECK_EQUAL(DiscoveryType::Type::DNS, read_data.Get()->type);
}
