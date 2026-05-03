#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "IElement.h"
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
    VendorSpecificPayload(const VendorSpecificPayload &) = default;
    VendorSpecificPayload(uint32_t vendor_identifier, uint16_t element_id, uint16_t length);

    uint32_t GetVendorIdentifier() const;
    uint32_t GetElementId() const;
    bool Validate() const;
    uint16_t GetTotalLength() const;
    static WritableVendorSpecificPayloadArray Dummy;
};

struct WritableVendorSpecificPayloadArray : IWritableConfigurationStatusRequestOptionalElement,
                                            IWritableConfigurationStatusResponseOptionalElement,
                                            IWritableJoinRequestOptionalElement,
                                            IWritableJoinResponseOptionalElement,
                                            IWritableImageDataRequestOptionalElement,
                                            IWritableConfigurationUpdateRequestOptionalElement,
                                            IWritableDiscoveryRequestOptionalElement,
                                            IWritableDiscoveryResponseOptionalElement,
                                            IWritableConfigurationUpdateResponseOptionalElement,
                                            IWritableEchoRequestOptionalElement,
                                            IWritableEchoResponseOptionalElement,
                                            IWritableImageDataResponseOptionalElement,
                                            IWritableResetRequestOptionalElement,
                                            IWritableWTPEventRequestOptionalElement {
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

    void Serialize(RawData *raw_data) const override final;
    uint16_t GetTotalLength() const override final;
    void Log() const override final;
};

struct ReadableVendorSpecificPayloadArray : IReadableConfigurationStatusRequestOptionalElement,
                                            IReadableConfigurationStatusResponseOptionalElement,
                                            IReadableJoinRequestOptionalElement,
                                            IReadableJoinResponseOptionalElement,
                                            IReadableImageDataRequestOptionalElement,
                                            IReadableConfigurationUpdateRequestOptionalElement,
                                            IReadableDiscoveryRequestOptionalElement,
                                            IReadableDiscoveryResponseOptionalElement,
                                            IReadableConfigurationUpdateResponseOptionalElement,
                                            IReadableEchoRequestOptionalElement,
                                            IReadableEchoResponseOptionalElement,
                                            IReadableImageDataResponseOptionalElement,
                                            IReadableResetRequestOptionalElement,
                                            IReadableWTPEventRequestOptionalElement {
  public:
    static const size_t max_data_size = 2048;
    static const size_t max_count = 16;

    struct Item : VendorSpecificPayload {
        char value[];
        Item(const Item &) = delete;
    };

  protected:
    std::array<const Item *, max_count> items;
    size_t count;

  public:
    ReadableVendorSpecificPayloadArray(const ReadableVendorSpecificPayloadArray &) = delete;
    ReadableVendorSpecificPayloadArray();

    bool Deserialize(RawData *raw_data) override final;
    nonstd::span<const ReadableVendorSpecificPayloadArray::Item *const> Get() const;
    void Log() const override final;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};
