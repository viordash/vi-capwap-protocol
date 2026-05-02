#pragma once

#include "IElement.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <cstdint>

struct __attribute__((packed)) InitiateDownload : ElementHeader {
    InitiateDownload(const InitiateDownload &) = delete;
    InitiateDownload();

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static InitiateDownload *Deserialize(RawData *raw_data);

    void Log() const;
};

struct WritableInitiateDownload : IWritableImageDataRequestOptionalElement {
  protected:
    InitiateDownload element;

  public:
    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableInitiateDownload : IReadableImageDataRequestOptionalElement {
  protected:
    InitiateDownload *element = nullptr;
    bool is_present = false;

  public:
    bool Deserialize(RawData *raw_data) override final;
    void Log() const override final;
    const InitiateDownload *const Get() const;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};