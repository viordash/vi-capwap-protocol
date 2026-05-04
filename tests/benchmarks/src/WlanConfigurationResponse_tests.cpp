#include "nanobench.h"
#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ClearHeader.h"
#include "WlanConfigurationResponse.h"
#include "elements/IEEE80211/AssignedWtpBssid.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(WlanConfigurationResponseTestsGroup){ //
                                                  TEST_SETUP(){}

                                                  TEST_TEARDOWN(){}
};

TEST(WlanConfigurationResponseTestsGroup, WlanConfigurationResponse_serialize_success_benchmark) {
    uint8_t buffer[4096] = {};

    ankerl::nanobench::Bench b;
    b.minEpochIterations(50000);

    WritableWlanConfigurationResponse write_data(ResultCode::Type::Success, {});

    b.run("serialization", [&] {
        RawData raw_data{ buffer, buffer + sizeof(buffer) };
        write_data.Serialize(&raw_data);
        ankerl::nanobench::doNotOptimizeAway(raw_data);

        raw_data = { buffer, buffer + 24 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };

        ReadableWlanConfigurationResponse read_data({});
        CHECK_TRUE(read_data.Deserialize(&raw_data));
        ankerl::nanobench::doNotOptimizeAway(raw_data);
        CHECK_EQUAL(0, read_data.unknown_elements);
    });

    const uint8_t reference[] = {
        0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xDD, 0x33, 0x00, 0x04,
        0x00,
        // ResultCode element (type 0x0021, length 0x0004)
        0x00, 0x21, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00
    };

    b.run("reference sum", [&] {
        size_t sum = 0;
        for (size_t i = 0; i < sizeof(reference); i++) {
            sum += reference[i];
        }
        ankerl::nanobench::doNotOptimizeAway(sum);
    });
}

TEST(WlanConfigurationResponseTestsGroup,
     WlanConfigurationResponse_serialize_with_assigned_bssid_benchmark) {
    uint8_t buffer[4096] = {};

    ankerl::nanobench::Bench b;
    b.minEpochIterations(50000);

    uint8_t bssid1[] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
    uint8_t bssid2[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

    WritableAssignedWtpBssidArray assigned_bssid;
    assigned_bssid.Add({ 1, 2, bssid1 });
    assigned_bssid.Add({ 3, 4, bssid2 });

    WritableWlanConfigurationResponse write_data(ResultCode::Type::Success, { &assigned_bssid });

    b.run("serialization", [&] {
        RawData raw_data{ buffer, buffer + sizeof(buffer) };
        write_data.Serialize(&raw_data);
        ankerl::nanobench::doNotOptimizeAway(raw_data);

        raw_data = { buffer, buffer + 48 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };

        ReadableAssignedWtpBssidArray assigned_bssid;
        ReadableWlanConfigurationResponse read_data({ &assigned_bssid });
        CHECK_TRUE(read_data.Deserialize(&raw_data));
        ankerl::nanobench::doNotOptimizeAway(raw_data);
        CHECK_EQUAL(0, read_data.unknown_elements);
    });

    const uint8_t reference[] = {
        0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xDD, 0x33, 0x00, 0x1C,
        0x00,
        // ResultCode element (type 0x0021, length 0x0004)
        0x00, 0x21, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
        // AssignedWtpBssid element 1 (type 0x0204, length 0x0008)
        0x04, 0x02, 0x00, 0x08, 0x01, 0x02, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
        // AssignedWtpBssid element 2 (type 0x0204, length 0x0008)
        0x04, 0x02, 0x00, 0x08, 0x03, 0x04, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
    };

    b.run("reference sum", [&] {
        size_t sum = 0;
        for (size_t i = 0; i < sizeof(reference); i++) {
            sum += reference[i];
        }
        ankerl::nanobench::doNotOptimizeAway(sum);
    });
}
