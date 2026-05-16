#include "WTPRebootStatistics.h"
#include "Logging.h"
#include "lassert.h"
#include <cstring>

WTPRebootStatistics::WTPRebootStatistics(uint16_t reboot_count,
                                         uint16_t ac_initiated_count,
                                         uint16_t link_failure_count,
                                         uint16_t sw_failure_count,
                                         uint16_t hw_failure_count,
                                         uint16_t other_failure_count,
                                         uint16_t unknown_failure_count,
                                         LastFailureType last_failure_type)
    : ElementHeader(ElementHeader::WTPRebootStatistics,
                    sizeof(WTPRebootStatistics) - sizeof(ElementHeader)),
      reboot_count{ reboot_count }, ac_initiated_count{ ac_initiated_count },
      link_failure_count{ link_failure_count }, sw_failure_count{ sw_failure_count },
      hw_failure_count{ hw_failure_count }, other_failure_count{ other_failure_count },
      unknown_failure_count{ unknown_failure_count }, last_failure_type{ last_failure_type } {
}
bool WTPRebootStatistics::Validate() const {
    static_assert(sizeof(WTPRebootStatistics) == 19);
    if (GetElementType() != ElementHeader::WTPRebootStatistics) {
        return false;
    }
    if (GetLength() != (sizeof(WTPRebootStatistics) - sizeof(ElementHeader))) {
        return false;
    }

    switch (last_failure_type) {
        case LastFailureType::NotSupported:
        case LastFailureType::ACInitiated:
        case LastFailureType::LinkFailure:
        case LastFailureType::SoftwareFailure:
        case LastFailureType::HardwareFailure:
        case LastFailureType::OtherFailure:
        case LastFailureType::Unknown:
            return true;
        default:
            return false;
    };
}

uint16_t WTPRebootStatistics::GetRebootCount() const {
    return reboot_count.Get();
}

uint16_t WTPRebootStatistics::GetACInitiatedCount() const {
    return ac_initiated_count.Get();
}

uint16_t WTPRebootStatistics::GetLinkFailureCount() const {
    return link_failure_count.Get();
}

uint16_t WTPRebootStatistics::GetSWFailureCount() const {
    return sw_failure_count.Get();
}

uint16_t WTPRebootStatistics::GetHWFailureCount() const {
    return hw_failure_count.Get();
}

uint16_t WTPRebootStatistics::GetOtherFailureCount() const {
    return other_failure_count.Get();
}

uint16_t WTPRebootStatistics::GetUnknownFailureCount() const {
    return unknown_failure_count.Get();
}

WTPRebootStatistics::LastFailureType WTPRebootStatistics::GetLastFailureType() const {
    return last_failure_type;
}

void WTPRebootStatistics::Log() const {
    log_i("ME WTPRebootStatistics Reboot Count:{}, AC Initiated Count:{}, Link Failure Count:{}, "
          "SW Failure Count:{}, HW Failure Count:{}, Other Failure Count:{}, Unknown Failure "
          "Count:{}, Last Failure Type:{}",
          GetRebootCount(),
          GetACInitiatedCount(),
          GetLinkFailureCount(),
          GetSWFailureCount(),
          GetHWFailureCount(),
          GetOtherFailureCount(),
          GetUnknownFailureCount(),
          (unsigned)GetLastFailureType());
}

WritableWTPRebootStatistics::WritableWTPRebootStatistics(
    uint16_t reboot_count,
    uint16_t ac_initiated_count,
    uint16_t link_failure_count,
    uint16_t sw_failure_count,
    uint16_t hw_failure_count,
    uint16_t other_failure_count,
    uint16_t unknown_failure_count,
    WTPRebootStatistics::LastFailureType last_failure_type)
    : element{ reboot_count,     ac_initiated_count,  link_failure_count,    sw_failure_count,
               hw_failure_count, other_failure_count, unknown_failure_count, last_failure_type } {
}

void WritableWTPRebootStatistics::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(WTPRebootStatistics) <= raw_data->end);
    std::memcpy(raw_data->current, &element, sizeof(element));
    raw_data->current += sizeof(element);
}

void WritableWTPRebootStatistics::Log() const {
    element.Log();
}

bool ReadableWTPRebootStatistics::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(WTPRebootStatistics) > raw_data->end) {
        return false;
    }

    auto res = (WTPRebootStatistics *)raw_data->current;
    if (!res->Validate()) {
        return false;
    }
    raw_data->current += sizeof(WTPRebootStatistics);

    element = res;
    is_present = true;
    return true;
}

const WTPRebootStatistics *ReadableWTPRebootStatistics::Get() const {
    return element;
}

void ReadableWTPRebootStatistics::Log() const {
    ASSERT(element != nullptr);
    element->Log();
}

ElementHeader::ElementType ReadableWTPRebootStatistics::GetElementType() const {
    return ElementHeader::WTPRebootStatistics;
}

bool ReadableWTPRebootStatistics::IsPresent() const {
    return is_present;
}