#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "IElement.h"
#include "elements/ElementHeader.h"
#include "elements/MacAclEntriesHeader.h"
#include "elements/MacAddress.h"
#include "span.hpp"
#include <vector>

struct WritableDeleteMacAclEntry : IWritableConfigurationUpdateRequestOptionalElement {

  protected:
    std::vector<MacAddress> items;

  public:
    WritableDeleteMacAclEntry(const WritableDeleteMacAclEntry &) = delete;
    WritableDeleteMacAclEntry();

    void Add(MacAddress mac_address);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableDeleteMacAclEntry : IReadableConfigurationUpdateRequestOptionalElement {
  public:
    MacAclEntriesHeader *header;

  protected:
    std::array<const ReadableMacAddress *, MacAclEntriesHeader::max_count> entries;
    size_t count;

  public:
    ReadableDeleteMacAclEntry(const ReadableDeleteMacAclEntry &) = delete;
    ReadableDeleteMacAclEntry();

    bool Deserialize(RawData *raw_data) override final;
    void Log() const override final;
    nonstd::span<const ReadableMacAddress *const> Get() const;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};
