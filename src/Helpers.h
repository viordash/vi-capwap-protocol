#pragma once
#include <cstdint>
#include <stddef.h>

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