#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
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
        WTPModelNumber = 0x0000,  // MUST be included
        WTPSerialNumber = 0x0100, // MUST be included
        BoardID = 0x0200,         // MAY be included
        BoardRevision = 0x0300,   // MAY be included
        BaseMACAddress = 0x0400   // MAY be included
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
    void Serialize(RawData *raw_data) const;
    static BoardDataSubElementHeader *Deserialize(RawData *raw_data);
};

struct WritableWTPBoardData {
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

    void Serialize(RawData *raw_data) const;

    uint16_t GetTotalLength() const;
    void Log() const;
};

struct ReadableWTPBoardData {
  public:
    static const size_t max_count = 10; //BoardDataSubElementHeader::Type * 2
    WTPBoardDataHeader *header;

  protected:
    std::array<const BoardDataSubElementHeader *, max_count> items;
    size_t count;

  public:
    ReadableWTPBoardData(const ReadableWTPBoardData &) = delete;
    ReadableWTPBoardData();

    bool Deserialize(RawData *raw_data);
    nonstd::span<const BoardDataSubElementHeader *const> Get() const;
    void Log() const;
};
