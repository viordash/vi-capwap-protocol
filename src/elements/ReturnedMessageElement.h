#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "Helpers.h"
#include "elements/ElementHeader.h"
#include "lassert.h"
#include "span.hpp"
#include <string_view>
#include <vector>

struct __attribute__((packed)) ReturnedMessageElement : ElementHeader {
  public:
    static const size_t max_data_size = 255;

    enum Reasons : uint8_t {
        Reserved = 0,
        UnknownMessageElement = 1,
        UnsupportedMessageElement = 2,
        UnknownMessageElementValue = 3,
        UnsupportedMessageElementValue = 4
    };

  protected:
    // Reason:   The reason the configuration in the offending message element could not be applied by the WTP
    Reasons reason;

    // Length:   The length of the Message Element field, which MUST NOT exceed 255 octets.
    uint8_t data_length;

  public:
    uint8_t data[];

    ReturnedMessageElement(const ReturnedMessageElement &) = default;
    ReturnedMessageElement(Reasons reason, uint16_t data_length);

    Reasons GetReason() const;
    uint8_t GetDataLength() const;
    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static ReturnedMessageElement *Deserialize(RawData *raw_data);
};

struct WritableReturnedMessageElementArray {
  public:
    struct Item {
        std::vector<uint8_t> data;
        ReturnedMessageElement header;
        Item(const Item &) = default;
        Item(ReturnedMessageElement::Reasons reason, std::vector<uint8_t> &&val)
            : data{ std::move(val) }, header{ reason, (uint16_t)data.size() } {};
    };

  private:
    std::vector<Item> items;

  public:
    WritableReturnedMessageElementArray(const WritableReturnedMessageElementArray &) = delete;
    WritableReturnedMessageElementArray(const nonstd::span<const Item> &items);
    WritableReturnedMessageElementArray();

    void Add(ReturnedMessageElement::Reasons reason, std::vector<uint8_t> val);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const;

    bool Validate() const;
    void Log() const;
};

struct ReadableReturnedMessageElementArray {
    static const size_t max_count = 32;

  protected:
    std::array<const ReturnedMessageElement *, max_count> items;
    size_t count;

  public:
    ReadableReturnedMessageElementArray(const ReadableReturnedMessageElementArray &) = delete;
    ReadableReturnedMessageElementArray();

    bool Deserialize(RawData *raw_data);
    nonstd::span<const ReturnedMessageElement *const> Get() const;
    void Log() const;
};
