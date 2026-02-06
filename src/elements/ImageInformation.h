#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <string_view>

struct __attribute__((packed)) ImageInformation : ElementHeader {
  protected:
    NetworkU32 file_size;

  public:
    uint8_t file_hash[16];

    ImageInformation(const ImageInformation &) = delete;
    ImageInformation();
    ImageInformation(uint32_t file_size, const nonstd::span<const uint8_t> &hash);

    uint32_t GetFileSize() const;
    bool Validate() const;
    uint16_t GetTotalLength() const;
    void Serialize(RawData *raw_data) const;
    static ImageInformation *Deserialize(RawData *raw_data);
    void Log() const;
};
