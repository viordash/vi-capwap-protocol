#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "IOptionalElement.h"
#include "elements/ElementHeader.h"
#include "span.hpp"

struct __attribute__((packed)) ImageDataHeader : ElementHeader {
    static const size_t max_data_size = 1024;
    enum Type : uint8_t { ImageDataIsIncluded = 1, LastImageDataIsIncluded = 2, ErrorOccured = 5 };

    Type type;

    ImageDataHeader(const ImageDataHeader &) = delete;
    ImageDataHeader(ImageDataHeader::Type type, uint16_t length);
    uint16_t GetDataLenght() const;

    bool Validate() const;
};

struct WritableImageData : IWritableImageDataRequestOptionalElement {
  protected:
    ImageDataHeader element;
    const nonstd::span<const uint8_t> data;

  public:
    WritableImageData(const WritableImageData &) = delete;
    WritableImageData(ImageDataHeader::Type type, const nonstd::span<const uint8_t> image_data);

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableImageData : IReadableImageDataRequestOptionalElement {
  public:
    struct Element : ImageDataHeader {
        uint8_t data[];
        Element(const Element &) = delete;
    };

  protected:
    Element *element = nullptr;
    bool is_present = false;

  public:
    bool Deserialize(RawData *raw_data) override final;
    void Log() const override final;
    const ReadableImageData::Element *const Get() const;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};
