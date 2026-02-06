#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <string_view>

struct __attribute__((packed)) WTPDescriptorHeader : ElementHeader {
    uint8_t MaxRadios;   // number of radios supported by WTP
    uint8_t RadiosInUse; // number of radios in use in the WTP.
    uint8_t NumEncrypt;  // number of 3-byte Encryption sub-elements. (1-255)

    WTPDescriptorHeader(const WTPDescriptorHeader &) = delete;
    WTPDescriptorHeader(uint8_t max_radios,
                        uint8_t radios_in_use,
                        uint8_t num_encrypt,
                        uint16_t sub_elements_size);

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static WTPDescriptorHeader *Deserialize(RawData *raw_data);
};

struct __attribute__((packed)) EncryptionSubElement {
    uint8_t WBID : 5;                // type of wireless packet associated with the radio
    uint8_t Resvd : 3;               // reserved for future use
    uint16_t EncryptionCapabilities; // WTP to communicate its capabilities to the AC

  public:
    EncryptionSubElement(const EncryptionSubElement &) = delete;
    EncryptionSubElement(uint16_t encryption_capabilities);

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static EncryptionSubElement *Deserialize(RawData *raw_data);
};

struct __attribute__((packed)) DescriptorSubElementHeader {
  public:
    enum Type : uint16_t {
        HardwareVersion = 0x0000,       // MUST be present
        ActiveSoftwareVersion = 0x0100, // MUST be present
        BootVersion = 0x0200,           // MUST be present
        OtherSoftwareVersion = 0x0300,  // MAY be present
    };

  private:
    // SMI Network Management Private Enterprise Codes. MUST NOT be set to zero
    NetworkU32 vendor_id;

    Type type;
    // MUST NOT exceed 1024 octets
    NetworkU16 length;

  public:
    char utf8_value[];

    DescriptorSubElementHeader(const DescriptorSubElementHeader &) = delete;
    DescriptorSubElementHeader(uint32_t vendor_identifier, Type type, uint16_t length);

    uint32_t GetVendorIdentifier() const;
    Type GetType() const;
    uint16_t GetLength() const;
    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static DescriptorSubElementHeader *Deserialize(RawData *raw_data);
};

struct WritableWTPDescriptor {
  public:
    struct __attribute__((packed)) SubElement {
        const char *utf8_value;
        DescriptorSubElementHeader header;
        SubElement(const SubElement &) = delete;
        SubElement(uint32_t vendor_identifier,
                   DescriptorSubElementHeader::Type type,
                   std::string_view utf8_string)
            : utf8_value{ utf8_string.data() },
              header{ vendor_identifier, type, (uint16_t)utf8_string.size() } {};
    };

  private:
    WTPDescriptorHeader header;
    const nonstd::span<const EncryptionSubElement> encr_items;
    const nonstd::span<const SubElement> desc_items;

    static uint16_t GetSubElementsSize(const nonstd::span<const EncryptionSubElement> &encr_items,
                                       const nonstd::span<const SubElement> &desc_items);

  public:
    WritableWTPDescriptor(const WritableWTPDescriptor &) = delete;
    WritableWTPDescriptor(uint8_t max_radios,
                          uint8_t radios_in_use,
                          const nonstd::span<const EncryptionSubElement> &encr_items,
                          const nonstd::span<const SubElement> &desc_items);

    void Serialize(RawData *raw_data) const;
    uint16_t GetTotalLength() const;
    void Log() const;
};

struct ReadableWTPDescriptor {
  public:
    static const size_t encr_max_count = 4;
    static const size_t desc_max_count = 8; //DescriptorSubElementHeader::Type * 2
    WTPDescriptorHeader *header;

  protected:
    std::array<const EncryptionSubElement *, encr_max_count> encr_items;
    std::array<const DescriptorSubElementHeader *, desc_max_count> desc_items;
    size_t desc_count;

  public:
    ReadableWTPDescriptor(const ReadableWTPDescriptor &) = delete;
    ReadableWTPDescriptor();

    bool Deserialize(RawData *raw_data);
    const nonstd::span<const EncryptionSubElement *> GetEncryptions();
    const nonstd::span<const DescriptorSubElementHeader *> GetDescriptors();
    void Log() const;
};
