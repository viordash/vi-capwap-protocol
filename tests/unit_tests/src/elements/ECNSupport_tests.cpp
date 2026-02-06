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
    uint8_t data[] = {
        0x00, 0x35, 0x00, 0x01, 0x01,
    };
    RawData raw_data{ data, data + sizeof(data) };
    auto element = ECNSupport::Deserialize(&raw_data);

    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::ECNSupport, element->GetElementType());
    CHECK_EQUAL(ECNSupport::Type::FullAndLimitedECN, element->type);
}

TEST(ECNSupportTestsGroup, ECNSupport_serialize) {
    uint8_t buffer[256] = {};
    ECNSupport element_0{ ECNSupport::Type::LimitedECN };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 5, raw_data.current);
    const uint8_t reference[] = {
        0x00, 0x35, 0x00, 0x01, 0x00,
    };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto element = ECNSupport::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(&buffer[0] + 5, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::ECNSupport, element->GetElementType());
    CHECK_EQUAL(ECNSupport::Type::LimitedECN, element->type);
}
