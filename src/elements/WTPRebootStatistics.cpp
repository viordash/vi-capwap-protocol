#include "WTPRebootStatistics.h"
#include "lassert.h"
#include "logging.h"
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
void WTPRebootStatistics::Serialize(RawData *raw_data) const {
    ASSERT(raw_data->current + sizeof(WTPRebootStatistics) <= raw_data->end);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
    memcpy(raw_data->current, this, sizeof(WTPRebootStatistics));
#pragma GCC diagnostic pop
    raw_data->current += sizeof(WTPRebootStatistics);
}
WTPRebootStatistics *WTPRebootStatistics::Deserialize(RawData *raw_data) {
    if (raw_data->current + sizeof(WTPRebootStatistics) > raw_data->end) {
        return nullptr;
    }

    auto res = (WTPRebootStatistics *)raw_data->current;
    if (!res->Validate()) {
        return nullptr;
    }
    raw_data->current += sizeof(WTPRebootStatistics);
    return res;
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

uint16_t WTPRebootStatistics::GetTotalLength() const {
    return GetLength() + sizeof(ElementHeader);
}

void WTPRebootStatistics::Log() const {
    log_i("ME WTPRebootStatistics Reboot Count:%u, AC Initiated Count:%u, Link Failure Count:%u, "
          "SW Failure Count:%u, HW Failure Count:%u, Other Failure Count:%u, Unknown Failure "
          "Count:%u, Last Failure Type:%u",
          GetRebootCount(),
          GetACInitiatedCount(),
          GetLinkFailureCount(),
          GetSWFailureCount(),
          GetHWFailureCount(),
          GetOtherFailureCount(),
          GetUnknownFailureCount(),
          (unsigned)GetLastFailureType());
}