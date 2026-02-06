#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "ClearHeader.h"
#include "DataChannelKeepAlive.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(DataChannelKeepAliveTestsGroup){ //
                                            TEST_SETUP(){}

                                            TEST_TEARDOWN(){}
};

TEST(DataChannelKeepAliveTestsGroup, DataChannelKeepAlive_serialize) {
    uint8_t buffer[4096] = {};
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    const uint8_t id[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                           0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
    AlignedSessionId session_id(id);

    DataChannelKeepAlive write_data(session_id);

    write_data.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 22, raw_data.current);
    const uint8_t reference[] = {
        0x00, 0x14, 0x00, 0x23, 0x00, 0x10, 0x00, 0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
    };

    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + 22 };
    auto *read_data = DataChannelKeepAlive::Deserialize(&raw_data);
    CHECK(read_data != nullptr);

    CHECK(read_data->GetSessionId() == session_id.Get());
}

TEST(DataChannelKeepAliveTestsGroup, DataChannelKeepAlive_deserialize) {
    // clang-format off
    uint8_t data[] = {
        // === Заголовок CAPWAP (8 байт) ===
        // Byte 0: Preamble (Version=0, Type=0) -> 0x00
        // Byte 1: HLEN=2 (header len 8 bytes) -> 0x10 (RID должен быть 0)
        // Byte 2: WBID=1, T=0, F=0, L=0 -> 0x00
        // Byte 3: W=0, M=0, K=1 (Keep-Alive bit set), Flags=0 -> 0x20
        // Bytes 4-7: Fragment ID, Offset, Rsvd = 0 -> 0x00...
        0x00, 0x10, 0x02, 0x08, 0x00, 0x00, 0x00, 0x00, 

        // === Payload Data Channel Keep-Alive (22 байта) ===
        
        // Message Element Length (2 байта)
        // Указывает общую длину следующих элементов. 
        // Здесь только Session ID: 4 байта заголовок элемента + 16 байт данные = 20 (0x0014)
        0x00, 0x14,

        // === Элементы сообщения ===
        // 1. Session ID (Тип 35, Длина 16)
        // Type: 35 (0x23)
        0x00, 0x23, 
        // Length: 16 (0x10)
        0x00, 0x10, 
        // Value: 16 байт ID сессии (пример: 00 01 02 ... 0F)
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };

    ReadableCapwapHeader capwap_header;
    CHECK_TRUE(capwap_header.Deserialize(&raw_data));

    CHECK_TRUE(capwap_header.header->K());

    auto *read_data = DataChannelKeepAlive::Deserialize(&raw_data);
    CHECK(read_data != nullptr);

    const uint8_t session_id[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                   0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
    MEMCMP_EQUAL(session_id, read_data->GetSessionId().session_id, sizeof(session_id));
}
