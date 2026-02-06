#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/WTPRadioStatistics.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(WTPRadioStatisticsTestsGroup){ //
                                          TEST_SETUP(){}

                                          TEST_TEARDOWN(){}
};

TEST(WTPRadioStatisticsTestsGroup, WTPRadioStatistics_deserialize) {
    // clang-format off
     uint8_t data[] = {
        // ---- Заголовок элемента сообщения (4 байта) ----
        0x00, 0x2F,     // Type: 47 (0x2F) (WTP Radio Statistics)
        0x00, 0x14,     // Length: 20 байт (0x14) - это длина последующих данных

        // ---- Данные (Value) (20 байт) ----
        0x01,           // Radio ID: 1
        0x01,           // Last Failure Type: 1 (Software Failure)
        0x00, 0x0F,     // Reset Count: 15
        0x00, 0x03,     // SW Failure Count: 3
        0x00, 0x01,     // HW Failure Count: 1
        0x00, 0x00,     // Other Failure Count: 0
        0x00, 0x01,     // Unknown Failure Count: 1
        0x00, 0x96,     // Config Update Count: 150
        0x00, 0x2A,     // Channel Change Count: 42
        0x00, 0x00,     // Band Change Count: 0
        0xFF, 0xA4      // Current Noise Floor: -92 dBm
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = WTPRadioStatistics::Deserialize(&raw_data);

    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::WTPRadioStatistics, element->GetElementType());
    CHECK_EQUAL(1, element->GetRadioID());
    CHECK_EQUAL(WTPRadioStatistics::LastFailureType::SoftwareFailure,
                element->GetLastFailureType());
    CHECK_EQUAL(15, element->GetResetCount());
    CHECK_EQUAL(3, element->GetSWFailureCount());
    CHECK_EQUAL(1, element->GetHWFailureCount());
    CHECK_EQUAL(0, element->GetOtherFailureCount());
    CHECK_EQUAL(1, element->GetUnknownFailureCount());
    CHECK_EQUAL(150, element->GetConfigUpdateCount());
    CHECK_EQUAL(42, element->GetChannelChangeCount());
    CHECK_EQUAL(0, element->GetBandChangeCount());
    CHECK_EQUAL(-92, element->GetCurrentNoiseFloor());
}

TEST(WTPRadioStatisticsTestsGroup, WTPRadioStatistics_serialize) {
    uint8_t buffer[256] = {};
    WTPRadioStatistics element_0{ 11,    WTPRadioStatistics::LastFailureType::OtherFailure,
                                  12340, 12341,
                                  12342, 12343,
                                  12344, 12345,
                                  12346, 12347,
                                  -12348 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 24, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x2F, 0x00, 0x14, 0x0B, 0x03, 0x30, 0x34,
                                  0x30, 0x35, 0x30, 0x36, 0x30, 0x37, 0x30, 0x38,
                                  0x30, 0x39, 0x30, 0x3A, 0x30, 0x3B, 0xCF, 0xC4 };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto element = WTPRadioStatistics::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(&buffer[0] + 24, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::WTPRadioStatistics, element->GetElementType());
    CHECK_EQUAL(11, element->GetRadioID());
    CHECK_EQUAL(WTPRadioStatistics::LastFailureType::OtherFailure, element->GetLastFailureType());
    CHECK_EQUAL(12340, element->GetResetCount());
    CHECK_EQUAL(12341, element->GetSWFailureCount());
    CHECK_EQUAL(12342, element->GetHWFailureCount());
    CHECK_EQUAL(12343, element->GetOtherFailureCount());
    CHECK_EQUAL(12344, element->GetUnknownFailureCount());
    CHECK_EQUAL(12345, element->GetConfigUpdateCount());
    CHECK_EQUAL(12346, element->GetChannelChangeCount());
    CHECK_EQUAL(12347, element->GetBandChangeCount());
    CHECK_EQUAL(-12348, element->GetCurrentNoiseFloor());
}

TEST(WTPRadioStatisticsTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableWTPRadioStatisticsArray w_statistics;
    w_statistics.Add({ 1,
                       WTPRadioStatistics::LastFailureType::HardwareFailure,
                       40,
                       41,
                       42,
                       43,
                       44,
                       45,
                       46,
                       47,
                       -48 });

    w_statistics.Add({ 2,
                       WTPRadioStatistics::LastFailureType::SoftwareFailure,
                       340,
                       341,
                       342,
                       343,
                       344,
                       345,
                       346,
                       347,
                       -348 });
    w_statistics.Add({ 3,
                       WTPRadioStatistics::LastFailureType::OtherFailure,
                       2340,
                       2341,
                       2342,
                       2343,
                       2344,
                       2345,
                       2346,
                       2347,
                       -2348 });
    CHECK_FALSE(w_statistics.Empty());

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_statistics.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 72, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x2F, 0x00, 0x14, 0x01, 0x02, 0x00, 0x28, 0x00, 0x29, 0x00,
                                  0x2A, 0x00, 0x2B, 0x00, 0x2C, 0x00, 0x2D, 0x00, 0x2E, 0x00, 0x2F,
                                  0xFF, 0xD0, 0x00, 0x2F, 0x00, 0x14, 0x02, 0x01, 0x01, 0x54, 0x01,
                                  0x55, 0x01, 0x56, 0x01, 0x57, 0x01, 0x58, 0x01, 0x59, 0x01, 0x5A,
                                  0x01, 0x5B, 0xFE, 0xA4, 0x00, 0x2F, 0x00, 0x14, 0x03, 0x03, 0x09,
                                  0x24, 0x09, 0x25, 0x09, 0x26, 0x09, 0x27, 0x09, 0x28, 0x09, 0x29,
                                  0x09, 0x2A, 0x09, 0x2B, 0xF6, 0xD4 };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    ReadableWTPRadioStatisticsArray r_statistics;

    raw_data = { buffer, buffer + sizeof(reference) };
    CHECK_TRUE(r_statistics.Deserialize(&raw_data));
    CHECK_TRUE(r_statistics.Deserialize(&raw_data));
    CHECK_TRUE(r_statistics.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);

    CHECK_EQUAL(3, r_statistics.Get().size());
    CHECK_EQUAL(ElementHeader::ElementType::WTPRadioStatistics,
                r_statistics.Get()[0]->GetElementType());
    CHECK_EQUAL(1, r_statistics.Get()[0]->GetRadioID());
    CHECK_EQUAL(WTPRadioStatistics::LastFailureType::HardwareFailure,
                r_statistics.Get()[0]->GetLastFailureType());
    CHECK_EQUAL(40, r_statistics.Get()[0]->GetResetCount());
    CHECK_EQUAL(41, r_statistics.Get()[0]->GetSWFailureCount());
    CHECK_EQUAL(42, r_statistics.Get()[0]->GetHWFailureCount());
    CHECK_EQUAL(43, r_statistics.Get()[0]->GetOtherFailureCount());
    CHECK_EQUAL(44, r_statistics.Get()[0]->GetUnknownFailureCount());
    CHECK_EQUAL(45, r_statistics.Get()[0]->GetConfigUpdateCount());
    CHECK_EQUAL(46, r_statistics.Get()[0]->GetChannelChangeCount());
    CHECK_EQUAL(47, r_statistics.Get()[0]->GetBandChangeCount());
    CHECK_EQUAL(-48, r_statistics.Get()[0]->GetCurrentNoiseFloor());

    CHECK_EQUAL(ElementHeader::ElementType::WTPRadioStatistics,
                r_statistics.Get()[1]->GetElementType());
    CHECK_EQUAL(2, r_statistics.Get()[1]->GetRadioID());
    CHECK_EQUAL(WTPRadioStatistics::LastFailureType::SoftwareFailure,
                r_statistics.Get()[1]->GetLastFailureType());
    CHECK_EQUAL(340, r_statistics.Get()[1]->GetResetCount());
    CHECK_EQUAL(341, r_statistics.Get()[1]->GetSWFailureCount());
    CHECK_EQUAL(342, r_statistics.Get()[1]->GetHWFailureCount());
    CHECK_EQUAL(343, r_statistics.Get()[1]->GetOtherFailureCount());
    CHECK_EQUAL(344, r_statistics.Get()[1]->GetUnknownFailureCount());
    CHECK_EQUAL(345, r_statistics.Get()[1]->GetConfigUpdateCount());
    CHECK_EQUAL(346, r_statistics.Get()[1]->GetChannelChangeCount());
    CHECK_EQUAL(347, r_statistics.Get()[1]->GetBandChangeCount());
    CHECK_EQUAL(-348, r_statistics.Get()[1]->GetCurrentNoiseFloor());

    CHECK_EQUAL(ElementHeader::ElementType::WTPRadioStatistics,
                r_statistics.Get()[2]->GetElementType());
    CHECK_EQUAL(3, r_statistics.Get()[2]->GetRadioID());
    CHECK_EQUAL(WTPRadioStatistics::LastFailureType::OtherFailure,
                r_statistics.Get()[2]->GetLastFailureType());
    CHECK_EQUAL(2340, r_statistics.Get()[2]->GetResetCount());
    CHECK_EQUAL(2341, r_statistics.Get()[2]->GetSWFailureCount());
    CHECK_EQUAL(2342, r_statistics.Get()[2]->GetHWFailureCount());
    CHECK_EQUAL(2343, r_statistics.Get()[2]->GetOtherFailureCount());
    CHECK_EQUAL(2344, r_statistics.Get()[2]->GetUnknownFailureCount());
    CHECK_EQUAL(2345, r_statistics.Get()[2]->GetConfigUpdateCount());
    CHECK_EQUAL(2346, r_statistics.Get()[2]->GetChannelChangeCount());
    CHECK_EQUAL(2347, r_statistics.Get()[2]->GetBandChangeCount());
    CHECK_EQUAL(-2348, r_statistics.Get()[2]->GetCurrentNoiseFloor());
}
TEST(WTPRadioStatisticsTestsGroup, Add_array_of_items_is_unique) {
    uint8_t buffer[2048] = {};

    WritableWTPRadioStatisticsArray w_statistics;
    w_statistics.Add({ 1,
                       WTPRadioStatistics::LastFailureType::HardwareFailure,
                       40,
                       41,
                       42,
                       43,
                       44,
                       45,
                       46,
                       47,
                       -48 });
    w_statistics.Add({ 1,
                       WTPRadioStatistics::LastFailureType::HardwareFailure,
                       40,
                       41,
                       42,
                       43,
                       44,
                       45,
                       46,
                       47,
                       -48 });
    w_statistics.Add({ 1,
                       WTPRadioStatistics::LastFailureType::HardwareFailure,
                       40,
                       41,
                       42,
                       43,
                       44,
                       45,
                       46,
                       47,
                       -48 });

    w_statistics.Add({ 2,
                       WTPRadioStatistics::LastFailureType::SoftwareFailure,
                       340,
                       341,
                       342,
                       343,
                       344,
                       345,
                       346,
                       347,
                       -348 });

    w_statistics.Add({ 2,
                       WTPRadioStatistics::LastFailureType::SoftwareFailure,
                       340,
                       341,
                       342,
                       343,
                       344,
                       345,
                       346,
                       347,
                       -348 });

    w_statistics.Add({ 2,
                       WTPRadioStatistics::LastFailureType::SoftwareFailure,
                       340,
                       341,
                       342,
                       343,
                       344,
                       345,
                       346,
                       347,
                       -348 });

    CHECK_FALSE(w_statistics.Empty());

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_statistics.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableWTPRadioStatisticsArray r_statistics;

    CHECK_TRUE(r_statistics.Deserialize(&raw_data));
    CHECK_TRUE(r_statistics.Deserialize(&raw_data));
    CHECK_FALSE(r_statistics.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);

    CHECK_EQUAL(2, r_statistics.Get().size());
    CHECK_EQUAL(ElementHeader::ElementType::WTPRadioStatistics,
                r_statistics.Get()[0]->GetElementType());
    CHECK_EQUAL(1, r_statistics.Get()[0]->GetRadioID());
    CHECK_EQUAL(WTPRadioStatistics::LastFailureType::HardwareFailure,
                r_statistics.Get()[0]->GetLastFailureType());
    CHECK_EQUAL(40, r_statistics.Get()[0]->GetResetCount());
    CHECK_EQUAL(41, r_statistics.Get()[0]->GetSWFailureCount());
    CHECK_EQUAL(42, r_statistics.Get()[0]->GetHWFailureCount());
    CHECK_EQUAL(43, r_statistics.Get()[0]->GetOtherFailureCount());
    CHECK_EQUAL(44, r_statistics.Get()[0]->GetUnknownFailureCount());
    CHECK_EQUAL(45, r_statistics.Get()[0]->GetConfigUpdateCount());
    CHECK_EQUAL(46, r_statistics.Get()[0]->GetChannelChangeCount());
    CHECK_EQUAL(47, r_statistics.Get()[0]->GetBandChangeCount());
    CHECK_EQUAL(-48, r_statistics.Get()[0]->GetCurrentNoiseFloor());

    CHECK_EQUAL(ElementHeader::ElementType::WTPRadioStatistics,
                r_statistics.Get()[1]->GetElementType());
    CHECK_EQUAL(2, r_statistics.Get()[1]->GetRadioID());
    CHECK_EQUAL(WTPRadioStatistics::LastFailureType::SoftwareFailure,
                r_statistics.Get()[1]->GetLastFailureType());
    CHECK_EQUAL(340, r_statistics.Get()[1]->GetResetCount());
    CHECK_EQUAL(341, r_statistics.Get()[1]->GetSWFailureCount());
    CHECK_EQUAL(342, r_statistics.Get()[1]->GetHWFailureCount());
    CHECK_EQUAL(343, r_statistics.Get()[1]->GetOtherFailureCount());
    CHECK_EQUAL(344, r_statistics.Get()[1]->GetUnknownFailureCount());
    CHECK_EQUAL(345, r_statistics.Get()[1]->GetConfigUpdateCount());
    CHECK_EQUAL(346, r_statistics.Get()[1]->GetChannelChangeCount());
    CHECK_EQUAL(347, r_statistics.Get()[1]->GetBandChangeCount());
    CHECK_EQUAL(-348, r_statistics.Get()[1]->GetCurrentNoiseFloor());
}