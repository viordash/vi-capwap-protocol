#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "IElement.h"
#include "elements/ElementHeader.h"
#include "span.hpp"

struct WritableACIPv4List : IWritableJoinResponseOptionalElement {
  public:
    struct Element : ElementHeader {
        Element(const Element &) = delete;
        Element(size_t count);
    };

  protected:
    Element element;
    const nonstd::span<const uint32_t> addresses;

  public:
    WritableACIPv4List(const nonstd::span<const uint32_t> addresses);

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableACIPv4List : IReadableJoinResponseOptionalElement {
  public:
    struct Element : ElementHeader {
        static const size_t max_count = 16;
        uint32_t addresses[];
        Element(const Element &) = delete;
        Element();
        bool Validate() const;
        size_t GetCount() const;
    };

  protected:
    Element *element = nullptr;
    bool is_present = false;

  public:
    bool Deserialize(RawData *raw_data) override final;
    void Log() const override final;
    const ReadableACIPv4List::Element *Get() const;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};