#include "nanobench.h"
#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ClearHeader.h"
#include "WlanConfigurationRequest.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(WlanConfigurationRequestTestsGroup){ //
                                                 TEST_SETUP(){}

                                                 TEST_TEARDOWN(){}
};

TEST(WlanConfigurationRequestTestsGroup,
     WlanConfigurationRequest_serialize_deserialize_perf_add_wlan) {
    ankerl::nanobench::Bench b;

    b.title("WlanConfigurationRequest AddWlan")
        .warmup(1000)
        .minEpochIterations(150000)
        .relative(true);
    b.performanceCounters(true);

    uint8_t buffer[4096] = {};

    WritableAddWlanArray add_wlan;
    uint8_t group_tsc[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
    uint8_t key[] = { 0x11, 0x22, 0x33, 0x44 };
    add_wlan.Add({ 1,
                   2,
                   0x0421,
                   0,
                   AddWlanHeader::KeyStatus::PerStationKeys,
                   nonstd::span<const uint8_t>(key, sizeof(key)),
                   group_tsc,
                   AddWlanHeader::QoS::BestEffort,
                   AddWlanHeader::AuthType::OpenSystem,
                   AddWlanHeader::MACMode::SplitMAC,
                   AddWlanHeader::TunnelMode::Tunnel8023,
                   0,
                   "TestSSID" });

    WritableInformationElementArray info_elements;
    uint8_t ie_data[] = { 0xDD, 0x08, 0x00, 0x50, 0xF2, 0x01, 0x01, 0x00, 0x00, 0x50 };
    info_elements.Add({ 1, 2, InformationElement::FLAG_BEACON, { ie_data, sizeof(ie_data) } });

    IWritableWlanConfigurationRequestOptionalElement *const elems_1[] = { &info_elements };
    WritableWlanConfigurationRequest write_data(&add_wlan, elems_1);

    b.run("serialization", [&] {
        RawData raw_data{ buffer, buffer + sizeof(buffer) };
        write_data.Serialize(&raw_data);
        ankerl::nanobench::doNotOptimizeAway(raw_data);

        raw_data = { buffer, buffer + 68 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };

        ReadableInformationElementArray info_elements;
        IReadableWlanConfigurationRequestOptionalElement *const elems_2[] = { &info_elements };
        ReadableWlanConfigurationRequest read_data(elems_2);
        CHECK_TRUE(read_data.Deserialize(&raw_data));
        ankerl::nanobench::doNotOptimizeAway(raw_data);
        CHECK_EQUAL(0, read_data.unknown_elements);
    });

    const uint8_t reference[] = {
        0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xDD, 0x33, 0x00, 0x2C,
        0x00,
        // AddWlan element (type 0x0004, length 0x1F = 31)
        0x04, 0x00, 0x00, 0x1F, 0x01, 0x02, 0x04, 0x21, 0x00, 0x00, 0x00, 0x04, 0x11, 0x22, 0x33,
        0x44, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x00, 0x01, 0x01, 0x00, 0x54, 0x65, 0x73,
        0x74, 0x53, 0x53, 0x49, 0x44,
        // InformationElement (type 0x0504)
        0x04, 0x05, 0x00, 0x0D, 0x01, 0x02, 0x80, 0xDD, 0x08, 0x00, 0x50, 0xF2, 0x01, 0x01, 0x00,
        0x00, 0x50
    };

    b.run("reference sum", [&] {
        size_t sum = 0;
        for (size_t i = 0; i < sizeof(reference); i++) {
            sum += reference[i];
        }
        ankerl::nanobench::doNotOptimizeAway(sum);
    });
}

TEST(WlanConfigurationRequestTestsGroup,
     WlanConfigurationRequest_serialize_deserialize_perf_delete_wlan) {
    ankerl::nanobench::Bench b;

    b.title("WlanConfigurationRequest DeleteWlan")
        .warmup(1000)
        .minEpochIterations(150000)
        .relative(true);
    b.performanceCounters(true);

    uint8_t buffer[4096] = {};

    WritableDeleteWlanArray delete_wlan;
    delete_wlan.Add({ 1, 2 });
    delete_wlan.Add({ 3, 4 });

    WritableWlanConfigurationRequest write_data(&delete_wlan, nonstd::span<IWritableWlanConfigurationRequestOptionalElement *const>{});

    b.run("serialization", [&] {
        RawData raw_data{ buffer, buffer + sizeof(buffer) };
        write_data.Serialize(&raw_data);
        ankerl::nanobench::doNotOptimizeAway(raw_data);

        raw_data = { buffer, buffer + 28 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };

        ReadableWlanConfigurationRequest read_data(nonstd::span<IReadableWlanConfigurationRequestOptionalElement *const>{});
        CHECK_TRUE(read_data.Deserialize(&raw_data));
        ankerl::nanobench::doNotOptimizeAway(raw_data);
        CHECK_EQUAL(0, read_data.unknown_elements);
    });

    const uint8_t reference[] = { 0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x01, 0xDD, 0x33, 0x00, 0x08, 0x00,
                                  // DeleteWlan element 1 (type 0x0304)
                                  0x04, 0x03, 0x00, 0x02, 0x01, 0x02,
                                  // DeleteWlan element 2 (type 0x0304)
                                  0x04, 0x03, 0x00, 0x02, 0x03, 0x04 };

    b.run("reference sum", [&] {
        size_t sum = 0;
        for (size_t i = 0; i < sizeof(reference); i++) {
            sum += reference[i];
        }
        ankerl::nanobench::doNotOptimizeAway(sum);
    });
}
