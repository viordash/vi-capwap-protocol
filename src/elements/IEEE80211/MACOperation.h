#pragma once

#include "Helpers.h"
#include "IElement.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <vector>

struct __attribute__((packed)) MACOperation : ElementHeader {
  protected:
    // Radio ID: An 8-bit value representing the radio, whose value is between 0 and 31.
    uint8_t radio_id;

    // Reserved: Must be set to zero.
    uint8_t reserved;

    // RTS Threshold: Number of octets in MPDU below which RTS/CTS handshake is not performed.
    NetworkU16 rts_threshold;

    // Short Retry: Maximum transmission attempts for frames <= RTSThreshold.
    uint8_t short_retry;

    // Long Retry: Maximum transmission attempts for frames > RTSThreshold.
    uint8_t long_retry;

    // Fragmentation Threshold: Maximum size in octets of MPDU delivered to PHY.
    NetworkU16 fragmentation_threshold;

    // Tx MSDU Lifetime: Elapsed time in TUs after which MSDU transmission attempts are terminated.
    NetworkU32 tx_msdu_lifetime;

    // Rx MSDU Lifetime: Elapsed time in TUs after which MSDU reassembly attempts are terminated.
    NetworkU32 rx_msdu_lifetime;

  public:
    MACOperation(const MACOperation &) = default;
    MACOperation(uint8_t radio_id,
                 uint16_t rts_threshold,
                 uint8_t short_retry,
                 uint8_t long_retry,
                 uint16_t fragmentation_threshold,
                 uint32_t tx_msdu_lifetime,
                 uint32_t rx_msdu_lifetime);

    uint8_t GetRadioID() const;
    uint16_t GetRTSThreshold() const;
    uint8_t GetShortRetry() const;
    uint8_t GetLongRetry() const;
    uint16_t GetFragmentationThreshold() const;
    uint32_t GetTxMSDULifetime() const;
    uint32_t GetRxMSDULifetime() const;

    bool Validate() const;
};

struct WritableMACOperationArray : IWritableConfigurationStatusRequestOptionalElement,
                                   IWritableConfigurationStatusResponseOptionalElement,
                                   IWritableConfigurationUpdateRequestOptionalElement {
  private:
    std::vector<MACOperation> items;

  public:
    WritableMACOperationArray(const WritableMACOperationArray &) = delete;
    WritableMACOperationArray();

    void Add(MACOperation op);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const override;
    void Log() const override;
};

struct ReadableMACOperationArray : IReadableConfigurationStatusRequestOptionalElement,
                                   IReadableConfigurationStatusResponseOptionalElement,
                                   IReadableConfigurationUpdateRequestOptionalElement {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const MACOperation *, max_count> items;
    size_t count;

  public:
    ReadableMACOperationArray(const ReadableMACOperationArray &) = delete;
    ReadableMACOperationArray();

    bool Deserialize(RawData *raw_data) override;
    ElementHeader::ElementType GetElementType() const override;
    bool IsPresent() const override;
    nonstd::span<const MACOperation *const> Get() const;
    void Log() const override;
};
