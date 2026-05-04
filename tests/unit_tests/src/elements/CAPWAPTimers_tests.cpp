#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/CAPWAPTimers.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(CAPWAPTimersTestsGroup){ //
                                    TEST_SETUP(){}

                                    TEST_TEARDOWN(){}
};

TEST(CAPWAPTimersTestsGroup, CAPWAPTimers_deserialize) {
    uint8_t data[] = { 0x00, 0x0C, 0x00, 0x02, 42, 19 };
    RawData raw_data{ data, data + sizeof(data) };
    ReadableCAPWAPTimers read_data;
    CHECK_FALSE(read_data.IsPresent());
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::CAPWAPTimers, read_data.GetElementType());
    CHECK_EQUAL(42, read_data.Get()->Discovery);
    CHECK_EQUAL(19, read_data.Get()->EchoInterval);
    CHECK_TRUE(read_data.IsPresent());
}

TEST(CAPWAPTimersTestsGroup, CAPWAPTimers_serialize) {
    uint8_t buffer[256] = {};
    WritableCAPWAPTimers element_0{ 42, 19 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 6, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x0C, 0x00, 0x02, 42, 19 };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };

    ReadableCAPWAPTimers read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(&buffer[0] + 6, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::CAPWAPTimers, read_data.GetElementType());
    CHECK_EQUAL(42, read_data.Get()->Discovery);
    CHECK_EQUAL(19, read_data.Get()->EchoInterval);
}
