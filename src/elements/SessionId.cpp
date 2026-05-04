#include "SessionId.h"
#include "Logging.h"
#include <cstring>
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
    return GetElementType() == ElementHeader::SessionID
        && GetLength() == (sizeof(SessionId) - sizeof(ElementHeader));
}

void SessionId::Log() const {
    log_i("ME SessionId: %s", ToString().c_str());
}

WritableSessionId::WritableSessionId(const SessionId &session_id) : element{ session_id } {
    static_assert(sizeof(SessionId) == 20);
}

void WritableSessionId::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(SessionId) <= raw_data->end);
    std::memcpy(raw_data->current, &element, sizeof(SessionId));
    raw_data->current += sizeof(SessionId);
}

void WritableSessionId::Log() const {
    element.Log();
}

const SessionId &WritableSessionId::Get() const {
    return element;
}

bool ReadableSessionId::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(SessionId) > raw_data->end) {
        return false;
    }

    auto res = (SessionId *)raw_data->current;
    if (!res->Validate()) {
        return false;
    }
    raw_data->current += sizeof(SessionId);

    element = res;
    is_present = true;
    return true;
}

SessionId *const ReadableSessionId::Get() const {
    return element;
}

void ReadableSessionId::Log() const {
    ASSERT(element != nullptr);
    element->Log();
}

ElementHeader::ElementType ReadableSessionId::GetElementType() const {
    return ElementHeader::SessionID;
}

bool ReadableSessionId::IsPresent() const {
    return is_present;
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