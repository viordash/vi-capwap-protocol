#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <string_view>

struct __attribute__((packed)) ACDescriptorHeader : ElementHeader {
    enum RMACField : uint8_t { Reserved = 0, Supported = 1, NotSupported = 2 };

  private:
    // Stations:   The number of stations currently served by the AC
    NetworkU16 stations;

    // Limit:   The maximum number of stations supported by the AC
    NetworkU16 limit;

    // Active WTPs:   The number of WTPs currently attached to the AC
    NetworkU16 active_wtp;

    // Max WTPs:   The maximum number of WTPs supported by the AC
    NetworkU16 max_wtp;

    // Security:   An 8-bit mask specifying the authentication credential type supported by the AC
    uint8_t r0 : 1;               // A reserved bit for future use
    uint8_t x509_cert : 1;        // The AC supports X.509 Certificate authentication
    uint8_t preshared_secret : 1; // The AC supports the pre-shared secret authentication
    uint8_t reserved : 5;         // reserved bits for future flags

    // The AC supports the optional Radio MAC Address field in the CAPWAP transport header
    RMACField rmac;

    uint8_t reserved1;

    // DTLS Policy:   The AC communicates its policy on the use of DTLS for the CAPWAP data channel
    uint8_t r1 : 1;             // A reserved bit for future use
    uint8_t clear_data_chn : 1; // Clear Text Data Channel Supported
    uint8_t dtls_data_chn : 1;  // DTLS-Enabled Data Channel Supported
    uint8_t reserved2 : 5;      // reserved bits for future flags

  public:
    ACDescriptorHeader(const ACDescriptorHeader &) = delete;
    ACDescriptorHeader(uint16_t stations,
                       uint16_t limit,
                       uint16_t active_wtp,
                       uint16_t max_wtp,
                       bool x509_cert,
                       bool preshared_secret,
                       RMACField rmac,
                       bool clear_data_chn,
                       bool dtls_data_chn,
                       uint16_t sub_elements_size);

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static ACDescriptorHeader *Deserialize(RawData *raw_data);
    uint16_t GetStations() const;
    uint16_t GetLimit() const;
    uint16_t GetActiveWTPs() const;
    uint16_t GetMaxWTPs() const;
    bool X509Cert() const;
    bool PresharedSecret() const;
    RMACField GetRMAC() const;
    bool ClearDataChannel() const;
    bool DTLSDataChannel() const;
};

struct __attribute__((packed)) ACInformationSubElementHeader {
  public:
    enum Type : uint16_t {
        HardwareVersion = 0x0400,
        SoftwareVersion = 0x0500,
    };

  private:
    // SMI Network Management Private Enterprise Codes. MUST NOT be set to zero
    NetworkU32 vendor_id;

    Type type;
    // MUST NOT exceed 1024 octets
    NetworkU16 length;

  public:
    uint8_t data[];

    ACInformationSubElementHeader(const ACInformationSubElementHeader &) = delete;
    ACInformationSubElementHeader(uint32_t vendor_identifier, Type type, uint16_t length);

    uint32_t GetVendorIdentifier() const;
    Type GetType() const;
    uint16_t GetLength() const;
    bool Validate() const;
};

struct WritableACDescriptor {
  public:
    struct __attribute__((packed)) SubElement {
        const char *data;
        ACInformationSubElementHeader header;
        SubElement(const SubElement &) = delete;
        SubElement(uint32_t vendor_identifier,
                   ACInformationSubElementHeader::Type type,
                   std::string_view data)
            : data{ data.data() }, header{ vendor_identifier, type, (uint16_t)data.size() } {};
    };

  private:
    ACDescriptorHeader header;
    const nonstd::span<const SubElement> sub_elements;

    static uint16_t GetSubElementsSize(const nonstd::span<const SubElement> &elements);

  public:
    WritableACDescriptor(const WritableACDescriptor &) = delete;
    WritableACDescriptor(uint16_t stations,
                         uint16_t limit,
                         uint16_t active_wtp,
                         uint16_t max_wtp,
                         bool x509_cert,
                         bool preshared_secret,
                         ACDescriptorHeader::RMACField rmac,
                         bool clear_data_chn,
                         bool dtls_data_chn,
                         const nonstd::span<const SubElement> &elements);

    void Serialize(RawData *raw_data) const;
    uint16_t GetTotalLength() const;
    void Log() const;
};

struct ReadableACDescriptor {
  public:
    static const size_t max_count = 8; //DescriptorSubElementHeader::Type * 2
    ACDescriptorHeader *header;

  private:
    std::array<const ACInformationSubElementHeader *, max_count> items;
    size_t count;

  public:
    ReadableACDescriptor(const ReadableACDescriptor &) = delete;
    ReadableACDescriptor();

    bool Deserialize(RawData *raw_data);
    nonstd::span<const ACInformationSubElementHeader *const> Get() const;
    void Log() const;
};
