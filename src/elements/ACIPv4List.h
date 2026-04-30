#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "IOptionalElement.h"
#include "elements/ElementHeader.h"
#include "span.hpp"

struct ACIPv4ListWritePacket : ElementHeader {
    const nonstd::span<const uint32_t> addresses;

    ACIPv4ListWritePacket(const ACIPv4ListWritePacket &) = delete;
    ACIPv4ListWritePacket(const nonstd::span<const uint32_t> addresses);

    void Serialize(RawData *raw_data) const;
    uint16_t GetTotalLength() const;
    void Log() const;
};

struct __attribute__((packed)) ACIPv4ListReadPacket : ElementHeader {
    static const size_t max_count = 16;
    uint32_t addresses[];

    ACIPv4ListReadPacket(const ACIPv4ListReadPacket &) = delete;
    ACIPv4ListReadPacket();

    bool Validate() const;
    static ACIPv4ListReadPacket *Deserialize(RawData *raw_data);
    size_t GetCount() const;
    void Log() const;
};

struct WritableACIPv4List : IWritableJoinResponseOptionalElement {
  protected:
    ACIPv4ListWritePacket element;

  public:
    WritableACIPv4List(const nonstd::span<const uint32_t> addresses);

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableACIPv4List : IReadableJoinResponseOptionalElement {
  protected:
    ACIPv4ListReadPacket *element = nullptr;
    bool is_present = false;

  public:
    bool Deserialize(RawData *raw_data) override final;
    void Log() const override final;
    const ACIPv4ListReadPacket *const Get() const;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};