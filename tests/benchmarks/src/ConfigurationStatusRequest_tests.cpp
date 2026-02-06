#include "nanobench.h"
#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ConfigurationStatusRequest.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(ConfigurationStatusRequestTestsGroup){ //
                                                  TEST_SETUP(){}

                                                  TEST_TEARDOWN(){}
};

TEST(ConfigurationStatusRequestTestsGroup, ConfigurationStatusRequest_serialize_deserialize_perf) {
    ankerl::nanobench::Bench b;

    b.title("ConfigurationStatusRequest").warmup(1000).minEpochIterations(150000).relative(true);
    b.performanceCounters(true);

    uint8_t buffer[4096] = {};

    WritableRadioAdministrativeStateArray radio_states;
    radio_states.Add({ 0, RadioAdministrativeState::States::Enabled });
    radio_states.Add({ 1, RadioAdministrativeState::States::Disabled });

    WTPRebootStatistics wtp_reboot_statistics{
        21, 22, 23, 24, 25, 26, 27, WTPRebootStatistics::LastFailureType::HardwareFailure
    };

    WritableACNameWithPriorityArray ac_names_with_priority;
    ac_names_with_priority.Add(1, "ACNameWithPriority");

    CapwapTransportProtocol capwap_transport_protocol{ CapwapTransportProtocol::Type::UDP };

    WTPStaticIPAddressInformation wtp_static_ipaddress{ inet_addr("192.168.100.10"),
                                                        inet_addr("255.255.255.0"),
                                                        inet_addr("192.168.1.1"),
                                                        true };

    WritableVendorSpecificPayloadArray vendor_specific_payloads;
    vendor_specific_payloads.Add(123456, 789, "01234567890ABCDEF0123");

    WritableConfigurationStatusRequest write_data("abcdefабвгд",
                                                  radio_states,
                                                  12345,
                                                  wtp_reboot_statistics,
                                                  ac_names_with_priority,
                                                  &capwap_transport_protocol,
                                                  &wtp_static_ipaddress,
                                                  vendor_specific_payloads);

    b.run("serialization", [&] {
        RawData raw_data{ buffer, buffer + sizeof(buffer) };
        write_data.Serialize(&raw_data);
        ankerl::nanobench::doNotOptimizeAway(raw_data);

        raw_data = { buffer, buffer + 149 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };
        ReadableConfigurationStatusRequest read_data;
        CHECK_TRUE(read_data.Deserialize(&raw_data));
        ankerl::nanobench::doNotOptimizeAway(raw_data);
        CHECK_EQUAL(0, read_data.unknown_elements);
    });

    const uint8_t reference[] = {
        0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x2A, 0x00, 0x85,
        0x00, 0x00, 0x04, 0x00, 0x10, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0xD0, 0xB0, 0xD0, 0xB1,
        0xD0, 0xB2, 0xD0, 0xB3, 0xD0, 0xB4, 0x00, 0x1F, 0x00, 0x02, 0x00, 0x01, 0x00, 0x1F, 0x00,
        0x02, 0x01, 0x02, 0x00, 0x24, 0x00, 0x02, 0x30, 0x39, 0x00, 0x30, 0x00, 0x0F, 0x00, 0x15,
        0x00, 0x16, 0x00, 0x17, 0x00, 0x18, 0x00, 0x19, 0x00, 0x1A, 0x00, 0x1B, 0x04, 0x00, 0x05,
        0x00, 0x13, 0x01, 0x41, 0x43, 0x4E, 0x61, 0x6D, 0x65, 0x57, 0x69, 0x74, 0x68, 0x50, 0x72,
        0x69, 0x6F, 0x72, 0x69, 0x74, 0x79, 0x00, 0x33, 0x00, 0x01, 0x02, 0x00, 0x31, 0x00, 0x0D,
        0xC0, 0xA8, 0x64, 0x0A, 0xFF, 0xFF, 0xFF, 0x00, 0xC0, 0xA8, 0x01, 0x01, 0x01, 0x00, 0x25,
        0x00, 0x1B, 0x00, 0x01, 0xE2, 0x40, 0x03, 0x15, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
        0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x30, 0x31, 0x32, 0x33
    };

    b.run("reference sum", [&] {
        size_t sum = 0;
        for (size_t i = 0; i < sizeof(reference); i++) {
            sum += reference[i];
        }
        ankerl::nanobench::doNotOptimizeAway(sum);
    });
}
