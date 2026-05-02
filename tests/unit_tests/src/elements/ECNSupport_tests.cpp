#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/ECNSupport.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(ECNSupportTestsGroup){ //
                                  TEST_SETUP(){}

                                  TEST_TEARDOWN(){}
};

TEST(ECNSupportTestsGroup, ECNSupport_deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Element Header (4 bytes) ----
        0x00, 0x35, // Element Type: ECN Support (53)
        0x00, 0x01, // Element Length: 1 byte

        // ECN Support: Full and Limited ECN (1)
        0x01,
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    ReadableECNSupport read_data;
    CHECK_FALSE(read_data.IsPresent());
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::ECNSupport, read_data.GetElementType());
    CHECK_EQUAL(ECNSupport::Type::FullAndLimitedECN, read_data.Get()->type);
    CHECK_TRUE(read_data.IsPresent());
}

TEST(ECNSupportTestsGroup, ECNSupport_serialize) {
    uint8_t buffer[256] = {};
    WritableECNSupport element_0{ ECNSupport::Type::LimitedECN };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 5, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x35, 0x00, 0x01, 0x00 };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    ReadableECNSupport read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(&buffer[0] + 5, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::ECNSupport, read_data.GetElementType());
    CHECK_EQUAL(ECNSupport::Type::LimitedECN, read_data.Get()->type);
}
