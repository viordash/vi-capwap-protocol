#include "nanobench.h"
#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "WTPEventRequest.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(WTPEventRequestTestsGroup){ //
                                       TEST_SETUP(){}

                                       TEST_TEARDOWN(){}
};

TEST(WTPEventRequestTestsGroup, WTPEventRequest_serialize_deserialize_perf) {
    ankerl::nanobench::Bench b;

    b.title("WTPEventRequest").warmup(1000).minEpochIterations(150000).relative(true);
    b.performanceCounters(true);

    uint8_t buffer[4096] = {};

    const uint8_t mac_6_0[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
    const uint8_t mac_6_1[] = { 0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD };
    const uint8_t mac_6_2[] = { 0xAB, 0xBC, 0xCD, 0xDE, 0xEF, 0xFF };
    const uint8_t mac_8_0[] = { 0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD, 0xEE, 0xFF };
    const uint8_t mac_8_1[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0xEE, 0xFF };

    std::vector<MacAddress> addresses0 = { { mac_6_0 }, { mac_8_0 } };
    WritableDecryptionErrorReportArray decryption_error_report;

    WritableDuplicateIPv4AdrArray duplicate_ipv4_address;

    WritableWTPRadioStatisticsArray wtp_radio_statistics;

    WritableDeleteStationArray delete_station;

    WritableVendorSpecificPayloadArray vendor_specific_payloads;

    decryption_error_report.Add(3, addresses0);
    duplicate_ipv4_address.Add(inet_addr("192.168.100.10"), DuplicateStatus::Detected, { mac_6_1 });
    duplicate_ipv4_address.Add(inet_addr("192.168.100.11"), DuplicateStatus::Cleared, { mac_8_1 });

    wtp_radio_statistics.Add({ 1,
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

    std::optional<WTPRebootStatistics> wtp_reboot_statistics{
        std::in_place, 12340, 12341,
        12342,         12343, 12344,
        12345,         12346, WTPRebootStatistics::LastFailureType::LinkFailure
    };

    delete_station.Add(7, { mac_6_2 });
    delete_station.Add(8, { mac_6_0 });

    vendor_specific_payloads.Add(123456, 789, { '0', '1', '2', '3', '4', '5', '6',
                                                '7', '8', '9', '0', 'A', 'B', 'C',
                                                'D', 'E', 'F', '0', '1', '2', '3' });

    b.run("serialization", [&] {
        RawData raw_data{ buffer, buffer + sizeof(buffer) };

        WritableWTPEventRequest write_data(decryption_error_report,
                                           duplicate_ipv4_address,
                                           wtp_radio_statistics,
                                           wtp_reboot_statistics,
                                           delete_station,
                                           vendor_specific_payloads);
        write_data.Serialize(&raw_data);
        ankerl::nanobench::doNotOptimizeAway(raw_data);

        raw_data = { buffer, buffer + 170 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };
        ReadableWTPEventRequest read_data;
        CHECK_TRUE(read_data.Deserialize(&raw_data));
        ankerl::nanobench::doNotOptimizeAway(raw_data);
        CHECK_EQUAL(0, read_data.unknown_elements);
    });

    const uint8_t reference[] = {
        0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x2A, 0x00, 0x9A,
        0x00, 0x00, 0x0F, 0x00, 0x12, 0x03, 0x02, 0x06, 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x08,
        0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD, 0xEE, 0xFF, 0x00, 0x15, 0x00, 0x0C, 0xC0, 0xA8, 0x64,
        0x0A, 0x01, 0x06, 0xAA, 0xBB, 0xCC, 0xFF, 0xFE, 0xDD, 0x00, 0x15, 0x00, 0x0E, 0xC0, 0xA8,
        0x64, 0x0B, 0x00, 0x08, 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0xEE, 0xFF, 0x00, 0x2F, 0x00,
        0x14, 0x01, 0x02, 0x00, 0x28, 0x00, 0x29, 0x00, 0x2A, 0x00, 0x2B, 0x00, 0x2C, 0x00, 0x2D,
        0x00, 0x2E, 0x00, 0x2F, 0xFF, 0xD0, 0x00, 0x30, 0x00, 0x0F, 0x30, 0x34, 0x30, 0x35, 0x30,
        0x36, 0x30, 0x37, 0x30, 0x38, 0x30, 0x39, 0x30, 0x3A, 0x02, 0x00, 0x12, 0x00, 0x08, 0x07,
        0x06, 0xAB, 0xBC, 0xCD, 0xDE, 0xEF, 0xFF, 0x00, 0x12, 0x00, 0x08, 0x08, 0x06, 0x00, 0x1A,
        0x2B, 0x3C, 0x4D, 0x5E, 0x00, 0x25, 0x00, 0x1B, 0x00, 0x01, 0xE2, 0x40, 0x03, 0x15, 0x30,
        0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45,
        0x46, 0x30, 0x31, 0x32, 0x33
    };

    b.run("reference sum", [&] {
        size_t sum = 0;
        for (size_t i = 0; i < sizeof(reference); i++) {
            sum += reference[i];
        }
        ankerl::nanobench::doNotOptimizeAway(sum);
    });
}
