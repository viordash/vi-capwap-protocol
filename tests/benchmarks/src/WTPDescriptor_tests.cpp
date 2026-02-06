#include "nanobench.h"
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/WTPDescriptor.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(WTPDescriptorTestsGroup){ //
                                     TEST_SETUP(){}

                                     TEST_TEARDOWN(){}
};

TEST(WTPDescriptorTestsGroup, WTPDescriptor_serialize_deserialize_perf) {
    ankerl::nanobench::Bench b;

    b.title("WTPDescriptor").warmup(1000).minEpochIterations(200000).relative(true);
    b.performanceCounters(true);

    uint8_t buffer[256] = {};

    EncryptionSubElement encr_elements[] = { { 0 } };

    WritableWTPDescriptor::SubElement descr_elements[] = {
        { 1234, DescriptorSubElementHeader::Type::ActiveSoftwareVersion, "abcd" },
        { 5678, DescriptorSubElementHeader::Type::BootVersion, "efghijklm" },
        { 9012,
          DescriptorSubElementHeader::Type::OtherSoftwareVersion,
          "01234567890ABCDEF01234567890ABCDEF01234567890ABCDEF01234567890ABCDEF" },
        { 3456, DescriptorSubElementHeader::Type::HardwareVersion, "1" }
    };

    WritableWTPDescriptor write_data{ 3, 2, encr_elements, descr_elements };

    b.run("serialization", [&] {
        RawData raw_data{ buffer, buffer + sizeof(buffer) };
        write_data.Serialize(&raw_data);
        ankerl::nanobench::doNotOptimizeAway(raw_data);

        raw_data = { buffer, buffer + sizeof(buffer) };
        ReadableWTPDescriptor read_data;
        CHECK_TRUE(read_data.Deserialize(&raw_data));
        ankerl::nanobench::doNotOptimizeAway(raw_data);
    });

    const uint8_t reference[] = {
        0x00, 0x27, 0x00, 0x78, 0x03, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0xD2,
        0x00, 0x01, 0x00, 0x04, 0x61, 0x62, 0x63, 0x64, 0x00, 0x00, 0x16, 0x2E, 0x00, 0x02,
        0x00, 0x09, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x00, 0x00, 0x23,
        0x34, 0x00, 0x03, 0x00, 0x44, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
        0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
        0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x30, 0x31, 0x32,
        0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43,
        0x44, 0x45, 0x46, 0x00, 0x00, 0x0D, 0x80, 0x00, 0x00, 0x00, 0x01, 0x31
    };
    b.run("reference sum", [&] {
        size_t sum = 0;
        for (size_t i = 0; i < sizeof(reference); i++) {
            sum += reference[i];
        }
        ankerl::nanobench::doNotOptimizeAway(sum);
    });
}
