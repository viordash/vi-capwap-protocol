#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "IElement.h"
#include "elements/ElementHeader.h"
#include <string_view>

struct __attribute__((packed)) LocationData : ElementHeader {
    static const size_t max_data_size = 1024;

    LocationData(const LocationData &) = delete;
    LocationData(uint16_t length);
    uint16_t GetDataLenght() const;

    bool Validate() const;
};

struct WritableLocationData : IWritableElement {
  protected:
    LocationData element;
    const std::string_view data;

  public:
    WritableLocationData(const WritableLocationData &) = delete;
    WritableLocationData(const std::string_view location);

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableLocationData : IReadableElement {
  public:
    struct Element : LocationData {
        char data[];
        Element(const Element &) = delete;
    };

  protected:
    Element *element = nullptr;
    bool is_present = false;

  public:
    bool Deserialize(RawData *raw_data) override final;
    void Log() const override final;
    const ReadableLocationData::Element *const Get() const;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};
