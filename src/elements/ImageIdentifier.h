#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "IOptionalElement.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <string_view>
#include <vector>

struct __attribute__((packed)) ImageIdentifierHeader : ElementHeader {
    static const size_t max_data_size = 1024;

    // SMI Network Management Private Enterprise Codes. MUST NOT be set to zero
    NetworkU32 vendor_id;

    ImageIdentifierHeader(const ImageIdentifierHeader &) = default;
    ImageIdentifierHeader(ImageIdentifierHeader &&) = default;
    ImageIdentifierHeader(uint32_t vendor_identifier, uint16_t length);

    ImageIdentifierHeader &operator=(const ImageIdentifierHeader &) = default;

    bool Validate() const;
};

struct WritableImageIdentifier : IWritableJoinResponseOptionalElement,
                                 IWritableImageDataRequestOptionalElement {
  protected:
    ImageIdentifierHeader element;
    std::vector<char> data;

  public:
    WritableImageIdentifier(uint32_t vendor_identifier, const std::string_view image_id);

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableImageIdentifier : IReadableJoinResponseOptionalElement,
                                 IReadableImageDataRequestOptionalElement {
  public:
    struct Element : ImageIdentifierHeader {
        char data[];
        Element(const Element &) = delete;
    };

  protected:
    Element *element = nullptr;
    bool is_present = false;

  public:
    bool Deserialize(RawData *raw_data) override final;
    void Log() const override final;
    const ReadableImageIdentifier::Element *const Get() const;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
    uint32_t GetVendorIdentifier() const;
    const std::string_view GetData() const;
};
