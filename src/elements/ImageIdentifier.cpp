
#include "ImageIdentifier.h"
#include "lassert.h"
#include "logging.h"
#include <string.h>

ImageIdentifierHeader::ImageIdentifierHeader(uint32_t vendor_identifier, uint16_t length)
    : ElementHeader(ElementHeader::ImageIdentifier,
                    (sizeof(ImageIdentifierHeader) - sizeof(ElementHeader)) + length),
      vendor_id{ vendor_identifier } {
}

bool ImageIdentifierHeader::Validate() const {
    static_assert(sizeof(ImageIdentifierHeader) == 8);
    return GetElementType() == ElementHeader::ImageIdentifier
        && GetLength() >= (sizeof(ImageIdentifierHeader) - sizeof(ElementHeader))
        && GetLength() <= ImageIdentifierHeader::max_data_size
                              + (sizeof(ImageIdentifierHeader) - sizeof(ElementHeader));
}

WritableImageIdentifier::WritableImageIdentifier(uint32_t vendor_identifier,
                                                 const std::string_view str)
    : data{ str.begin(), str.end() }, header{ vendor_identifier, (uint16_t)str.size() } {
}

void WritableImageIdentifier::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(ImageIdentifierHeader) + data.size() <= raw_data->end);

    memcpy(raw_data->current, &header, sizeof(header));
    raw_data->current += sizeof(header);

    memcpy(raw_data->current, data.data(), data.size());
    raw_data->current += data.size();
}

void WritableImageIdentifier::Log() const {
    log_i("ME ImageIdentifierHeader VendorId:%u, Value:%.*s",
          header.vendor_id.Get(),
          (int)(header.GetLength() - (sizeof(ImageIdentifierHeader) - sizeof(ElementHeader))),
          data.data());
}

ReadableImageIdentifier::ReadableImageIdentifier() : header{}, data{ nullptr } {
}

bool ReadableImageIdentifier::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ElementHeader) > raw_data->end) {
        return false;
    }

    auto res = (ImageIdentifierHeader *)raw_data->current;
    if (!res->Validate()) {
        return false;
    }
    if (raw_data->current + sizeof(ElementHeader) + res->GetLength() > raw_data->end) {
        return false;
    }
    header = res;

    raw_data->current += sizeof(ImageIdentifierHeader);

    data = (const char *)raw_data->current;

    raw_data->current += res->GetLength() - (sizeof(ImageIdentifierHeader) - sizeof(ElementHeader));

    return true;
}

uint32_t ReadableImageIdentifier::GetVendorIdentifier() const {
    if (header == nullptr) {
        return {};
    }
    return header->vendor_id.Get();
}

const std::string_view ReadableImageIdentifier::GetData() const {
    if (header == nullptr) {
        return {};
    }
    return { data, header->GetLength() - (sizeof(ImageIdentifierHeader) - sizeof(ElementHeader)) };
}

void ReadableImageIdentifier::Log() const {
    if (header == nullptr) {
        return;
    }
    log_i("ME ImageIdentifierHeader VendorId:%u, Value:%.*s",
          header->vendor_id.Get(),
          (int)(header->GetLength() - (sizeof(ImageIdentifierHeader) - sizeof(ElementHeader))),
          (char *)data);
}
