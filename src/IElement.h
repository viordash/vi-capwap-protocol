#pragma once
#include "Helpers.h"
#include "elements/ElementHeader.h"

struct IElement {
    virtual void Log() const = 0;
};

struct IWritableElement : IElement {
    virtual void Serialize(RawData *raw_data) const = 0;
};

struct IReadableElement : IElement {
    virtual bool Deserialize(RawData *raw_data) = 0;
    virtual ElementHeader::ElementType GetElementType() const = 0;
    virtual bool IsPresent() const = 0;
};

struct IWritableConfigurationStatusRequestOptionalElement : IWritableElement {};
struct IReadableConfigurationStatusRequestOptionalElement : IReadableElement {};

struct IWritableConfigurationStatusResponseOptionalElement : IWritableElement {};
struct IReadableConfigurationStatusResponseOptionalElement : IReadableElement {};

struct IWritableConfigurationUpdateRequestOptionalElement : IWritableElement {};
struct IReadableConfigurationUpdateRequestOptionalElement : IReadableElement {};

struct IWritableJoinRequestOptionalElement : IWritableElement {};
struct IReadableJoinRequestOptionalElement : IReadableElement {};

struct IWritableJoinResponseOptionalElement : IWritableElement {};
struct IReadableJoinResponseOptionalElement : IReadableElement {};

struct IWritableImageDataRequestOptionalElement : IWritableElement {};
struct IReadableImageDataRequestOptionalElement : IReadableElement {};

struct IWritableImageDataResponseOptionalElement : IWritableElement {};
struct IReadableImageDataResponseOptionalElement : IReadableElement {};
struct IWritableDiscoveryRequestOptionalElement : IWritableElement {
    virtual uint16_t GetTotalLength() const = 0;
};
struct IReadableDiscoveryRequestOptionalElement : IReadableElement {};

struct IWritableDiscoveryResponseOptionalElement : IWritableElement {};
struct IReadableDiscoveryResponseOptionalElement : IReadableElement {};

struct IWritableConfigurationUpdateResponseOptionalElement : IWritableElement {};
struct IReadableConfigurationUpdateResponseOptionalElement : IReadableElement {};

struct IWritableEchoRequestOptionalElement : IWritableElement {};
struct IReadableEchoRequestOptionalElement : IReadableElement {};

struct IWritableEchoResponseOptionalElement : IWritableElement {};
struct IReadableEchoResponseOptionalElement : IReadableElement {};

struct IWritableResetRequestOptionalElement : IWritableElement {};
struct IReadableResetRequestOptionalElement : IReadableElement {};

struct IWritableWTPEventRequestOptionalElement : IWritableElement {};
struct IReadableWTPEventRequestOptionalElement : IReadableElement {};

struct IWritableWTPEventResponseOptionalElement : IWritableElement {};
struct IReadableWTPEventResponseOptionalElement : IReadableElement {};

struct IWritableChangeStateEventRequestOptionalElement : IWritableElement {
    virtual ElementHeader::ElementType GetElementType() const = 0;
};
struct IReadableChangeStateEventRequestOptionalElement : IReadableElement {};

struct IWritableChangeStateEventResponseOptionalElement : IWritableElement {};
struct IReadableChangeStateEventResponseOptionalElement : IReadableElement {};

struct IWritableWlanConfigurationRequestOptionalElement : IWritableElement {};
struct IReadableWlanConfigurationRequestOptionalElement : IReadableElement {};

struct IWritableWlanConfigurationResponseOptionalElement : IWritableElement {};
struct IReadableWlanConfigurationResponseOptionalElement : IReadableElement {};

struct IWritableResetResponseOptionalElement : IWritableElement {};
struct IReadableResetResponseOptionalElement : IReadableElement {};