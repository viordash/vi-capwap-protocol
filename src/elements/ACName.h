#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "IElement.h"
#include "elements/ElementHeader.h"
#include <string_view>
#include <vector>

struct __attribute__((packed)) ACName : ElementHeader {
    static const size_t max_data_size = 512;

    ACName(const ACName &) = delete;
    ACName(uint16_t length);
    uint16_t GetDataLenght() const;

    bool Validate() const;
};

struct WritableACName : IWritableElement {
  protected:
    ACName element;
    const std::string_view name;

  public:
    WritableACName(const WritableACName &) = delete;
    WritableACName(const std::string_view name);

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableACName : IReadableElement {
  public:
    struct Element : ACName {
        char name[];
        Element(const Element &) = delete;
    };

  protected:
    Element *element = nullptr;
    bool is_present = false;

  public:
    bool Deserialize(RawData *raw_data) override final;
    void Log() const override final;
    const ReadableACName::Element *Get() const;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};
