#include "nanobench.h"
#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ChangeStateEventRequest.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(ChangeStateEventRequestTestsGroup){ //
                                               TEST_SETUP(){}

                                               TEST_TEARDOWN(){}
};

TEST(ChangeStateEventRequestTestsGroup, ChangeStateEventRequest_serialize_deserialize_perf) {
    ankerl::nanobench::Bench b;

    b.title("ChangeStateEventRequest").warmup(1000).minEpochIterations(150000).relative(true);
    b.performanceCounters(true);

    uint8_t buffer[4096] = {};

    WritableRadioOperationalStateArray radio_operational_states;
    radio_operational_states.Add({ 0,
                                   RadioOperationalState::States::Enabled,
                                   RadioOperationalState::Causes::AdministrativelySet });
    radio_operational_states.Add(
        { 1, RadioOperationalState::States::Disabled, RadioOperationalState::Causes::Normal });
    radio_operational_states.Add({ 2,
                                   RadioOperationalState::States::Reserved,
                                   RadioOperationalState::Causes::RadioFailure });

    ResultCode result_code = ResultCode::Type::Success;

    const uint8_t UnknownMessageElement[25] = {};
    const uint8_t UnsupportedMessageElement[35] = {};
    WritableReturnedMessageElementArray returned_elements;
    returned_elements.Add(
        ReturnedMessageElement::Reasons::UnknownMessageElement,
        { UnknownMessageElement, UnknownMessageElement + sizeof(UnknownMessageElement) });
    returned_elements.Add(ReturnedMessageElement::Reasons::UnsupportedMessageElement,
                          { UnsupportedMessageElement,
                            UnsupportedMessageElement + sizeof(UnsupportedMessageElement) });

    WritableVendorSpecificPayloadArray vendor_specific_payloads;
    vendor_specific_payloads.Add(123456, 789, "01234567890ABCDEF0123");

    WritableChangeStateEventRequest write_data(radio_operational_states,
                                               result_code,
                                               returned_elements,
                                               vendor_specific_payloads);

    b.run("serialization", [&] {
        RawData raw_data{ buffer, buffer + sizeof(buffer) };
        write_data.Serialize(&raw_data);
        ankerl::nanobench::doNotOptimizeAway(raw_data);

        raw_data = { buffer, buffer + 148 - (sizeof(ClearHeader) + sizeof(ControlHeader)) };
        ReadableChangeStateEventRequest read_data;
        CHECK_TRUE(read_data.Deserialize(&raw_data));
        ankerl::nanobench::doNotOptimizeAway(raw_data);
        CHECK_EQUAL(0, read_data.unknown_elements);
    });

    const uint8_t reference[] = {
        0x00, 0x10, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x2A, 0x00, 0x84,
        0x00, 0x00, 0x20, 0x00, 0x03, 0x00, 0x01, 0x03, 0x00, 0x20, 0x00, 0x03, 0x01, 0x02, 0x00,
        0x00, 0x20, 0x00, 0x03, 0x02, 0x00, 0x01, 0x00, 0x21, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x25, 0x00, 0x1B, 0x00, 0x01, 0xE2, 0x40, 0x03, 0x15, 0x30, 0x31, 0x32, 0x33, 0x34,
        0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x30, 0x31, 0x32,
        0x33, 0x00, 0x22, 0x00, 0x1B, 0x01, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x22, 0x00, 0x25, 0x02, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    b.run("reference sum", [&] {
        size_t sum = 0;
        for (size_t i = 0; i < sizeof(reference); i++) {
            sum += reference[i];
        }
        ankerl::nanobench::doNotOptimizeAway(sum);
    });
}
