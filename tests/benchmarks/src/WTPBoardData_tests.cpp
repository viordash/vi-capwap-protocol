#include "nanobench.h"
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/WTPBoardData.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(WTPBoardDataTestsGroup){ //
                                    TEST_SETUP(){}

                                    TEST_TEARDOWN(){}
};

TEST(WTPBoardDataTestsGroup, WTPBoardData_serialize_deserialize_perf) {
    ankerl::nanobench::Bench b;

    b.title("WTPBoardData").warmup(1000).minEpochIterations(200000).relative(true);
    b.performanceCounters(true);

    uint8_t buffer[256] = {};

    WritableWTPBoardData::SubElement elements[] = {
        { BoardDataSubElementHeader::Type::WTPModelNumber, "abcd" },
        { BoardDataSubElementHeader::Type::WTPSerialNumber, "efghijklm" },
        { BoardDataSubElementHeader::Type::BoardID,
          "01234567890ABCDEF01234567890ABCDEF01234567890ABCDEF01234567890ABCDEF" },
        { BoardDataSubElementHeader::Type::BoardRevision, "1" }
    };

    WritableWTPBoardData write_data{ 1234, elements };

    b.run("serialization", [&] {
        RawData raw_data{ buffer, buffer + sizeof(buffer) };
        write_data.Serialize(&raw_data);
        ankerl::nanobench::doNotOptimizeAway(raw_data);

        raw_data = { buffer, buffer + sizeof(buffer) };
        ReadableWTPBoardData read_data;
        CHECK_TRUE(read_data.Deserialize(&raw_data));
        ankerl::nanobench::doNotOptimizeAway(raw_data);
    });

    const uint8_t reference[] = { 0x00, 0x26, 0x00, 0x66, 0x00, 0x00, 0x04, 0xD2, 0x00, 0x00, 0x00,
                                  0x04, 'a',  'b',  'c',  'd',  0x00, 0x01, 0x00, 0x09, 'e',  'f',
                                  'g',  'h',  'i',  'j',  'k',  'l',  'm',  0x00, 0x02, 0x00, 0x44,
                                  '0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '0',
                                  'A',  'B',  'C',  'D',  'E',  'F',  '0',  '1',  '2',  '3',  '4',
                                  '5',  '6',  '7',  '8',  '9',  '0',  'A',  'B',  'C',  'D',  'E',
                                  'F',  '0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',
                                  '0',  'A',  'B',  'C',  'D',  'E',  'F',  '0',  '1',  '2',  '3',
                                  '4',  '5',  '6',  '7',  '8',  '9',  '0',  'A',  'B',  'C',  'D',
                                  'E',  'F',  0x00, 0x03, 0x00, 0x01, '1' };
    b.run("reference sum", [&] {
        size_t sum = 0;
        for (size_t i = 0; i < sizeof(reference); i++) {
            sum += reference[i];
        }
        ankerl::nanobench::doNotOptimizeAway(sum);
    });
}
