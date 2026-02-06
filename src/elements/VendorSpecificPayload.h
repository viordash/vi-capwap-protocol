#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <string_view>
#include <vector>

struct WritableVendorSpecificPayloadArray;
struct __attribute__((packed)) VendorSpecificPayload : ElementHeader {
  protected:
    // SMI Network Management Private Enterprise Codes. MUST NOT be set to zero
    NetworkU32 vendor_id;

    NetworkU16 element_id;

  public:
    char value[];

    VendorSpecificPayload(const VendorSpecificPayload &) = default;
    VendorSpecificPayload(uint32_t vendor_identifier, uint16_t element_id, uint16_t length);

    uint32_t GetVendorIdentifier() const;
    uint32_t GetElementId() const;
    bool Validate() const;
    uint16_t GetTotalLength() const;
    void Serialize(RawData *raw_data) const;
    static VendorSpecificPayload *Deserialize(RawData *raw_data);
    static WritableVendorSpecificPayloadArray Dummy;
};

struct WritableVendorSpecificPayloadArray {
  public:
    struct Item {
        std::vector<char> value;
        VendorSpecificPayload header;
        Item(const Item &) = default;
        Item(uint32_t vendor_identifier, uint16_t element_id, std::vector<char> &&val)
            : value{ std::move(val) },
              header{ vendor_identifier, element_id, (uint16_t)value.size() } {};
    };

  private:
    std::vector<Item> items;

  public:
    WritableVendorSpecificPayloadArray(const WritableVendorSpecificPayloadArray &) = delete;
    WritableVendorSpecificPayloadArray(const nonstd::span<const Item> &items);
    WritableVendorSpecificPayloadArray();

    void Add(uint32_t vendor_identifier, uint16_t element_id, std::vector<char> val);
    void Add(uint32_t vendor_identifier, uint16_t element_id, const std::string_view str);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const;
    uint16_t GetTotalLength() const;
    void Log() const;
};

struct ReadableVendorSpecificPayloadArray {
  public:
    static const size_t max_data_size = 2048;
    static const size_t max_count = 16;

  protected:
    std::array<const VendorSpecificPayload *, max_count> items;
    size_t count;

  public:
    ReadableVendorSpecificPayloadArray(const ReadableVendorSpecificPayloadArray &) = delete;
    ReadableVendorSpecificPayloadArray();

    bool Deserialize(RawData *raw_data);
    nonstd::span<const VendorSpecificPayload *const> Get() const;
    void Log() const;
};
