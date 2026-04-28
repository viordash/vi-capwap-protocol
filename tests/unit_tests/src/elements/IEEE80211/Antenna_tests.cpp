#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/IEEE80211/Antenna.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(AntennaTestsGroup){ //
                               TEST_SETUP(){}

                               TEST_TEARDOWN(){}
};

TEST(AntennaTestsGroup, Deserialize) {
    // clang-format off
    uint8_t data[] = {
        // ---- Message Element Header (4 bytes) ----
        0x04, 0x01,     // Type: 1025 (IEEE 802.11 Antenna)
        0x00, 0x06,     // Length: 6 bytes (4 fixed + 2 antenna selections)

        // ---- Value (6 bytes) ----
        0x01,           // Radio ID: 1
        0x01,           // Diversity: Enabled
        0x03,           // Combiner: Omni
        0x02,           // Antenna Count: 2
        0x01,           // Antenna Selection[0]: Internal
        0x02            // Antenna Selection[1]: External
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = Antenna::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::Antenna, element->GetElementType());

    CHECK_EQUAL(1, element->GetRadioID());
    CHECK_EQUAL(Antenna::Diversity::Enabled, element->GetDiversity());
    CHECK_EQUAL(Antenna::Combiner::Omni, element->GetCombiner());
    CHECK_EQUAL(2, element->GetAntennaCount());
    CHECK_EQUAL(Antenna::AntennaSelection::Internal, element->antenna_selection[0]);
    CHECK_EQUAL(Antenna::AntennaSelection::External, element->antenna_selection[1]);
}

TEST(AntennaTestsGroup, Serialize) {
    uint8_t buffer[256] = {};
    std::vector<Antenna::AntennaSelection> selections = { Antenna::AntennaSelection::External,
                                                          Antenna::AntennaSelection::Internal,
                                                          Antenna::AntennaSelection::External };

    WritableAntennaArray w_antennas;
    w_antennas.Add(5, Antenna::Diversity::Disabled, Antenna::Combiner::MIMO, selections);

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_antennas.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 11, raw_data.current);

    // clang-format off
    const uint8_t reference[] = {
        0x04, 0x01,     // Type: 1025
        0x00, 0x07,     // Length: 7 (4 + 3 antenna selections)
        0x05,           // Radio ID: 5
        0x00,           // Diversity: Disabled
        0x04,           // Combiner: MIMO
        0x03,           // Antenna Count: 3
        0x02, 0x01, 0x02 // Antenna Selections: External, Internal, External
    };
    // clang-format on
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto element = Antenna::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(&buffer[0] + 11, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::Antenna, element->GetElementType());
    CHECK_EQUAL(5, element->GetRadioID());
    CHECK_EQUAL(Antenna::Diversity::Disabled, element->GetDiversity());
    CHECK_EQUAL(Antenna::Combiner::MIMO, element->GetCombiner());
    CHECK_EQUAL(3, element->GetAntennaCount());
}

TEST(AntennaTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableAntennaArray w_antennas;

    w_antennas.Add(1,
                   Antenna::Diversity::Enabled,
                   Antenna::Combiner::SectorizedLeft,
                   { Antenna::AntennaSelection::Internal });
    w_antennas.Add(2,
                   Antenna::Diversity::Disabled,
                   Antenna::Combiner::SectorizedRight,
                   { Antenna::AntennaSelection::External, Antenna::AntennaSelection::External });
    w_antennas.Add(3,
                   Antenna::Diversity::Enabled,
                   Antenna::Combiner::Omni,
                   { Antenna::AntennaSelection::Internal,
                     Antenna::AntennaSelection::External,
                     Antenna::AntennaSelection::Internal });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_antennas.Serialize(&raw_data);
    // Element 1: 8 + 1 = 9, Element 2: 8 + 2 = 10, Element 3: 8 + 3 = 11 => total 30
    CHECK_EQUAL(&buffer[0] + 30, raw_data.current);

    ReadableAntennaArray r_antennas;

    raw_data = { buffer, raw_data.current };

    CHECK_TRUE(r_antennas.Deserialize(&raw_data));
    CHECK_TRUE(r_antennas.Deserialize(&raw_data));
    CHECK_TRUE(r_antennas.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_antennas.Get().size());

    CHECK_EQUAL(1, r_antennas.Get()[0]->GetRadioID());
    CHECK_EQUAL(Antenna::Diversity::Enabled, r_antennas.Get()[0]->GetDiversity());
    CHECK_EQUAL(Antenna::Combiner::SectorizedLeft, r_antennas.Get()[0]->GetCombiner());
    CHECK_EQUAL(1, r_antennas.Get()[0]->GetAntennaCount());

    CHECK_EQUAL(2, r_antennas.Get()[1]->GetRadioID());
    CHECK_EQUAL(Antenna::Diversity::Disabled, r_antennas.Get()[1]->GetDiversity());
    CHECK_EQUAL(Antenna::Combiner::SectorizedRight, r_antennas.Get()[1]->GetCombiner());
    CHECK_EQUAL(2, r_antennas.Get()[1]->GetAntennaCount());

    CHECK_EQUAL(3, r_antennas.Get()[2]->GetRadioID());
    CHECK_EQUAL(Antenna::Diversity::Enabled, r_antennas.Get()[2]->GetDiversity());
    CHECK_EQUAL(Antenna::Combiner::Omni, r_antennas.Get()[2]->GetCombiner());
    CHECK_EQUAL(3, r_antennas.Get()[2]->GetAntennaCount());
}

TEST(AntennaTestsGroup, Add_array_of_items_is_unique_by_radio_id) {
    uint8_t buffer[2048] = {};

    WritableAntennaArray w_antennas;

    // Add same RadioID multiple times - should replace
    w_antennas.Add(1,
                   Antenna::Diversity::Enabled,
                   Antenna::Combiner::Omni,
                   { Antenna::AntennaSelection::Internal });
    w_antennas.Add(1,
                   Antenna::Diversity::Disabled,
                   Antenna::Combiner::MIMO,
                   { Antenna::AntennaSelection::External });
    w_antennas.Add(2,
                   Antenna::Diversity::Enabled,
                   Antenna::Combiner::SectorizedLeft,
                   { Antenna::AntennaSelection::Internal });
    w_antennas.Add(2,
                   Antenna::Diversity::Disabled,
                   Antenna::Combiner::SectorizedRight,
                   { Antenna::AntennaSelection::External });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_antennas.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableAntennaArray r_antennas;

    CHECK_TRUE(r_antennas.Deserialize(&raw_data));
    CHECK_TRUE(r_antennas.Deserialize(&raw_data));
    CHECK_FALSE(r_antennas.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, r_antennas.Get().size());

    // Should have the last values for each RadioID
    CHECK_EQUAL(1, r_antennas.Get()[0]->GetRadioID());
    CHECK_EQUAL(Antenna::Diversity::Disabled, r_antennas.Get()[0]->GetDiversity());
    CHECK_EQUAL(Antenna::Combiner::MIMO, r_antennas.Get()[0]->GetCombiner());

    CHECK_EQUAL(2, r_antennas.Get()[1]->GetRadioID());
    CHECK_EQUAL(Antenna::Diversity::Disabled, r_antennas.Get()[1]->GetDiversity());
    CHECK_EQUAL(Antenna::Combiner::SectorizedRight, r_antennas.Get()[1]->GetCombiner());
}

TEST(AntennaTestsGroup, Validate_Diversity) {
    // clang-format off
    // Valid Diversity = 0 (Disabled)
    uint8_t data_valid_0[] = {
        0x04, 0x01, 0x00, 0x05, 0x01, 0x00, 0x03, 0x01, 0x01
    };
    // Valid Diversity = 1 (Enabled)
    uint8_t data_valid_1[] = {
        0x04, 0x01, 0x00, 0x05, 0x01, 0x01, 0x03, 0x01, 0x01
    };
    // Invalid Diversity = 2
    uint8_t data_invalid[] = {
        0x04, 0x01, 0x00, 0x05, 0x01, 0x02, 0x03, 0x01, 0x01
    };
    // clang-format on

    RawData raw_data = { data_valid_0, data_valid_0 + sizeof(data_valid_0) };
    CHECK(Antenna::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_1, data_valid_1 + sizeof(data_valid_1) };
    CHECK(Antenna::Deserialize(&raw_data) != nullptr);

    raw_data = { data_invalid, data_invalid + sizeof(data_invalid) };
    CHECK(Antenna::Deserialize(&raw_data) == nullptr);
}

TEST(AntennaTestsGroup, Validate_Combiner) {
    // clang-format off
    // Invalid Combiner = 0
    uint8_t data_invalid_0[] = {
        0x04, 0x01, 0x00, 0x05, 0x01, 0x00, 0x00, 0x01, 0x01
    };
    // Valid Combiner = 1 (SectorizedLeft)
    uint8_t data_valid_1[] = {
        0x04, 0x01, 0x00, 0x05, 0x01, 0x00, 0x01, 0x01, 0x01
    };
    // Valid Combiner = 4 (MIMO)
    uint8_t data_valid_4[] = {
        0x04, 0x01, 0x00, 0x05, 0x01, 0x00, 0x04, 0x01, 0x01
    };
    // Invalid Combiner = 5
    uint8_t data_invalid_5[] = {
        0x04, 0x01, 0x00, 0x05, 0x01, 0x00, 0x05, 0x01, 0x01
    };
    // clang-format on

    RawData raw_data = { data_invalid_0, data_invalid_0 + sizeof(data_invalid_0) };
    CHECK(Antenna::Deserialize(&raw_data) == nullptr);

    raw_data = { data_valid_1, data_valid_1 + sizeof(data_valid_1) };
    CHECK(Antenna::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_4, data_valid_4 + sizeof(data_valid_4) };
    CHECK(Antenna::Deserialize(&raw_data) != nullptr);

    raw_data = { data_invalid_5, data_invalid_5 + sizeof(data_invalid_5) };
    CHECK(Antenna::Deserialize(&raw_data) == nullptr);
}

TEST(AntennaTestsGroup, Validate_AntennaSelection) {
    // clang-format off
    // Invalid AntennaSelection = 0
    uint8_t data_invalid_0[] = {
        0x04, 0x01, 0x00, 0x05, 0x01, 0x00, 0x03, 0x01, 0x00
    };
    // Valid AntennaSelection = 1 (Internal)
    uint8_t data_valid_1[] = {
        0x04, 0x01, 0x00, 0x05, 0x01, 0x00, 0x03, 0x01, 0x01
    };
    // Valid AntennaSelection = 2 (External)
    uint8_t data_valid_2[] = {
        0x04, 0x01, 0x00, 0x05, 0x01, 0x00, 0x03, 0x01, 0x02
    };
    // Invalid AntennaSelection = 3
    uint8_t data_invalid_3[] = {
        0x04, 0x01, 0x00, 0x05, 0x01, 0x00, 0x03, 0x01, 0x03
    };
    // clang-format on

    RawData raw_data = { data_invalid_0, data_invalid_0 + sizeof(data_invalid_0) };
    CHECK(Antenna::Deserialize(&raw_data) == nullptr);

    raw_data = { data_valid_1, data_valid_1 + sizeof(data_valid_1) };
    CHECK(Antenna::Deserialize(&raw_data) != nullptr);

    raw_data = { data_valid_2, data_valid_2 + sizeof(data_valid_2) };
    CHECK(Antenna::Deserialize(&raw_data) != nullptr);

    raw_data = { data_invalid_3, data_invalid_3 + sizeof(data_invalid_3) };
    CHECK(Antenna::Deserialize(&raw_data) == nullptr);
}

TEST(AntennaTestsGroup, Validate_AntennaCount_mismatch) {
    // clang-format off
    // AntennaCount = 2, but only 1 selection in data
    uint8_t data_mismatch[] = {
        0x04, 0x01, 0x00, 0x05, 0x01, 0x00, 0x03, 0x02, 0x01
    };
    // clang-format on

    RawData raw_data = { data_mismatch, data_mismatch + sizeof(data_mismatch) };
    CHECK(Antenna::Deserialize(&raw_data) == nullptr);
}
