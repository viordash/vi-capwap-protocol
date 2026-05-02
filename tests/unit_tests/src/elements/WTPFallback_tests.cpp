#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/WTPFallback.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(WTPFallbackTestsGroup){ //
                                   TEST_SETUP(){}

                                   TEST_TEARDOWN(){}
};

TEST(WTPFallbackTestsGroup, WTPFallback_deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Element Header (4 bytes) ----
        0x00, 0x28,       // Element Type: WTP Fallback (40)
        0x00, 0x01,       // Element Length: 1 byte

        // WTP Fallback: Enabled (1)
        0x01,
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    ReadableWTPFallback read_data;
    CHECK_FALSE(read_data.IsPresent());
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::WTPFallback, read_data.GetElementType());
    CHECK_EQUAL(WTPFallback::Mode::Enabled, read_data.Get()->mode);
    CHECK_TRUE(read_data.IsPresent());
}

TEST(WTPFallbackTestsGroup, WTPFallback_serialize) {
    uint8_t buffer[256] = {};
    WritableWTPFallback element_0{ WTPFallback::Mode::Reserved };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 5, raw_data.current);
    const uint8_t reference[] = {
        0x00, 0x28, 0x00, 0x01, 0x00,
    };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    ReadableWTPFallback read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(&buffer[0] + 5, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::WTPFallback, read_data.GetElementType());
    CHECK_EQUAL(WTPFallback::Mode::Reserved, read_data.Get()->mode);
}
