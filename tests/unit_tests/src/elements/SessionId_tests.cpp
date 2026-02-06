#include <arpa/inet.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "elements/SessionId.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(SessionIdTestsGroup){ //
                                 TEST_SETUP(){}

                                 TEST_TEARDOWN(){}
};

TEST(SessionIdTestsGroup, SessionId_deserialize) {
    // clang-format off
    // Юнит-тест: Session ID - Пример 2 (в стиле UUID)
    // Тип элемента = 35, Длина = 16
    uint8_t data[] = {
        // --- TLV Header (4 байта) ---
        0x00, 0x23,       // Type = 35
        0x00, 0x10,       // Length = 16

        // --- Value (16 байт) ---
        0xF8, 0x1D, 0x4F, 0xAE,
        0x7D, 0xEC, 0x11, 0xD0,
        0xA7, 0x65, 0x00, 0xA0,
        0xC9, 0x1E, 0x6B, 0xF6
    };
    // clang-format on
    RawData raw_data{ data, data + sizeof(data) };
    auto element = SessionId::Deserialize(&raw_data);

    CHECK(element != nullptr);
    CHECK_EQUAL(raw_data.current, raw_data.end);
    CHECK_EQUAL(ElementHeader::ElementType::SessionID, element->GetElementType());
}

TEST(SessionIdTestsGroup, SessionId_serialize) {
    uint8_t buffer[256] = {};
    const uint8_t id[] = { 0xF8, 0x1D, 0x4F, 0xAE, 0x7D, 0xEC, 0x11, 0xD0,
                           0xA7, 0x65, 0x00, 0xA0, 0xC9, 0x1E, 0x6B, 0xF6 };
    SessionId element_0;
    std::memcpy(element_0.session_id, id, sizeof(element_0.session_id));
    RawData raw_data{ buffer, buffer + sizeof(buffer) };

    element_0.Serialize(&raw_data);
    CHECK_EQUAL(&buffer[0] + 20, raw_data.current);
    const uint8_t reference[] = { 0x00, 0x23, 0x00, 0x10, 0xF8, 0x1D, 0x4F, 0xAE, 0x7D, 0xEC,
                                  0x11, 0xD0, 0xA7, 0x65, 0x00, 0xA0, 0xC9, 0x1E, 0x6B, 0xF6 };
    MEMCMP_EQUAL(buffer, reference, sizeof(reference));

    raw_data = { buffer, buffer + sizeof(buffer) };
    auto element = SessionId::Deserialize(&raw_data);
    CHECK(element != nullptr);
    CHECK_EQUAL(&buffer[0] + 20, raw_data.current);
    CHECK_EQUAL(ElementHeader::ElementType::SessionID, element->GetElementType());
    MEMCMP_EQUAL(id, element->session_id, 16);
}

TEST(SessionIdTestsGroup, SessionId_equal) {
    const uint8_t id[] = { 0xF8, 0x1D, 0x4F, 0xAE, 0x7D, 0xEC, 0x11, 0xD0,
                           0xA7, 0x65, 0x00, 0xA0, 0xC9, 0x1E, 0x6B, 0xF6 };
    SessionId element_0;
    std::memcpy(element_0.session_id, id, sizeof(element_0.session_id));

    const uint8_t reference_equal[] = { 0xF8, 0x1D, 0x4F, 0xAE, 0x7D, 0xEC, 0x11, 0xD0,
                                        0xA7, 0x65, 0x00, 0xA0, 0xC9, 0x1E, 0x6B, 0xF6 };
    const uint8_t reference_not_equal[] = { 0xF8 + 1, 0x1D, 0x4F, 0xAE, 0x7D, 0xEC, 0x11, 0xD0,
                                            0xA7,     0x65, 0x00, 0xA0, 0xC9, 0x1E, 0x6B, 0xF6 };

    CHECK_TRUE(element_0 == reference_equal);
    CHECK_FALSE(element_0 == reference_not_equal);
}

TEST(SessionIdTestsGroup, SessionId_To_String) {
    const uint8_t id[] = { 0xF8, 0x1D, 0x4F, 0xAE, 0x7D, 0xEC, 0x11, 0xD0,
                           0xA7, 0x65, 0x00, 0xA0, 0xC9, 0x1E, 0x6B, 0xF6 };
    SessionId element_0;
    std::memcpy(element_0.session_id, id, sizeof(element_0.session_id));

    STRCMP_EQUAL("F81D4FAE7DEC11D0A76500A0C91E6BF6", element_0.ToString().c_str());
}

TEST(SessionIdTestsGroup, Fill_from_dtls_session_id) {
    uint8_t dtls_session_id[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
                                  0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    AlignedSessionId element_0(dtls_session_id);

    const uint8_t reference[] = { 0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0xF9, 0xF8,
                                  0xF7, 0xF6, 0xF5, 0xF4, 0xF3, 0xF2, 0xF1, 0xF0 };

    CHECK_EQUAL(sizeof(reference), element_0.Get().size());
    MEMCMP_EQUAL(reference, element_0.Get().data(), sizeof(reference));
}

TEST(SessionIdTestsGroup, AlignedSessionId_equals) {
    uint8_t dtls_session_id_0[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    AlignedSessionId element_0(dtls_session_id_0);

    uint8_t dtls_session_id_1[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    AlignedSessionId element_1(dtls_session_id_1);

    uint8_t dtls_session_id_2[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x05, 0x06, 0x07,
                                    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    AlignedSessionId element_2(dtls_session_id_2);

    CHECK_TRUE(element_0 == element_1);
    CHECK_FALSE(element_0 != element_1);

    CHECK_TRUE(element_0 != element_2);
    CHECK_FALSE(element_0 == element_2);
}
