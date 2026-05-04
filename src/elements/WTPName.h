#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "IElement.h"
#include "elements/ElementHeader.h"
#include <string_view>
#include <vector>

struct __attribute__((packed)) WTPName : ElementHeader {
    static const size_t max_data_size = 512;

    WTPName(const WTPName &) = delete;
    WTPName(uint16_t length);
    uint16_t GetDataLenght() const;

    bool Validate() const;
};

struct WritableWTPName : IWritableConfigurationUpdateRequestOptionalElement {
  protected:
    WTPName element;
    const std::string_view name;

  public:
    WritableWTPName(const WritableWTPName &) = delete;
    WritableWTPName(const std::string_view location);

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableWTPName : IReadableConfigurationUpdateRequestOptionalElement {
  public:
    struct Element : WTPName {
        char name[];
        Element(const Element &) = delete;
    };

  protected:
    Element *element = nullptr;
    bool is_present = false;

  public:
    bool Deserialize(RawData *raw_data) override final;
    void Log() const override final;
    ReadableWTPName::Element *const Get() const;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};
