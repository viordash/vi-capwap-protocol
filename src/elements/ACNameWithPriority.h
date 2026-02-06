#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "Helpers.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <string_view>
#include <vector>

struct __attribute__((packed)) ACNameWithPriority : ElementHeader {
    static const size_t max_data_size = 512;

  protected:
    // Priority: A value between 1 and 255 specifying the priority order of the preferred AC.
    uint8_t priority;

  public:
    char name[];

    ACNameWithPriority(const ACNameWithPriority &) = default;
    ACNameWithPriority(uint8_t priority, uint16_t length);

    uint8_t GetPriority() const;
    uint16_t GetNameLenght() const;
    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static ACNameWithPriority *Deserialize(RawData *raw_data);
};

struct WritableACNameWithPriorityArray {
  public:
    struct Item {
        std::vector<char> name;
        ACNameWithPriority header;
        Item(const Item &) = default;
        Item(uint8_t priority, std::vector<char> &&val)
            : name{ std::move(val) }, header{ priority, (uint16_t)name.size() } {};
    };

  private:
    std::vector<Item> items;

  public:
    WritableACNameWithPriorityArray(const WritableACNameWithPriorityArray &) = delete;
    WritableACNameWithPriorityArray();

    void Add(uint8_t priority, const std::string_view str);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const;

    void Log() const;
};

struct ReadableACNameWithPriorityArray {
    static const size_t max_count = 32;

  protected:
    std::array<const ACNameWithPriority *, max_count> items;
    size_t count;

  public:
    ReadableACNameWithPriorityArray(const ReadableACNameWithPriorityArray &) = delete;
    ReadableACNameWithPriorityArray();

    bool Deserialize(RawData *raw_data);
    nonstd::span<const ACNameWithPriority *const> Get() const;
    void Log() const;
};
