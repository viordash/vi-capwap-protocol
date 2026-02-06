#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ClearHeader.h"
#include "ControlHeader.h"
#include "elements/DecryptionErrorReportPeriod.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(DecryptionErrorReportPeriodTestsGroup){ //
                                                   TEST_SETUP(){}

                                                   TEST_TEARDOWN(){}
};

TEST(DecryptionErrorReportPeriodTestsGroup, Deserialize) {
    // Дамп для элемента 4.6.18 Decryption Error Report Period
    // Радио ID 1, интервал 120 секунд.
    uint8_t data[] = {
        // --- Заголовок элемента ---
        0x00,
        0x10, // Type: 16 (0x0010)
        0x00,
        0x03, // Length: 3 (длина поля Value)

        // --- Поле Value (3 байта) ---
        0x01, // Radio ID: 1
        0x01,
        0x2C // Report Interval: 300 (в сетевом порядке байтов)
    };

    RawData raw_data{ data, data + sizeof(data) };
    auto element = DecryptionErrorReportPeriod::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::DecryptionErrorReportPeriod, element->GetElementType());

    CHECK_EQUAL(1, element->RadioID());
    CHECK_EQUAL(300, element->ReportInterval());
}

TEST(DecryptionErrorReportPeriodTestsGroup, Serialize) {
    uint8_t buffer[256] = {};
    DecryptionErrorReportPeriod element_0{ 5, 420 };
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 7, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x10, 0x00, 0x03, 0x05, 0x01, 0xA4 };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto element = DecryptionErrorReportPeriod::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(&buffer[0] + 7, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::DecryptionErrorReportPeriod, element->GetElementType());
    CHECK_EQUAL(5, element->RadioID());
    CHECK_EQUAL(420, element->ReportInterval());
}

TEST(DecryptionErrorReportPeriodTestsGroup, Serialize_Deserialize_few_elements) {
    uint8_t buffer[2048] = {};

    WritableDecryptionErrorReportPeriodArray write_data;
    write_data.Add({ 0, 10 });
    write_data.Add({ 1, 100 });
    write_data.Add({ 2, 1000 });
    write_data.Add({ 3, 10000 });
    write_data.Add({ 31, 65535 });

    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 35, raw_data.current);
    uint8_t reference[] = { 0x00, 0x10, 0x00, 0x03, 0x00, 0x00, 0x0A, 0x00, 0x10, 0x00, 0x03, 0x01,
                            0x00, 0x64, 0x00, 0x10, 0x00, 0x03, 0x02, 0x03, 0xE8, 0x00, 0x10, 0x00,
                            0x03, 0x03, 0x27, 0x10, 0x00, 0x10, 0x00, 0x03, 0x1F, 0xFF, 0xFF };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    ReadableDecryptionErrorReportPeriodArray periods;

    raw_data = { reference, reference + sizeof(reference) };

    CHECK_TRUE(periods.Deserialize(&raw_data));
    CHECK_TRUE(periods.Deserialize(&raw_data));
    CHECK_TRUE(periods.Deserialize(&raw_data));
    CHECK_TRUE(periods.Deserialize(&raw_data));
    CHECK_TRUE(periods.Deserialize(&raw_data));
    CHECK_EQUAL(5, periods.Get().size());

    CHECK_EQUAL(0, periods.Get()[0]->RadioID());
    CHECK_EQUAL(10, periods.Get()[0]->ReportInterval());
    CHECK_EQUAL(1, periods.Get()[1]->RadioID());
    CHECK_EQUAL(100, periods.Get()[1]->ReportInterval());
    CHECK_EQUAL(2, periods.Get()[2]->RadioID());
    CHECK_EQUAL(1000, periods.Get()[2]->ReportInterval());
    CHECK_EQUAL(3, periods.Get()[3]->RadioID());
    CHECK_EQUAL(10000, periods.Get()[3]->ReportInterval());
    CHECK_EQUAL(31, periods.Get()[4]->RadioID());
    CHECK_EQUAL(65535, periods.Get()[4]->ReportInterval());
}

TEST(DecryptionErrorReportPeriodTestsGroup, Add_array_of_items_is_unique) {
    uint8_t buffer[2048] = {};

    WritableDecryptionErrorReportPeriodArray write_data;
    write_data.Add({ 0, 10 });
    write_data.Add({ 0, 10 });
    write_data.Add({ 0, 10 });
    write_data.Add({ 1, 100 });
    write_data.Add({ 1, 100 });
    write_data.Add({ 1, 100 });
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    write_data.Serialize(&raw_data);

    ReadableDecryptionErrorReportPeriodArray periods;

    auto data_size = raw_data.current - buffer;
    raw_data = { buffer, buffer + data_size };
    CHECK_TRUE(periods.Deserialize(&raw_data));
    CHECK_TRUE(periods.Deserialize(&raw_data));
    CHECK_FALSE(periods.Deserialize(&raw_data));

    CHECK_EQUAL(2, periods.Get().size());

    CHECK_EQUAL(0, periods.Get()[0]->RadioID());
    CHECK_EQUAL(10, periods.Get()[0]->ReportInterval());
    CHECK_EQUAL(1, periods.Get()[1]->RadioID());
    CHECK_EQUAL(100, periods.Get()[1]->ReportInterval());
}
