#pragma once

#include "CapwapMessage.h"
#include "IElement.h"
#include "elements/VendorSpecificPayload.h"
#include "span.hpp"
#include <limits>
#include <unordered_map>

struct WritableEchoResponse : WritableCapwapResponse {
  private:
    nonstd::span<IWritableEchoResponseOptionalElement *const> optional_elements;

  public:
    WritableEchoResponse(const WritableEchoResponse &) = delete;
    WritableEchoResponse(
        nonstd::span<IWritableEchoResponseOptionalElement *const> optional_elements);
    WritableEchoResponse(
        std::initializer_list<IWritableEchoResponseOptionalElement *const> optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    ControlHeader::MessageType GetRequestMessageType() const override final;
    void Serialize(RawData *raw_data) const override final;
};

struct ReadableEchoResponse : ReadableCapwapResponse {
  protected:
    std::unordered_map<ElementHeader::ElementType, IReadableEchoResponseOptionalElement *const>
        key_optional_elements;

    std::unordered_map<ElementHeader::ElementType, IReadableEchoResponseOptionalElement *const>
    MapOptionalsElements(
        nonstd::span<IReadableEchoResponseOptionalElement *const> optional_elements);

  public:
    size_t unknown_elements;

    ReadableEchoResponse(const ReadableEchoResponse &) = delete;
    ReadableEchoResponse(
        nonstd::span<IReadableEchoResponseOptionalElement *const> optional_elements);
    ReadableEchoResponse(
        std::initializer_list<IReadableEchoResponseOptionalElement *> optional_elements);

    ControlHeader::MessageType GetMessageType() const override final;
    bool Deserialize(RawData *raw_data) override final;
    void Log() const;

    template <typename T> T *GetOptionalElement(ElementHeader::ElementType element_type) {
        auto it = key_optional_elements.find(element_type);
        if (it != key_optional_elements.end()) {
            return static_cast<T *>(it->second);
        }
        return nullptr;
    }
};
