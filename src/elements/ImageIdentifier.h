#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
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

struct WritableImageIdentifier {
  private:
    std::vector<char> data;
    ImageIdentifierHeader header;

  public:
    WritableImageIdentifier(const WritableImageIdentifier &) = default;
    WritableImageIdentifier(WritableImageIdentifier &&) = default;
    WritableImageIdentifier(uint32_t vendor_identifier, const std::string_view str);

    WritableImageIdentifier &operator=(const WritableImageIdentifier &) = default;

    void Serialize(RawData *raw_data) const;
    void Log() const;
};

struct ReadableImageIdentifier {
  protected:
    ImageIdentifierHeader *header;
    const char *data;

  public:
    ReadableImageIdentifier(const ReadableImageIdentifier &) = delete;
    ReadableImageIdentifier();

    bool Deserialize(RawData *raw_data);
    uint32_t GetVendorIdentifier() const;
    const std::string_view GetData() const;
    void Log() const;
};
