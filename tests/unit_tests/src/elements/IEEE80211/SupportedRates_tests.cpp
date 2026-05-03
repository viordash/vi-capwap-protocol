#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/SupportedRates.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(SupportedRatesTestsGroup){ //
                                      TEST_SETUP(){}

                                      TEST_TEARDOWN(){}
};

TEST(SupportedRatesTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableSupportedRatesArray w_rates;

    std::vector<uint8_t> rates_0 = { 0x82, 0x84 };
    std::vector<uint8_t> rates_1 = { 0x0C, 0x12, 0x18, 0x24 };
    std::vector<uint8_t> rates_2 = { 0x30, 0x48, 0x60, 0x6C, 0x8C, 0x98 };

    w_rates.Add({ 1, rates_0 });
    w_rates.Add({ 1, rates_1 });
    w_rates.Add({ 2, rates_2 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_rates.Serialize(&raw_data);
    // Element 0: 4 + 1 + 2 = 7 bytes
    // Element 1: 4 + 1 + 4 = 9 bytes
    // Element 2: 4 + 1 + 6 = 11 bytes
    // Total: 27 bytes
    CHECK_EQUAL(&buffer[0] + 27, raw_data.current);

    ReadableSupportedRatesArray r_rates;
    CHECK_FALSE(r_rates.IsPresent());

    raw_data = { buffer, buffer + 27 };

    CHECK_TRUE(r_rates.Deserialize(&raw_data));
    CHECK_TRUE(r_rates.IsPresent());
    CHECK_TRUE(r_rates.Deserialize(&raw_data));
    CHECK_TRUE(r_rates.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_rates.Get().size());

    CHECK_EQUAL(1, r_rates.Get()[0]->GetRadioID());
    CHECK_EQUAL(2, r_rates.Get()[0]->GetRatesCount());
    CHECK_EQUAL(2 + 1, r_rates.Get()[0]->GetLength());
    MEMCMP_EQUAL(rates_0.data(), r_rates.Get()[0]->data, 2);

    CHECK_EQUAL(1, r_rates.Get()[1]->GetRadioID());
    CHECK_EQUAL(4, r_rates.Get()[1]->GetRatesCount());
    CHECK_EQUAL(4 + 1, r_rates.Get()[1]->GetLength());
    MEMCMP_EQUAL(rates_1.data(), r_rates.Get()[1]->data, 4);

    CHECK_EQUAL(2, r_rates.Get()[2]->GetRadioID());
    CHECK_EQUAL(6, r_rates.Get()[2]->GetRatesCount());
    CHECK_EQUAL(6 + 1, r_rates.Get()[2]->GetLength());
    MEMCMP_EQUAL(rates_2.data(), r_rates.Get()[2]->data, 6);
}
