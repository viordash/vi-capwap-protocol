#include "nanobench.h"
#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ChangeStateEventResponse.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(ChangeStateEventResponseTestsGroup){ //
                                                TEST_SETUP(){}

                                                TEST_TEARDOWN(){}
};

TEST(ChangeStateEventResponseTestsGroup, ChangeStateEventResponse_serialize_deserialize_perf) {
    ankerl::nanobench::Bench b;

    b.title("ChangeStateEventResponse").warmup(1000).minEpochIterations(300000).relative(true);
    b.performanceCounters(true);

    uint8_t buffer[4096] = {};

    WritableVendorSpecificPayloadArray vendor_specific_payloads;
    vendor_specific_payloads.Add(123456, 789, "01234567890ABCDEF0123");

    WritableChangeStateEventResponse write_data(vendor_specific_payloads);

    b.run("serialization", [&] {
        RawData raw_data{ buffer, buffer + sizeof(buffer) };
        write_data.Serialize(&raw_data);
        ankerl::nanobench::doNotOptimizeAway(raw_data);

        raw_data = { buffer, buffer + 47 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };
        ReadableChangeStateEventResponse read_data;
        CHECK_TRUE(read_data.Deserialize(&raw_data));
        ankerl::nanobench::doNotOptimizeAway(raw_data);
        CHECK_EQUAL(0, read_data.unknown_elements);
    });

    const uint8_t reference[] = { 0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x0C, 0x2A, 0x00, 0x1F, 0x00, 0x00, 0x25, 0x00, 0x1B,
                                  0x00, 0x01, 0xE2, 0x40, 0x03, 0x15, 0x30, 0x31, 0x32, 0x33,
                                  0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43,
                                  0x44, 0x45, 0x46, 0x30, 0x31, 0x32, 0x33 };

    b.run("reference sum", [&] {
        size_t sum = 0;
        for (size_t i = 0; i < sizeof(reference); i++) {
            sum += reference[i];
        }
        ankerl::nanobench::doNotOptimizeAway(sum);
    });
}
