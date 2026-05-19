#include "nanobench.h"
#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ClearHeader.h"
#include "StationConfigurationRequest.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(StationConfigurationRequestTestsGroup){ //
                                                    TEST_SETUP(){} 

                                                    TEST_TEARDOWN(){}
};

TEST(StationConfigurationRequestTestsGroup,
     StationConfigurationRequest_serialize_deserialize_perf_add_station) {
    ankerl::nanobench::Bench b;

    b.title("StationConfigurationRequest AddStation")
        .warmup(1000)
        .minEpochIterations(150000)
        .relative(true);
    b.performanceCounters(true);

    uint8_t buffer[4096] = {};

    WritableAddStationArray add_station;
    uint8_t mac1[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
    add_station.Add(1, MacAddress(nonstd::span<const uint8_t>(mac1, 6)));

    WritableVendorSpecificPayloadArray vendor_specific_payloads;
    vendor_specific_payloads.Add(123456, 789, "test_payload");

    IWritableStationConfigurationRequestOptionalElement *const elems_1[] = {
        &vendor_specific_payloads
    };
    WritableStationConfigurationRequest write_data(&add_station, elems_1);

    b.run("serialization", [&] {
        RawData raw_data{ buffer, buffer + sizeof(buffer) };
        write_data.Serialize(&raw_data);
        ankerl::nanobench::doNotOptimizeAway(raw_data);

        size_t serialized_size = raw_data.current - buffer;
        raw_data = { buffer, buffer + serialized_size };

        ReadableVendorSpecificPayloadArray vendor_specific_payloads;
        IReadableStationConfigurationRequestOptionalElement *const elems_2[] = {
            &vendor_specific_payloads
        };
        ReadableStationConfigurationRequest read_data(elems_2);
        CHECK_TRUE(read_data.Deserialize(&raw_data));
        ankerl::nanobench::doNotOptimizeAway(raw_data);
        CHECK_EQUAL(0, read_data.unknown_elements);
    });

    // AddStation: 12 bytes, VendorSpecific: 22 bytes
    const uint8_t reference[] = { 0x00, 0x08, 0x00, 0x08, 0x01, 0x06, 0x00, 0x1A,
                                  0x2B, 0x3C, 0x4D, 0x5E, 0x00, 0x25, 0x00, 0x12,
                                  0x00, 0x01, 0xE2, 0x40, 0x03, 0x15 };

    b.run("reference sum", [&] {
        size_t sum = 0;
        for (size_t i = 0; i < sizeof(reference); i++) {
            sum += reference[i];
        }
        ankerl::nanobench::doNotOptimizeAway(sum);
    });
}

TEST(StationConfigurationRequestTestsGroup,
     StationConfigurationRequest_serialize_deserialize_perf_delete_station) {
    ankerl::nanobench::Bench b;

    b.title("StationConfigurationRequest DeleteStation")
        .warmup(1000)
        .minEpochIterations(150000)
        .relative(true);
    b.performanceCounters(true);

    uint8_t buffer[4096] = {};

    WritableDeleteStationArray delete_station;
    uint8_t mac1[] = { 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E };
    uint8_t mac2[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    delete_station.Add(7, MacAddress(nonstd::span<const uint8_t>(mac1, 6)));
    delete_station.Add(8, MacAddress(nonstd::span<const uint8_t>(mac2, 6)));

    WritableStationConfigurationRequest write_data(
        &delete_station,
        nonstd::span<IWritableStationConfigurationRequestOptionalElement *const>{});

    b.run("serialization", [&] {
        RawData raw_data{ buffer, buffer + sizeof(buffer) };
        write_data.Serialize(&raw_data);
        ankerl::nanobench::doNotOptimizeAway(raw_data);

        size_t serialized_size = raw_data.current - buffer;
        raw_data = { buffer, buffer + serialized_size };

        ReadableStationConfigurationRequest read_data(
            nonstd::span<IReadableStationConfigurationRequestOptionalElement *const>{});
        CHECK_TRUE(read_data.Deserialize(&raw_data));
        ankerl::nanobench::doNotOptimizeAway(raw_data);
        CHECK_EQUAL(0, read_data.unknown_elements);
    });

    const uint8_t reference[] = {
        // DeleteStation element 1
        0x00, 0x12, 0x00, 0x08, 0x07, 0x06, 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E,
        // DeleteStation element 2
        0x00, 0x12, 0x00, 0x08, 0x08, 0x06, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
    };

    b.run("reference sum", [&] {
        size_t sum = 0;
        for (size_t i = 0; i < sizeof(reference); i++) {
            sum += reference[i];
        }
        ankerl::nanobench::doNotOptimizeAway(sum);
    });
}
