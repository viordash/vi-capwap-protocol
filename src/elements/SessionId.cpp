#include "SessionId.h"
#include "logging.h"
#include <iomanip>
#include <sstream>
#include <string.h>

SessionId::SessionId()
    : ElementHeader(ElementHeader::SessionID, sizeof(SessionId) - sizeof(ElementHeader)) {
}

SessionId::SessionId(const AlignedSessionId &aligned_id) : SessionId() {
    ASSERT(sizeof(aligned_id.session_id) == sizeof(session_id));
    std::memcpy(session_id, aligned_id.session_id, sizeof(session_id));
}

bool SessionId::Validate() const {
    static_assert(sizeof(SessionId) == 20);
    return ElementHeader::GetElementType() == ElementHeader::SessionID
        && ElementHeader::GetLength() == (sizeof(SessionId) - sizeof(ElementHeader));
}
void SessionId::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(SessionId) <= raw_data->end);
    SessionId *dst = (SessionId *)raw_data->current;
    *dst = *this;
    raw_data->current += sizeof(SessionId);
}
SessionId *SessionId::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(SessionId) > raw_data->end) {
        return nullptr;
    }

    auto res = (SessionId *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += sizeof(SessionId);
    return res;
}
uint16_t SessionId::GetTotalLength() const {
    return GetLength() + sizeof(ElementHeader);
}

void SessionId::Log() const {
    log_i("ME SessionId: %s", ToString().c_str());
}

std::string SessionId::ToString(const nonstd::span<const uint8_t> &session_id) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::uppercase;

    for (uint8_t b : session_id) {
        ss << std::setw(2) << static_cast<int>(b);
    }

    return ss.str();
}

std::string SessionId::ToString() const {
    return SessionId::ToString(session_id);
}

AlignedSessionId::AlignedSessionId(const nonstd::span<const uint8_t> unaligned_id) {
    ASSERT(unaligned_id.size() >= sizeof(session_id)
           && unaligned_id.size() % sizeof(session_id) == 0);

    auto val0 = reinterpret_cast<uint64_t *>(&session_id[sizeof(uint64_t) * 0]);
    auto val1 = reinterpret_cast<uint64_t *>(&session_id[sizeof(uint64_t) * 1]);

    std::memcpy(val0, &unaligned_id[sizeof(uint64_t) * 0], sizeof(uint64_t));
    std::memcpy(val1, &unaligned_id[sizeof(uint64_t) * 1], sizeof(uint64_t));

    uint64_t u64;
    switch (unaligned_id.size()) {
        case sizeof(session_id):
            break;
        case sizeof(session_id) * 2:
            std::memcpy(&u64, &unaligned_id[sizeof(uint64_t) * 2], sizeof(uint64_t));
            *val0 = *val0 ^ u64;

            std::memcpy(&u64, &unaligned_id[sizeof(uint64_t) * 3], sizeof(uint64_t));
            *val1 = *val1 ^ u64;
            break;

        default:
            ASSERT(false);
            break;
    }

    hash = *val0 ^ *val1;
}

const nonstd::span<const uint8_t> AlignedSessionId::Get() const {
    return { session_id, sizeof(session_id) };
}

size_t AlignedSessionId::Hash() const {
    return hash;
}

std::string AlignedSessionId::ToString() const {
    return SessionId::ToString(session_id);
}