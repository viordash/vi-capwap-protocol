#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "Helpers.h"
#include "IElement.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <string_view>
#include <vector>

struct __attribute__((packed)) ACNameWithPriorityHeader : ElementHeader {
    static const size_t max_data_size = 512;

  protected:
    // Priority: A value between 1 and 255 specifying the priority order of the preferred AC.
    uint8_t priority;

  public:
    char name[];

    ACNameWithPriorityHeader(const ACNameWithPriorityHeader &) = default;
    ACNameWithPriorityHeader(uint8_t priority, uint16_t length);

    uint8_t GetPriority() const;
    uint16_t GetNameLenght() const;
    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static ACNameWithPriorityHeader *Deserialize(RawData *raw_data);
};

struct WritableACNameWithPriorityArray : IWritableConfigurationStatusRequestOptionalElement,
                                         IWritableConfigurationUpdateRequestOptionalElement {
  public:
    struct Item {
        std::string_view name;
        ACNameWithPriorityHeader header;
        Item(const Item &) = default;
        Item(uint8_t priority, const std::string_view ac_name)
            : name{ ac_name }, header{ priority, (uint16_t)name.size() } {};
    };

  private:
    std::vector<Item> items;

  public:
    WritableACNameWithPriorityArray(const WritableACNameWithPriorityArray &) = delete;
    WritableACNameWithPriorityArray();

    void Add(uint8_t priority, const std::string_view str);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const override final;

    void Log() const override final;
};

struct ReadableACNameWithPriorityArray : IReadableConfigurationStatusRequestOptionalElement,
                                         IReadableConfigurationUpdateRequestOptionalElement {
    static const size_t max_count = 32;

  protected:
    std::array<const ACNameWithPriorityHeader *, max_count> items;
    size_t count;

  public:
    ReadableACNameWithPriorityArray(const ReadableACNameWithPriorityArray &) = delete;
    ReadableACNameWithPriorityArray();

    bool Deserialize(RawData *raw_data) override final;
    nonstd::span<const ACNameWithPriorityHeader *const> Get() const;
    void Log() const override final;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};
