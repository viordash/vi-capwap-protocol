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

TEST(WTPBoardDataTestsGroup, WTPBoardDataHeader_deserialize) {
    uint8_t data[] = {
        // 5. WTP Board Data (Тип: 38, Длина: 26)
        0x00,
        0x26,
        0x00,
        0x04,
        // Value:
        0x00,
        0x00,
        0x44,
        0x5C, // Vendor ID: 17500 (Fictional-Networks)
    };
    RawData raw_data{ data, data + sizeof(data) };
    auto element = WTPBoardDataHeader::Deserialize(&raw_data);

    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::WTPBoardData, element->GetElementType());
    CHECK_EQUAL(17500, element->GetVendorIdentifier());
}

TEST(WTPBoardDataTestsGroup, WTPBoardDataHeader_serialize) {
    uint8_t buffer[256] = {};
    WTPBoardDataHeader element_0{ 305419896, 22 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 8, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x26, 0x00, 0x1a, 0x12, 0x34, 0x56, 0x78 };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto element = WTPBoardDataHeader::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(&buffer[0] + 8, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::WTPBoardData, element->GetElementType());
    CHECK_EQUAL(305419896, element->GetVendorIdentifier());
}

TEST(WTPBoardDataTestsGroup, BoardDataSubElementHeader_deserialize) {
    uint8_t data[] = {
        // Board Data Sub-Element #1 (Model Number)
        0x00, 0x00,                          // Type: 0 (Model Number)
        0x00, 0x07,                          // Length: 7
        'A',  'P',  '-', '2', '0', '0', '0', // Value: "AP-2000"
    };
    RawData raw_data{ data, data + sizeof(data) };
    auto element = BoardDataSubElementHeader::Deserialize(&raw_data);

    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current + 7, raw_data.end);
    STRNCMP_EQUAL("AP-2000", (char *)raw_data.current, 7);
    CHECK_EQUAL(BoardDataSubElementHeader::Type::WTPModelNumber, element->GetType());
    CHECK_EQUAL(7, element->GetLength());
}

TEST(WTPBoardDataTestsGroup, BoardDataSubElementHeader_serialize) {
    uint8_t buffer[256] = {};
    BoardDataSubElementHeader element_0{ BoardDataSubElementHeader::Type::BoardID, 10 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 4, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x02, 0x00, 0x0a };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto element = BoardDataSubElementHeader::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(&buffer[0] + 4, raw_data.current);
    CHECK_EQUAL(BoardDataSubElementHeader::Type::BoardID, element->GetType());
    CHECK_EQUAL(10, element->GetLength());
}

TEST(WTPBoardDataTestsGroup, WTPBoardData_serialize) {
    uint8_t buffer[256] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    WritableWTPBoardData::SubElement elements[] = {
        { BoardDataSubElementHeader::Type::WTPModelNumber, "abcd" },
        { BoardDataSubElementHeader::Type::WTPSerialNumber, "efghijklm" },
        { BoardDataSubElementHeader::Type::BoardID,
          "01234567890ABCDEF01234567890ABCDEF01234567890ABCDEF01234567890ABCDEF" },
        { BoardDataSubElementHeader::Type::BoardRevision, "1" }
    };

    WritableWTPBoardData write_data{ 1234, elements };

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 106, raw_data.current);
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
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    ReadableWTPBoardData read_data;
    CHECK_TRUE(read_data.Deserialize(&raw_data));

    CHECK_EQUAL(&buffer[0] + sizeof(reference), raw_data.current);
    CHECK_EQUAL(4, read_data.Get().size());
    CHECK_EQUAL(1234, read_data.header->GetVendorIdentifier());

    STRNCMP_EQUAL("abcd", (char *)read_data.Get()[0]->value, 4);
    CHECK_EQUAL(BoardDataSubElementHeader::Type::WTPModelNumber, read_data.Get()[0]->GetType());
    CHECK_EQUAL(4, read_data.Get()[0]->GetLength());

    STRNCMP_EQUAL("efghijklm", (char *)read_data.Get()[1]->value, 9);
    CHECK_EQUAL(BoardDataSubElementHeader::Type::WTPSerialNumber, read_data.Get()[1]->GetType());
    CHECK_EQUAL(9, read_data.Get()[1]->GetLength());

    STRNCMP_EQUAL("01234567890ABCDEF01234567890ABCDEF01234567890ABCDEF01234567890ABCDEF",
                  (char *)read_data.Get()[2]->value,
                  68);
    CHECK_EQUAL(BoardDataSubElementHeader::Type::BoardID, read_data.Get()[2]->GetType());
    CHECK_EQUAL(68, read_data.Get()[2]->GetLength());

    STRNCMP_EQUAL("1", (char *)read_data.Get()[3]->value, 1);
    CHECK_EQUAL(BoardDataSubElementHeader::Type::BoardRevision, read_data.Get()[3]->GetType());
    CHECK_EQUAL(1, read_data.Get()[3]->GetLength());
}

TEST(WTPBoardDataTestsGroup, WTPBoardData_deserialize) {
    uint8_t data[] = {
        // 5. WTP Board Data (Тип: 38, Длина: 26)
        0x00,
        0x26,
        0x00,
        0x1a,
        // Value:
        0x00,
        0x00,
        0x44,
        0x5C, // Vendor ID: 17500 (Fictional-Networks)

        // Board Data Sub-Element #1 (Model Number)
        0x00,
        0x00, // Type: 0 (Model Number)
        0x00,
        0x07, // Length: 7
        'A',
        'P',
        '-',
        '2',
        '0',
        '0',
        '0', // Value: "AP-2000"

        // Board Data Sub-Element #2 (Serial Number)
        0x00,
        0x01, // Type: 1 (Serial Number)
        0x00,
        0x07, // Length: 7
        'S',
        'N',
        'A',
        'B',
        'C',
        'D',
        'E', // Value: "SNABCDE"
    };
    RawData raw_data{ data, data + sizeof(data) };

    ReadableWTPBoardData wtp_board_data;

    CHECK_TRUE(wtp_board_data.Deserialize(&raw_data));

    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(2, wtp_board_data.Get().size());
    CHECK_EQUAL(17500, wtp_board_data.header->GetVendorIdentifier());

    STRNCMP_EQUAL("AP-2000", (char *)wtp_board_data.Get()[0]->value, 7);
    CHECK_EQUAL(BoardDataSubElementHeader::Type::WTPModelNumber,
                wtp_board_data.Get()[0]->GetType());
    CHECK_EQUAL(7, wtp_board_data.Get()[0]->GetLength());

    STRNCMP_EQUAL("SNABCDE", (char *)wtp_board_data.Get()[1]->value, 7);
    CHECK_EQUAL(BoardDataSubElementHeader::Type::WTPSerialNumber,
                wtp_board_data.Get()[1]->GetType());
    CHECK_EQUAL(7, wtp_board_data.Get()[1]->GetLength());
}
