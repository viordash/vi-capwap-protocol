#pragma once

#include "CapwapMessage.h"
#include "IElement.h"
#include "elements/VendorSpecificPayload.h"
#include "span.hpp"
#include <limits>
#include <unordered_map>
#include <vector>

struct WritableEchoRequest : WritableCapwapRequest {

  private:
    nonstd::span<IWritableEchoRequestOptionalElement *const> optional_elements;

  public:
    WritableEchoRequest(const WritableEchoRequest &) = delete;
    WritableEchoRequest(nonstd::span<IWritableEchoRequestOptionalElement *const> optional_elements);
    WritableEchoRequest(
        std::initializer_list<IWritableEchoRequestOptionalElement *const> optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetResponseMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableEchoRequest : ReadableCapwapRequest {
  protected:
    std::unordered_map<ElementHeader::ElementType, IReadableEchoRequestOptionalElement *const>
        key_optional_elements;

    std::unordered_map<ElementHeader::ElementType, IReadableEchoRequestOptionalElement *const>
    MapOptionalsElements(
        nonstd::span<IReadableEchoRequestOptionalElement *const> optional_elements);

  public:
    size_t unknown_elements;

    ReadableEchoRequest(const ReadableEchoRequest &) = delete;
    ReadableEchoRequest(nonstd::span<IReadableEchoRequestOptionalElement *const> optional_elements);
    ReadableEchoRequest(
        std::initializer_list<IReadableEchoRequestOptionalElement *> optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;
};
