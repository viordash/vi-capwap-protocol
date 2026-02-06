#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/InitiateDownload.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(InitiateDownloadTestsGroup){ //
                                        TEST_SETUP(){}

                                        TEST_TEARDOWN(){}
};

TEST(InitiateDownloadTestsGroup, InitiateDownload_deserialize) {
    // clang-format off
    uint8_t data[] = {
        // --- TLV Header (4 байта) ---
        0x00, 0x1B,       // Type = 27
        0x00, 0x00,       // Length = 0
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = InitiateDownload::Deserialize(&raw_data);

    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::InitiateDownload, element->GetElementType());
}

TEST(InitiateDownloadTestsGroup, InitiateDownload_serialize) {
    uint8_t buffer[256] = {};
    InitiateDownload element_0;
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 4, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x1B, 0x00, 0x00 };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto element = InitiateDownload::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(&buffer[0] + 4, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::InitiateDownload, element->GetElementType());
}
