#include "nanobench.h"
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ImageDataRequest.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(ImageDataRequestTestsGroup){ //
                                        TEST_SETUP(){}

                                        TEST_TEARDOWN(){}
};

TEST(ImageDataRequestTestsGroup, ImageDataRequest_serialize_deserialize_perf) {
    ankerl::nanobench::Bench b;

    b.title("ImageDataRequest").warmup(1000).minEpochIterations(150000).relative(true);
    b.performanceCounters(true);

    uint8_t buffer[4096] = {};

    CapwapTransportProtocol capwap_transport_protocol{ CapwapTransportProtocol::Type::UDP };

    const uint8_t data[] = { 0xF8, 0x1D, 0x4F, 0xAE, 0x7D, 0xEC, 0x11, 0xD0,
                             0xA7, 0x65, 0x00, 0xA0, 0xC9, 0x1E, 0x6B, 0xF6 };

    WritableImageData image_data{ ImageDataHeader::Type::ImageDataIsIncluded, data };

    WritableVendorSpecificPayloadArray vendor_specific_payloads;
    vendor_specific_payloads.Add(123456, 789, "01234567890ABCDEF0123");

    WritableImageIdentifier image_identifier{ 123456, "1232344" };
    InitiateDownload initiate_download;

    WritableImageDataRequest write_data(&capwap_transport_protocol,
                                        &image_data,
                                        vendor_specific_payloads,
                                        &image_identifier,
                                        &initiate_download);

    b.run("serialization", [&] {
        RawData raw_data{ buffer, buffer + sizeof(buffer) };
        write_data.Serialize(&raw_data);
        ankerl::nanobench::doNotOptimizeAway(raw_data);

        raw_data = { buffer, buffer + 92 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };
        ReadableImageDataRequest read_data;
        CHECK_TRUE(read_data.Deserialize(&raw_data));
        ankerl::nanobench::doNotOptimizeAway(raw_data);
        CHECK_EQUAL(0, read_data.unknown_elements);
    });

    const uint8_t reference[] = { 0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x0F, 0x2A, 0x00, 0x4C, 0x00, 0x00, 0x33, 0x00, 0x01, 0x02, 0x00,
                                  0x18, 0x00, 0x11, 0x01, 0xF8, 0x1D, 0x4F, 0xAE, 0x7D, 0xEC, 0x11,
                                  0xD0, 0xA7, 0x65, 0x00, 0xA0, 0xC9, 0x1E, 0x6B, 0xF6, 0x00, 0x25,
                                  0x00, 0x1B, 0x00, 0x01, 0xE2, 0x40, 0x03, 0x15, 0x30, 0x31, 0x32,
                                  0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43,
                                  0x44, 0x45, 0x46, 0x30, 0x31, 0x32, 0x33, 0x00, 0x19, 0x00, 0x0B,
                                  0x00, 0x01, 0xE2, 0x40, 0x31, 0x32, 0x33, 0x32, 0x33, 0x34, 0x34,
                                  0x00, 0x1B, 0x00, 0x00 };

    b.run("reference sum", [&] {
        size_t sum = 0;
        for (size_t i = 0; i < sizeof(reference); i++) {
            sum += reference[i];
        }
        ankerl::nanobench::doNotOptimizeAway(sum);
    });
}
