#pragma once

#include "Helpers.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <vector>

// RFC 5416 Section 6.16 - IEEE 802.11 Statistics
//
//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |    Radio ID   |                   Reserved                    |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                       Tx Fragment Count                       |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                       Multicast Tx Count                      |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                          Failed Count                         |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                          Retry Count                          |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                      Multiple Retry Count                     |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                     Frame Duplicate Count                     |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                       RTS Success Count                       |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                       RTS Failure Count                       |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                       ACK Failure Count                       |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                       Rx Fragment Count                       |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                       Multicast RX Count                      |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                        FCS Error  Count                       |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                        Tx Frame Count                         |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                       Decryption Errors                       |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                  Discarded QoS Fragment Count                 |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                    Associated Station Count                   |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                  QoS CF Polls Received Count                  |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                   QoS CF Polls Unused Count                   |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                  QoS CF Polls Unusable Count                  |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Type:   1039 for IEEE 802.11 Statistics
// Length: 80

struct __attribute__((packed)) Statistics : ElementHeader {
  protected:
    // Radio ID: An 8-bit value representing the radio, whose value is between 0 and 31.
    uint8_t radio_id;

    // Reserved: Must be set to zero.
    uint8_t reserved[3];

    // Tx Fragment Count: Number of fragmented frames transmitted.
    NetworkU32 tx_fragment_count;

    // Multicast Tx Count: Number of multicast frames transmitted.
    NetworkU32 multicast_tx_count;

    // Failed Count: Transmit excessive retries.
    NetworkU32 failed_count;

    // Retry Count: Number of transmit retries.
    NetworkU32 retry_count;

    // Multiple Retry Count: Number of transmits that required more than one retry.
    NetworkU32 multiple_retry_count;

    // Frame Duplicate Count: Duplicate frames received.
    NetworkU32 frame_duplicate_count;

    // RTS Success Count: Number of successfully transmitted RTS.
    NetworkU32 rts_success_count;

    // RTS Failure Count: Failed transmitted RTS.
    NetworkU32 rts_failure_count;

    // ACK Failure Count: Number of failed acknowledgements.
    NetworkU32 ack_failure_count;

    // Rx Fragment Count: Number of fragmented frames received.
    NetworkU32 rx_fragment_count;

    // Multicast RX Count: Number of multicast frames received.
    NetworkU32 multicast_rx_count;

    // FCS Error Count: Number of FCS failures.
    NetworkU32 fcs_error_count;

    // Tx Frame Count: Number of transmitted frames.
    NetworkU32 tx_frame_count;

    // Decryption Errors: Number of decryption errors.
    NetworkU32 decryption_errors;

    // Discarded QoS Fragment Count: Number of discarded QoS fragments received.
    NetworkU32 discarded_qos_fragment_count;

    // Associated Station Count: Number of associated stations.
    NetworkU32 associated_station_count;

    // QoS CF Polls Received Count: Number of (+)CF-Polls received.
    NetworkU32 qos_cf_polls_received_count;

    // QoS CF Polls Unused Count: Number of (+)CF-Polls received but not used.
    NetworkU32 qos_cf_polls_unused_count;

    // QoS CF Polls Unusable Count: Number of (+)CF-Polls received but unusable.
    NetworkU32 qos_cf_polls_unusable_count;

  public:
    Statistics(const Statistics &) = default;
    Statistics(uint8_t radio_id,
               uint32_t tx_fragment_count,
               uint32_t multicast_tx_count,
               uint32_t failed_count,
               uint32_t retry_count,
               uint32_t multiple_retry_count,
               uint32_t frame_duplicate_count,
               uint32_t rts_success_count,
               uint32_t rts_failure_count,
               uint32_t ack_failure_count,
               uint32_t rx_fragment_count,
               uint32_t multicast_rx_count,
               uint32_t fcs_error_count,
               uint32_t tx_frame_count,
               uint32_t decryption_errors,
               uint32_t discarded_qos_fragment_count,
               uint32_t associated_station_count,
               uint32_t qos_cf_polls_received_count,
               uint32_t qos_cf_polls_unused_count,
               uint32_t qos_cf_polls_unusable_count);

    uint8_t GetRadioID() const;
    uint32_t GetTxFragmentCount() const;
    uint32_t GetMulticastTxCount() const;
    uint32_t GetFailedCount() const;
    uint32_t GetRetryCount() const;
    uint32_t GetMultipleRetryCount() const;
    uint32_t GetFrameDuplicateCount() const;
    uint32_t GetRTSSuccessCount() const;
    uint32_t GetRTSFailureCount() const;
    uint32_t GetACKFailureCount() const;
    uint32_t GetRxFragmentCount() const;
    uint32_t GetMulticastRxCount() const;
    uint32_t GetFCSErrorCount() const;
    uint32_t GetTxFrameCount() const;
    uint32_t GetDecryptionErrors() const;
    uint32_t GetDiscardedQoSFragmentCount() const;
    uint32_t GetAssociatedStationCount() const;
    uint32_t GetQoSCFPollsReceivedCount() const;
    uint32_t GetQoSCFPollsUnusedCount() const;
    uint32_t GetQoSCFPollsUnusableCount() const;

    bool Validate() const;
};

struct WritableStatisticsArray {
  private:
    std::vector<Statistics> items;

  public:
    WritableStatisticsArray(const WritableStatisticsArray &) = delete;
    WritableStatisticsArray();

    void Add(Statistics element);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const;
    void Log() const;
};

struct ReadableStatisticsArray {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const Statistics *, max_count> items;
    size_t count;

  public:
    ReadableStatisticsArray(const ReadableStatisticsArray &) = delete;
    ReadableStatisticsArray();

    bool Deserialize(RawData *raw_data);
    nonstd::span<const Statistics *const> Get() const;
    void Log() const;
};
