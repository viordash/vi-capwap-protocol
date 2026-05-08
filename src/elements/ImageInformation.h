#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "IElement.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <string_view>

struct __attribute__((packed)) ImageInformation : ElementHeader {
  protected:
    NetworkU32 file_size;

  public:
    uint8_t file_hash[16];

    ImageInformation(const ImageInformation &) = default;
    ImageInformation();
    ImageInformation(uint32_t file_size, const nonstd::span<const uint8_t> &hash);

    uint32_t GetFileSize() const;
    bool Validate() const;
    void Log() const;
};

struct WritableImageInformation : IWritableImageDataResponseOptionalElement {
  protected:
    ImageInformation element;

  public:
    WritableImageInformation(uint32_t file_size, const nonstd::span<const uint8_t> &hash);

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableImageInformation : IReadableImageDataResponseOptionalElement {
  protected:
    ImageInformation *element = nullptr;
    bool is_present = false;

  public:
    bool Deserialize(RawData *raw_data) override final;
    void Log() const override final;
    const ImageInformation *Get() const;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};
