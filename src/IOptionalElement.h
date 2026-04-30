#pragma once
#include "Helpers.h"
#include "elements/ElementHeader.h"

struct IOptionalElement {
    virtual void Log() const = 0;
};

struct IWritableOptionalElement : IOptionalElement {
    virtual void Serialize(RawData *raw_data) const = 0;
};

struct IReadableOptionalElement : IOptionalElement {
    virtual bool Deserialize(RawData *raw_data) = 0;
    virtual ElementHeader::ElementType GetElementType() const = 0;
    virtual bool IsPresent() const = 0;
};

struct IWritableConfigurationStatusRequestOptionalElement : IWritableOptionalElement {};
struct IReadableConfigurationStatusRequestOptionalElement : IReadableOptionalElement {};
