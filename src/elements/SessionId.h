#pragma once

#include "elements/ElementHeader.h"
#include "lassert.h"
#include "span.hpp"
#include <cstdint>
#include <cstring>

struct AlignedSessionId;

struct __attribute__((packed)) SessionId : ElementHeader {
    uint8_t session_id[16];

    SessionId();
    SessionId(const SessionId &) = default;
    SessionId(const AlignedSessionId &aligned_id);

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static SessionId *Deserialize(RawData *raw_data);

    uint16_t GetTotalLength() const;
    void Log() const;

    bool operator==(const nonstd::span<const uint8_t> &other) const {
        return sizeof(SessionId::session_id) == other.size()
            && std::memcmp(session_id, other.data(), sizeof(SessionId::session_id)) == 0;
    }
    bool operator!=(const nonstd::span<const uint8_t> &other) const {
        return !(*this == other);
    }

    bool operator==(const SessionId &other) const {
        return std::memcmp(session_id, other.session_id, sizeof(SessionId::session_id)) == 0;
    }
    bool operator!=(const SessionId &other) const {
        return !(*this == other);
    }

    static std::string ToString(const nonstd::span<const uint8_t> &session_id);
    std::string ToString() const;
};

struct AlignedSessionId {
  protected:
    size_t hash;
    uint8_t session_id[sizeof(SessionId::session_id)];

    friend SessionId;

  public:
    AlignedSessionId() = default;
    AlignedSessionId(const nonstd::span<const uint8_t> unaligned_id);

    bool operator==(const AlignedSessionId &other) const {
        ASSERT(sizeof(session_id) == sizeof(uint64_t) * 2);
        auto val0 = reinterpret_cast<const uint64_t *>(&session_id[sizeof(uint64_t) * 0]);
        auto val1 = reinterpret_cast<const uint64_t *>(&session_id[sizeof(uint64_t) * 1]);

        auto other_val0 =
            reinterpret_cast<const uint64_t *>(&other.session_id[sizeof(uint64_t) * 0]);
        auto other_val1 =
            reinterpret_cast<const uint64_t *>(&other.session_id[sizeof(uint64_t) * 1]);

        return *val0 == *other_val0 && *val1 == *other_val1;
    }
    bool operator!=(const AlignedSessionId &other) const {
        return !(*this == other);
    }

    const nonstd::span<const uint8_t> Get() const;
    size_t Hash() const;
    std::string ToString() const;
};

namespace std {
    template <> struct hash<AlignedSessionId> {
        size_t operator()(const AlignedSessionId &session_id) const {
            return session_id.Hash();
        }
    };
} // namespace std