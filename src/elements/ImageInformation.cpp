
#include "ImageInformation.h"
#include "Logging.h"
#include "lassert.h"
#include <cstring>

ImageInformation::ImageInformation()
    : ElementHeader(ElementHeader::ImageInformation,
                    (sizeof(ImageInformation) - sizeof(ElementHeader))),
      file_size{ 0 } {
    memset(file_hash, 0, sizeof(file_hash));
}

ImageInformation::ImageInformation(uint32_t file_size, const nonstd::span<const uint8_t> &hash)
    : ElementHeader(ElementHeader::ImageInformation,
                    (sizeof(ImageInformation) - sizeof(ElementHeader))),
      file_size{ file_size } {
    memcpy(file_hash, hash.data(), std::min(sizeof(file_hash), hash.size()));
}

uint32_t ImageInformation::GetFileSize() const {
    return file_size.Get();
}

bool ImageInformation::Validate() const {
    static_assert(sizeof(ImageInformation) == 24);
    return GetElementType() == ElementHeader::ImageInformation
        && GetLength() == (sizeof(ImageInformation) - sizeof(ElementHeader));
}

void ImageInformation::Log() const {
    log_i("ME ImageInformation FileSize: {}, hash: "
          "{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}{:"
          "02X}{:02X}",
          GetFileSize(),
          file_hash[0],
          file_hash[1],
          file_hash[2],
          file_hash[3],
          file_hash[4],
          file_hash[5],
          file_hash[6],
          file_hash[7],
          file_hash[8],
          file_hash[9],
          file_hash[10],
          file_hash[11],
          file_hash[12],
          file_hash[13],
          file_hash[14],
          file_hash[15]);
}

WritableImageInformation::WritableImageInformation(uint32_t file_size,
                                                   const nonstd::span<const uint8_t> &hash)
    : element{ file_size, hash } {
    static_assert(sizeof(element) == 24);
}

void WritableImageInformation::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(ImageInformation) <= raw_data->end);
    std::memcpy(raw_data->current, &element, sizeof(element));
    raw_data->current += sizeof(element);
}

void WritableImageInformation::Log() const {
    element.Log();
}

bool ReadableImageInformation::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ImageInformation) > raw_data->end) {
        return false;
    }

    auto res = (ImageInformation *)raw_data->current;
    if (!res->Validate()) {
        return false;
    }

    uint8_t *last = raw_data->current + sizeof(ElementHeader) + res->GetLength();
    if (last > raw_data->end) {
        return false;
    }

    raw_data->current = last;
    element = res;
    is_present = true;
    return true;
}

const ImageInformation *ReadableImageInformation::Get() const {
    return element;
}

void ReadableImageInformation::Log() const {
    ASSERT(element != nullptr);
    element->Log();
}

ElementHeader::ElementType ReadableImageInformation::GetElementType() const {
    return ElementHeader::ImageInformation;
}

bool ReadableImageInformation::IsPresent() const {
    return is_present;
}
