#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "IElement.h"
#include "elements/ElementHeader.h"
#include "elements/MacAclEntriesHeader.h"
#include "elements/MacAddress.h"
#include "span.hpp"
#include <vector>

struct WritableAddMacAclEntry : IWritableElement {

  protected:
    std::vector<MacAddress> items;

  public:
    WritableAddMacAclEntry(const WritableAddMacAclEntry &) = delete;
    WritableAddMacAclEntry();

    void Add(MacAddress mac_address);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableAddMacAclEntry : IReadableElement {
  public:
    MacAclEntriesHeader *header;

  protected:
    std::array<const ReadableMacAddress *, MacAclEntriesHeader::max_count> entries;
    size_t count;

  public:
    ReadableAddMacAclEntry(const ReadableAddMacAclEntry &) = delete;
    ReadableAddMacAclEntry();

    bool Deserialize(RawData *raw_data) override final;
    nonstd::span<const ReadableMacAddress *const> Get() const;
    void Log() const override final;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};
