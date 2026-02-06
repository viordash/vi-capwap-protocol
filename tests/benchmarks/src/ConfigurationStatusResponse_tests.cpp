#include "nanobench.h"
#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ConfigurationStatusResponse.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(ConfigurationStatusResponseTestsGroup){ //
                                                   TEST_SETUP(){}

                                                   TEST_TEARDOWN(){}
};

TEST(ConfigurationStatusResponseTestsGroup,
     ConfigurationStatusResponse_serialize_deserialize_perf) {
    ankerl::nanobench::Bench b;

    b.title("ConfigurationStatusResponse").warmup(1000).minEpochIterations(150000).relative(true);
    b.performanceCounters(true);

    uint8_t buffer[4096] = {};

    CAPWAPTimers capwap_timers{ 42, 19 };

    WritableDecryptionErrorReportPeriodArray decryption_error_report_periods;
    decryption_error_report_periods.Add({ 0, 10 });
    decryption_error_report_periods.Add({ 1, 100 });
    decryption_error_report_periods.Add({ 31, 65535 });

    uint32_t idle_timeout = 1234;

    WTPFallback::Mode wtp_fallback = WTPFallback::Mode::Enabled;

    uint32_t ac_ipv4_list[] = {
        { inet_addr("192.168.1.110") },
        { inet_addr("192.168.1.111") },
        { inet_addr("192.168.1.112") },
    };

    WTPStaticIPAddressInformation wtp_static_ipaddress{ inet_addr("192.168.100.10"),
                                                        inet_addr("255.255.255.0"),
                                                        inet_addr("192.168.1.1"),
                                                        true };

    WritableVendorSpecificPayloadArray vendor_specific_payloads;
    vendor_specific_payloads.Add(123456, 789, "01234567890ABCDEF0123");

    WritableConfigurationStatusResponse write_data(capwap_timers,
                                                   decryption_error_report_periods,
                                                   idle_timeout,
                                                   wtp_fallback,
                                                   ac_ipv4_list,
                                                   &wtp_static_ipaddress,
                                                   vendor_specific_payloads);

    b.run("serialization", [&] {
        RawData raw_data{ buffer, buffer + sizeof(buffer) };
        write_data.Serialize(&raw_data);
        ankerl::nanobench::doNotOptimizeAway(raw_data);

        raw_data = { buffer, buffer + 120 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };
        ReadableConfigurationStatusResponse read_data;
        CHECK_TRUE(read_data.Deserialize(&raw_data));
        ankerl::nanobench::doNotOptimizeAway(raw_data);
        CHECK_EQUAL(0, read_data.unknown_elements);
    });

    const uint8_t reference[] = {
        0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x2A, 0x00, 0x68,
        0x00, 0x00, 0x0C, 0x00, 0x02, 0x2A, 0x13, 0x00, 0x10, 0x00, 0x03, 0x00, 0x00, 0x0A, 0x00,
        0x10, 0x00, 0x03, 0x01, 0x00, 0x64, 0x00, 0x10, 0x00, 0x03, 0x1F, 0xFF, 0xFF, 0x00, 0x17,
        0x00, 0x04, 0x00, 0x00, 0x04, 0xD2, 0x00, 0x28, 0x00, 0x01, 0x01, 0x00, 0x02, 0x00, 0x0C,
        0xC0, 0xA8, 0x01, 0x6E, 0xC0, 0xA8, 0x01, 0x6F, 0xC0, 0xA8, 0x01, 0x70, 0x00, 0x31, 0x00,
        0x0D, 0xC0, 0xA8, 0x64, 0x0A, 0xFF, 0xFF, 0xFF, 0x00, 0xC0, 0xA8, 0x01, 0x01, 0x01, 0x00,
        0x25, 0x00, 0x1B, 0x00, 0x01, 0xE2, 0x40, 0x03, 0x15, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
        0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x30, 0x31, 0x32, 0x33
    };

    b.run("reference sum", [&] {
        size_t sum = 0;
        for (size_t i = 0; i < sizeof(reference); i++) {
            sum += reference[i];
        }
        ankerl::nanobench::doNotOptimizeAway(sum);
    });
}
