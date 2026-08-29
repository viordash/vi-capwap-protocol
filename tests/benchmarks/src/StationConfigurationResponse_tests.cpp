#include "nanobench.h"
#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ClearHeader.h"
#include "StationConfigurationResponse.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(StationConfigurationResponseTestsGroup){ //
                                                     TEST_SETUP(){} 

                                                     TEST_TEARDOWN(){}
};

TEST(StationConfigurationResponseTestsGroup,
     StationConfigurationResponse_serialize_success_benchmark) {
    uint8_t buffer[4096] = {};

    ankerl::nanobench::Bench b;
    b.minEpochIterations(50000);

    WritableStationConfigurationResponse write_data(
        ResultCode::Type::Success,
        nonstd::span<IWritableStationConfigurationResponseOptionalElement *const>{});

    b.run("serialization", [&] {
        RawData raw_data{ buffer, buffer + sizeof(buffer) };
        write_data.Serialize(&raw_data);
        ankerl::nanobench::doNotOptimizeAway(raw_data);

        size_t serialized_size = raw_data.current - buffer;
        raw_data = { buffer, buffer + serialized_size };

        ReadableStationConfigurationResponse read_data(
            nonstd::span<IReadableStationConfigurationResponseOptionalElement *const>{});
        CHECK_TRUE(read_data.Deserialize(&raw_data));
        ankerl::nanobench::doNotOptimizeAway(raw_data);
        CHECK_EQUAL(0, read_data.unknown_elements);
    });

    const uint8_t reference[] = {
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
