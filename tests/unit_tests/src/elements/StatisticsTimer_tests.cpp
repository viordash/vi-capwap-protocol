#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/StatisticsTimer.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(StatisticsTimerTestsGroup){ //
                                       TEST_SETUP(){}

                                       TEST_TEARDOWN(){}
};

TEST(StatisticsTimerTestsGroup, StatisticsTimer_deserialize) {
    uint8_t data[] = {
        // --- TLV Header (4 байта) ---
        0x00,
        0x24, // Type = 36
        0x00,
        0x02, // Length = 2

        // --- Value (2 байта) ---
        0x40,
        0x00 // Value = 16384 (в Big Endian)
    };
    RawData raw_data{ data, data + sizeof(data) };
    ReadableStatisticsTimer read_data;
    CHECK_FALSE(read_data.IsPresent());
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::StatisticsTimer, read_data.GetElementType());
    CHECK_EQUAL(16384, read_data.Get()->GetValue());
    CHECK_TRUE(read_data.IsPresent());
}

TEST(StatisticsTimerTestsGroup, StatisticsTimer_serialize) {
    uint8_t buffer[256] = {};
    WritableStatisticsTimer element_0{ 12345 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 6, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x24, 0x00, 0x02, 0x30, 0x39 };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    ReadableStatisticsTimer read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));
    CHECK_EQUAL(&buffer[0] + 6, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::StatisticsTimer, read_data.GetElementType());
    CHECK_EQUAL(12345, read_data.Get()->GetValue());
}
