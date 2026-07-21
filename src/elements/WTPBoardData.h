#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "IElement.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <string_view>

struct __attribute__((packed)) WTPBoardDataHeader : ElementHeader {
  private:
    // SMI Network Management Private Enterprise Codes. MUST NOT be set to zero
    NetworkU32 vendor_id;

  public:
    WTPBoardDataHeader(const WTPBoardDataHeader &) = delete;
    WTPBoardDataHeader(uint32_t vendor_identifier, uint16_t sub_elements_size);

    uint32_t GetVendorIdentifier() const;
    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static WTPBoardDataHeader *Deserialize(RawData *raw_data);
};

struct __attribute__((packed)) BoardDataSubElementHeader {
  public:
    enum Type : uint16_t {
        WTPModelNumber = ToNetworkOrder16(0),  // MUST be included
        WTPSerialNumber = ToNetworkOrder16(1), // MUST be included
        BoardID = ToNetworkOrder16(2),         // MAY be included
        BoardRevision = ToNetworkOrder16(3),   // MAY be included
        BaseMACAddress = ToNetworkOrder16(4)   // MAY be included
    };

  private:
    Type type;
    // MUST NOT exceed 1024 octets
    NetworkU16 length;

  public:
    char value[];

    BoardDataSubElementHeader(const BoardDataSubElementHeader &) = delete;
    BoardDataSubElementHeader(Type type, uint16_t length);

    Type GetType() const;
    uint16_t GetLength() const;
    bool Validate() const;
};

struct WritableWTPBoardData : IWritableElement {
  public:
    struct __attribute__((packed)) SubElement {
        const char *value;
        BoardDataSubElementHeader header;
        SubElement(const SubElement &) = delete;
        SubElement(BoardDataSubElementHeader::Type type, std::string_view string)
            : value{ string.data() }, header{ type, (uint16_t)string.size() } {};
    };

  private:
    WTPBoardDataHeader header;
    const nonstd::span<const SubElement> items;

    static uint16_t GetSubElementsSize(const nonstd::span<const SubElement> &items);

  public:
    WritableWTPBoardData(const WritableWTPBoardData &) = delete;
    WritableWTPBoardData(const uint32_t vendor_identifier,
                         const nonstd::span<const SubElement> &items);

    void Serialize(RawData *raw_data) const override final;

    uint16_t GetTotalLength() const;
    void Log() const override final;
};

struct ReadableWTPBoardData : IReadableElement {
  public:
    static const size_t max_count = 10; //BoardDataSubElementHeader::Type * 2
    WTPBoardDataHeader *header;

  private:
    std::array<const BoardDataSubElementHeader *, max_count> items;
    size_t count;

  public:
    ReadableWTPBoardData(const ReadableWTPBoardData &) = delete;
    ReadableWTPBoardData();

    bool Deserialize(RawData *raw_data) override final;
    nonstd::span<const BoardDataSubElementHeader *const> Get() const;
    void Log() const override final;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};
