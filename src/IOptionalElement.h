#pragma once
#include "Helpers.h"

struct IOptionalElement {
    virtual void Log() const = 0;
};

struct IWritableOptionalElement : IOptionalElement {
    virtual void Serialize(RawData *raw_data) const = 0;
};

struct IReadableOptionalElement : IOptionalElement {
    virtual bool Deserialize(RawData *raw_data) = 0;
};

struct IWritableConfigurationStatusRequestOptionalElement : IWritableOptionalElement {};

struct IReadableConfigurationStatusRequestOptionalElement : IReadableOptionalElement {};
