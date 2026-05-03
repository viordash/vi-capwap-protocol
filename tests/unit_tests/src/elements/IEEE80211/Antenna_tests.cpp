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

TEST(AntennaTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableAntennaArray w_antennas;

    std::vector<Antenna::AntennaSelection> selection_0 = { Antenna::AntennaSelection::Internal };
    w_antennas.Add(1, Antenna::Diversity::Enabled, Antenna::Combiner::SectorizedLeft, selection_0);

    std::vector<Antenna::AntennaSelection> selection_1 = { Antenna::AntennaSelection::External,
                                                           Antenna::AntennaSelection::External };
    w_antennas.Add(2,
                   Antenna::Diversity::Disabled,
                   Antenna::Combiner::SectorizedRight,
                   selection_1);

    std::vector<Antenna::AntennaSelection> selection_2 = { Antenna::AntennaSelection::Internal,
                                                           Antenna::AntennaSelection::External,
                                                           Antenna::AntennaSelection::Internal };
    w_antennas.Add(3, Antenna::Diversity::Enabled, Antenna::Combiner::Omni, selection_2);

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_antennas.Serialize(&raw_data);
    // Element 1: 8 + 1 = 9, Element 2: 8 + 2 = 10, Element 3: 8 + 3 = 11 => total 30
    CHECK_EQUAL(&buffer[0] + 30, raw_data.current);

    ReadableAntennaArray r_antennas;
    CHECK_FALSE(r_antennas.IsPresent());

    raw_data = { buffer, raw_data.current };

    CHECK_TRUE(r_antennas.Deserialize(&raw_data));
    CHECK_TRUE(r_antennas.IsPresent());
    CHECK_TRUE(r_antennas.Deserialize(&raw_data));
    CHECK_TRUE(r_antennas.Deserialize(&raw_data));
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(3, r_antennas.Get().size());

    CHECK_EQUAL(1, r_antennas.Get()[0]->GetRadioID());
    CHECK_EQUAL(Antenna::Diversity::Enabled, r_antennas.Get()[0]->GetDiversity());
    CHECK_EQUAL(Antenna::Combiner::SectorizedLeft, r_antennas.Get()[0]->GetCombiner());
    CHECK_EQUAL(1, r_antennas.Get()[0]->GetAntennaCount());
    CHECK_EQUAL(Antenna::AntennaSelection::Internal, r_antennas.Get()[0]->antenna_selection[0]);

    CHECK_EQUAL(2, r_antennas.Get()[1]->GetRadioID());
    CHECK_EQUAL(Antenna::Diversity::Disabled, r_antennas.Get()[1]->GetDiversity());
    CHECK_EQUAL(Antenna::Combiner::SectorizedRight, r_antennas.Get()[1]->GetCombiner());
    CHECK_EQUAL(2, r_antennas.Get()[1]->GetAntennaCount());
    CHECK_EQUAL(Antenna::AntennaSelection::External, r_antennas.Get()[1]->antenna_selection[0]);
    CHECK_EQUAL(Antenna::AntennaSelection::External, r_antennas.Get()[1]->antenna_selection[1]);

    CHECK_EQUAL(3, r_antennas.Get()[2]->GetRadioID());
    CHECK_EQUAL(Antenna::Diversity::Enabled, r_antennas.Get()[2]->GetDiversity());
    CHECK_EQUAL(Antenna::Combiner::Omni, r_antennas.Get()[2]->GetCombiner());
    CHECK_EQUAL(3, r_antennas.Get()[2]->GetAntennaCount());
    CHECK_EQUAL(Antenna::AntennaSelection::Internal, r_antennas.Get()[2]->antenna_selection[0]);
    CHECK_EQUAL(Antenna::AntennaSelection::External, r_antennas.Get()[2]->antenna_selection[1]);
    CHECK_EQUAL(Antenna::AntennaSelection::Internal, r_antennas.Get()[2]->antenna_selection[2]);
}

TEST(AntennaTestsGroup, Add_array_of_items_is_unique_by_radio_id) {
    uint8_t buffer[2048] = {};

    WritableAntennaArray w_antennas;

    // Add same RadioID multiple times - should replace

    std::vector<Antenna::AntennaSelection> selection_0 = { Antenna::AntennaSelection::Internal };
    w_antennas.Add(1, Antenna::Diversity::Enabled, Antenna::Combiner::Omni, selection_0);

    std::vector<Antenna::AntennaSelection> selection_1 = { Antenna::AntennaSelection::External };
    w_antennas.Add(1, Antenna::Diversity::Disabled, Antenna::Combiner::MIMO, selection_1);

    std::vector<Antenna::AntennaSelection> selection_2 = { Antenna::AntennaSelection::Internal };
    w_antennas.Add(2, Antenna::Diversity::Enabled, Antenna::Combiner::SectorizedLeft, selection_2);

    std::vector<Antenna::AntennaSelection> selection_3 = { Antenna::AntennaSelection::External };
    w_antennas.Add(2,
                   Antenna::Diversity::Disabled,
                   Antenna::Combiner::SectorizedRight,
                   selection_3);

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    w_antennas.Serialize(&raw_data);

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };

    ReadableAntennaArray r_antennas;
    CHECK_FALSE(r_antennas.IsPresent());

    CHECK_TRUE(r_antennas.Deserialize(&raw_data));
    CHECK_TRUE(r_antennas.IsPresent());
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
