#include "ElementHeader.h"
#include "lassert.h"
#include "logging.h"

ElementHeader::ElementType ElementHeader::GetElementType() const {
    return element_type;
}
uint16_t ElementHeader::GetLength() const {
    return element_length.Get();
}

ElementHeader::ElementHeader(ElementType element_type, uint16_t element_length)
    : element_type{ element_type }, element_length{ element_length } {
}