#pragma once
#include <cstdint>
#include <stddef.h>

// Byte order of the target. Detected from the compiler, may be overridden with
// -DVI_CAPWAP_BIG_ENDIAN=0|1 for toolchains that do not define __BYTE_ORDER__.
#if !defined(VI_CAPWAP_BIG_ENDIAN)
#if defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__)                                       \
    && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define VI_CAPWAP_BIG_ENDIAN 1
#elif defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__)                                  \
    && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define VI_CAPWAP_BIG_ENDIAN 0
#else
#error "vi-capwap-protocol: cannot detect byte order, define VI_CAPWAP_BIG_ENDIAN manually"
#endif
#endif

// Converts a protocol constant to its on-the-wire (network order) representation.
// Protocol constants are stored raw in packed structures, so they must be spelled
// in the target's byte order. Implemented in the header because the result is
// required at compile time (enumerator initializers).
constexpr uint16_t ToNetworkOrder16(uint16_t value) {
#if VI_CAPWAP_BIG_ENDIAN
    return value;
#else
    return (uint16_t)((value >> 8) | (value << 8));
#endif
}

constexpr uint32_t ToNetworkOrder32(uint32_t value) {
#if VI_CAPWAP_BIG_ENDIAN
    return value;
#else
    return ((value >> 24) & 0x000000FF) //
         | ((value >> 8) & 0x0000FF00)  //
         | ((value << 8) & 0x00FF0000)  //
         | ((value << 24) & 0xFF000000);
#endif
}

// Converts an on-the-wire (network order) value back to a host value.
// The conversion is symmetric, these are provided for readability at call sites.
constexpr uint16_t ToHostOrder16(uint16_t value) {
    return ToNetworkOrder16(value);
}

constexpr uint32_t ToHostOrder32(uint32_t value) {
    return ToNetworkOrder32(value);
}

struct RawData {
    uint8_t *current;
    const uint8_t *end;
};

static inline size_t RoundTo4(size_t size) {
    return (size + 3) & ~3;
}

union __attribute__((packed)) NetworkU32 {
  private:
    struct __attribute__((packed)) U8 {
        uint8_t b0;
        uint8_t b1;
        uint8_t b2;
        uint8_t b3;
        U8(uint32_t value)
            : b0{ (uint8_t)(value >> 24) }, b1{ (uint8_t)(value >> 16) },
              b2{ (uint8_t)(value >> 8) }, b3{ (uint8_t)(value & 0xFF) } {
        }
    } u8;
    uint32_t u32;

  public:
    NetworkU32(uint32_t value) : u8{ value } {
    }
    NetworkU32() : u32{} {
    }
    uint32_t Get() const {
        return (u8.b0 << 24) + (u8.b1 << 16) + (u8.b2 << 8) + u8.b3;
    }

    bool operator==(const NetworkU32 &other) const {
        return u32 == other.u32;
    }
};

union __attribute__((packed)) NetworkU16 {
  private:
    struct __attribute__((packed)) U8 {
        uint8_t b0;
        uint8_t b1;
        U8(uint16_t value) : b0{ (uint8_t)(value >> 8) }, b1{ (uint8_t)value } {
        }
    } u8;
    uint16_t u16;

  public:
    NetworkU16(uint16_t value) : u8{ value } {
    }
    NetworkU16() : u16{} {
    }
    uint16_t Get() const {
        return (u8.b0 << 8) + u8.b1;
    }

    bool operator==(const NetworkU16 &other) const {
        return u16 == other.u16;
    }
};

union __attribute__((packed)) NetworkS16 {
  private:
    struct __attribute__((packed)) U8 {
        uint8_t b0;
        uint8_t b1;
        U8(int16_t value) : b0{ (uint8_t)(value >> 8) }, b1{ (uint8_t)value } {
        }
    } u8;
    int16_t s16;

  public:
    NetworkS16(int16_t value) : u8{ value } {
    }
    NetworkS16() : s16{} {
    }
    int16_t Get() const {
        return (u8.b0 << 8) + u8.b1;
    }

    bool operator==(const NetworkS16 &other) const {
        return s16 == other.s16;
    }
};