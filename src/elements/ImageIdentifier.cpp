
#include "ImageIdentifier.h"
#include "Logging.h"
#include "lassert.h"
#include <cstring>
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
                                                 const std::string_view image_id)
    : element(vendor_identifier, image_id.size()), data{ image_id.begin(), image_id.end() } {
    static_assert(sizeof(element) == 8);
    ASSERT(image_id.size() <= ImageIdentifierHeader::max_data_size);
}

void WritableImageIdentifier::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(element) + data.size() <= raw_data->end);

    std::memcpy(raw_data->current, &element, sizeof(element));
    raw_data->current += sizeof(element);

    std::memcpy(raw_data->current, data.data(), data.size());
    raw_data->current += data.size();
}

void WritableImageIdentifier::Log() const {
    log_i("ME ImageIdentifierHeader VendorId:%u, Value:%.*s",
          element.vendor_id.Get(),
          (int)(element.GetLength() - (sizeof(element) - sizeof(ElementHeader))),
          data.data());
}

bool ReadableImageIdentifier::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(ElementHeader) > raw_data->end) {
        return false;
    }

    auto res = (ReadableImageIdentifier::Element *)raw_data->current;
    if (!res->Validate()) {
        return false;
    }
    if (raw_data->current + sizeof(ElementHeader) + res->GetLength() > raw_data->end) {
        return false;
    }
    raw_data->current += sizeof(ElementHeader) + res->GetLength();

    element = res;
    is_present = true;
    return true;
}

const ReadableImageIdentifier::Element *const ReadableImageIdentifier::Get() const {
    return element;
}

void ReadableImageIdentifier::Log() const {
    ASSERT(element != nullptr);

    log_i("ME ImageIdentifierHeader VendorId:%u, Value:%.*s",
          element->vendor_id.Get(),
          (int)(element->GetLength() - (sizeof(ImageIdentifierHeader) - sizeof(ElementHeader))),
          (char *)element->data);
}

ElementHeader::ElementType ReadableImageIdentifier::GetElementType() const {
    return ElementHeader::ImageIdentifier;
}

bool ReadableImageIdentifier::IsPresent() const {
    return is_present;
}

uint32_t ReadableImageIdentifier::GetVendorIdentifier() const {
    ASSERT(element != nullptr);
    return element->vendor_id.Get();
}

const std::string_view ReadableImageIdentifier::GetData() const {
    ASSERT(element != nullptr);
    return { element->data,
             element->GetLength() - (sizeof(ImageIdentifierHeader) - sizeof(ElementHeader)) };
}
