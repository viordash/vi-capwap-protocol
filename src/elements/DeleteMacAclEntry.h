#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "elements/ElementHeader.h"
#include "elements/MacAclEntriesHeader.h"
#include "elements/MacAddress.h"
#include "span.hpp"
#include <vector>

struct WritableDeleteMacAclEntry {

  protected:
    std::vector<MacAddress> items;

  public:
    WritableDeleteMacAclEntry(const WritableDeleteMacAclEntry &) = delete;
    WritableDeleteMacAclEntry();

    void Add(MacAddress mac_address);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const;
    uint16_t GetTotalLength() const;
    void Log() const;
};

struct ReadableDeleteMacAclEntry {
  public:
    MacAclEntriesHeader *header;

  protected:
    std::array<const ReadableMacAddress *, MacAclEntriesHeader::max_count> entries;
    size_t count;

  public:
    ReadableDeleteMacAclEntry(const ReadableDeleteMacAclEntry &) = delete;
    ReadableDeleteMacAclEntry();

    bool Deserialize(RawData *raw_data);
    nonstd::span<const ReadableMacAddress *const> Get() const;
    void Log() const;
};
