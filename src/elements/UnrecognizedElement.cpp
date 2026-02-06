#include "UnrecognizedElement.h"
#include "lassert.h"
#include "logging.h"

UnrecognizedElement *UnrecognizedElement::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(UnrecognizedElement) > raw_data->end) {
        return nullptr;
    }
    auto res = (UnrecognizedElement *)raw_data->current;
    if (raw_data->current + sizeof(UnrecognizedElement) + res->GetLength() > raw_data->end) {
        return nullptr;
    }
    raw_data->current += sizeof(UnrecognizedElement) + res->GetLength();
    return res;
}
