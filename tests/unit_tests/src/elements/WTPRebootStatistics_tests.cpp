#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/WTPRebootStatistics.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(WTPRebootStatisticsTestsGroup){ //
                                           TEST_SETUP(){}

                                           TEST_TEARDOWN(){}
};

TEST(WTPRebootStatisticsTestsGroup, WTPRebootStatistics_deserialize) {
    // clang-format off
    // Юнит-тест: WTP Reboot Statistics - Пример 2 (проблемная WTP)
    // Тип элемента = 48, Длина = 15
     uint8_t data[] = {
        // --- TLV Header (4 байта) ---
        0x00, 0x30,       // Type = 48
        0x00, 0x0F,       // Length = 15

        // --- Value (15 байт) ---
        0x01, 0x2C,       // Reboot Count (сбои): 300 (0x012C)
        0x00, 0x04,       // AC Initiated Count: 4
        0x00, 0x0C,       // Link Failure Count: 12
        0x01, 0x2A,       // SW Failure Count: 298 (0x012A)
        0x00, 0x01,       // HW Failure Count: 1
        0x00, 0x00,       // Other Failure Count: 0
        0x00, 0x01,       // Unknown Failure Count: 1
        0x03              // Last Failure Type: 3 (Software Failure)
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = WTPRebootStatistics::Deserialize(&raw_data);

    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::WTPRebootStatistics, element->GetElementType());
    CHECK_EQUAL(300, element->GetRebootCount());
    CHECK_EQUAL(4, element->GetACInitiatedCount());
    CHECK_EQUAL(12, element->GetLinkFailureCount());
    CHECK_EQUAL(298, element->GetSWFailureCount());
    CHECK_EQUAL(1, element->GetHWFailureCount());
    CHECK_EQUAL(0, element->GetOtherFailureCount());
    CHECK_EQUAL(1, element->GetUnknownFailureCount());
    CHECK_EQUAL(WTPRebootStatistics::LastFailureType::SoftwareFailure,
                element->GetLastFailureType());
}

TEST(WTPRebootStatisticsTestsGroup, WTPRebootStatistics_serialize) {
    uint8_t buffer[256] = {};
    WTPRebootStatistics element_0{ 12340, 12341,
                                   12342, 12343,
                                   12344, 12345,
                                   12346, WTPRebootStatistics::LastFailureType::LinkFailure };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 19, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x30, 0x00, 0x0F, 0x30, 0x34, 0x30, 0x35, 0x30, 0x36,
                                  0x30, 0x37, 0x30, 0x38, 0x30, 0x39, 0x30, 0x3A, 0x02 };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto element = WTPRebootStatistics::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(&buffer[0] + 19, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::WTPRebootStatistics, element->GetElementType());
    CHECK_EQUAL(12340, element->GetRebootCount());
}
