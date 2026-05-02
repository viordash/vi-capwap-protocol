#pragma once

#include "IElement.h"
#include "elements/ElementHeader.h"
#include <cstdint>

struct __attribute__((packed)) WTPRebootStatistics : ElementHeader {
  public:
    enum LastFailureType : uint8_t {
        NotSupported = 0,
        ACInitiated = 1,
        LinkFailure = 2,
        SoftwareFailure = 3,
        HardwareFailure = 4,
        OtherFailure = 5,
        Unknown = 255
    };
    const static uint16_t NoInfo = 65535;

  private:
    // Reboot Count:   The number of reboots that have occurred due to a WTP crash.
    //   A value of 65535 implies that this information is not available on the WTP.
    NetworkU16 reboot_count;

    // AC Initiated Count:   The number of reboots that have occurred at the
    //   request of a CAPWAP protocol message, such as a change in
    //   configuration that required a reboot or an explicit CAPWAP
    //   protocol reset request.  A value of 65535 implies that this
    //   information is not available on the WTP.
    NetworkU16 ac_initiated_count;

    // Link Failure Count:   The number of times that a CAPWAP protocol
    //     connection with an AC has failed due to link failure.
    NetworkU16 link_failure_count;

    // SW Failure Count:   The number of times that a CAPWAP protocol
    //   connection with an AC has failed due to software-related reasons.
    NetworkU16 sw_failure_count;

    // HW Failure Count:   The number of times that a CAPWAP protocol
    //   connection with an AC has failed due to hardware-related reasons.
    NetworkU16 hw_failure_count;

    // Other Failure Count:   The number of times that a CAPWAP protocol
    //   connection with an AC has failed due to known reasons, other than
    //   AC initiated, link, SW or HW failure.
    NetworkU16 other_failure_count;

    // Unknown Failure Count:   The number of times that a CAPWAP protocol
    //   connection with an AC has failed for unknown reasons.
    NetworkU16 unknown_failure_count;

    // Last Failure Type:   The failure type of the most recent WTP failure.
    //   The following enumerated values are supported:
    LastFailureType last_failure_type;

  public:
    WTPRebootStatistics(const WTPRebootStatistics &) = default;
    WTPRebootStatistics(uint16_t reboot_count,
                        uint16_t ac_initiated_count,
                        uint16_t link_failure_count,
                        uint16_t sw_failure_count,
                        uint16_t hw_failure_count,
                        uint16_t other_failure_count,
                        uint16_t unknown_failure_count,
                        LastFailureType last_failure_type);

    bool Validate() const;

    uint16_t GetRebootCount() const;
    uint16_t GetACInitiatedCount() const;
    uint16_t GetLinkFailureCount() const;
    uint16_t GetSWFailureCount() const;
    uint16_t GetHWFailureCount() const;
    uint16_t GetOtherFailureCount() const;
    uint16_t GetUnknownFailureCount() const;
    LastFailureType GetLastFailureType() const;
    void Log() const;
};

struct WritableWTPRebootStatistics : IWritableConfigurationStatusRequestOptionalElement,
                                     IWritableJoinRequestOptionalElement {
  protected:
    WTPRebootStatistics element;

  public:
    WritableWTPRebootStatistics(uint16_t reboot_count,
                                uint16_t ac_initiated_count,
                                uint16_t link_failure_count,
                                uint16_t sw_failure_count,
                                uint16_t hw_failure_count,
                                uint16_t other_failure_count,
                                uint16_t unknown_failure_count,
                                WTPRebootStatistics::LastFailureType last_failure_type);

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableWTPRebootStatistics : IReadableConfigurationStatusRequestOptionalElement,
                                     IReadableJoinRequestOptionalElement {
  protected:
    WTPRebootStatistics *element = nullptr;
    bool is_present = false;

  public:
    bool Deserialize(RawData *raw_data) override final;
    void Log() const override final;
    const WTPRebootStatistics *const Get() const;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};