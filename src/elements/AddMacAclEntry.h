#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "elements/ElementHeader.h"
#include "elements/MacAclEntriesHeader.h"
#include "elements/MacAddress.h"
#include "span.hpp"
#include <vector>

struct WritableAddMacAclEntry {

  protected:
    std::vector<MacAddress> items;

  public:
    WritableAddMacAclEntry(const WritableAddMacAclEntry &) = delete;
    WritableAddMacAclEntry();

    void Add(MacAddress mac_address);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const;
    void Log() const;
};

struct ReadableAddMacAclEntry {
  public:
    MacAclEntriesHeader *header;

  protected:
    std::array<const ReadableMacAddress *, MacAclEntriesHeader::max_count> entries;
    size_t count;

  public:
    ReadableAddMacAclEntry(const ReadableAddMacAclEntry &) = delete;
    ReadableAddMacAclEntry();

    bool Deserialize(RawData *raw_data);
    nonstd::span<const ReadableMacAddress *const> Get() const;
    void Log() const;
};
