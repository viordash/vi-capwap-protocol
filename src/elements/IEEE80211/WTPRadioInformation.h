#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "IElement.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <vector>

struct __attribute__((packed)) WTPRadioInformation : ElementHeader {
    //The Radio Identifier, whose value is between one (1) and 31, which typically refers to an interface index on the WTP
    uint8_t RadioID;
    uint8_t Reservd_0;
    uint8_t Reservd_1;
    uint8_t Reservd_2;

#if VI_CAPWAP_BIG_ENDIAN
    uint8_t Reservd_3 : 1;
    uint8_t BE : 1; // Bit 6: 802.11be (Wi-Fi 7) - 2.4 / 5 / 6 GHz
    uint8_t AX : 1; // Bit 5: 802.11ax (Wi-Fi 6/6E) - 2.4 / 5 / 6 GHz
    uint8_t AC : 1; // Bit 4: 802.11ac (Wi-Fi 5) - 5 GHz
    uint8_t N : 1;  // Bit 3: 802.11n (Wi-Fi 4) - 2.4 / 5 GHz
    uint8_t G : 1;  // Bit 2: 802.11g (Wi-Fi 3) - 2.4 GHz
    uint8_t A : 1;  // Bit 1: 802.11a (Wi-Fi 2) - 5 GHz
    uint8_t B : 1;  // Bit 0: 802.11b (Wi-Fi 1) - 2.4 GHz
#else
    uint8_t B : 1;  // Bit 0: 802.11b (Wi-Fi 1) - 2.4 GHz
    uint8_t A : 1;  // Bit 1: 802.11a (Wi-Fi 2) - 5 GHz
    uint8_t G : 1;  // Bit 2: 802.11g (Wi-Fi 3) - 2.4 GHz
    uint8_t N : 1;  // Bit 3: 802.11n (Wi-Fi 4) - 2.4 / 5 GHz
    uint8_t AC : 1; // Bit 4: 802.11ac (Wi-Fi 5) - 5 GHz
    uint8_t AX : 1; // Bit 5: 802.11ax (Wi-Fi 6/6E) - 2.4 / 5 / 6 GHz
    uint8_t BE : 1; // Bit 6: 802.11be (Wi-Fi 7) - 2.4 / 5 / 6 GHz
    uint8_t Reservd_3 : 1;
#endif

    WTPRadioInformation(const WTPRadioInformation &) = default;
    WTPRadioInformation(uint8_t radio_id,
                        bool b,
                        bool a,
                        bool g,
                        bool n,
                        bool ac,
                        bool ax,
                        bool be);

    bool Validate() const;
    uint16_t GetTotalLength() const;
    std::string ToString() const;
};

struct WritableWTPRadioInformationArray : IWritableConfigurationStatusRequestOptionalElement {
  private:
    std::vector<WTPRadioInformation> items;

  public:
    WritableWTPRadioInformationArray(const WritableWTPRadioInformationArray &) = delete;
    WritableWTPRadioInformationArray();

    void Add(WTPRadioInformation radio_info);
    bool Empty() const;
    void Clear();
    size_t Size();

    void Serialize(RawData *raw_data) const override;
    uint16_t GetTotalLength() const;
    void Log() const override;
};

struct ReadableWTPRadioInformationArray : IReadableConfigurationStatusRequestOptionalElement {
  public:
    static const size_t max_count = 32; //Radio ID

  protected:
    std::array<const WTPRadioInformation *, max_count> items;
    size_t count;

  public:
    ReadableWTPRadioInformationArray(const ReadableWTPRadioInformationArray &) = delete;
    ReadableWTPRadioInformationArray();

    bool Deserialize(RawData *raw_data) override;
    ElementHeader::ElementType GetElementType() const override;
    bool IsPresent() const override;
    nonstd::span<const WTPRadioInformation *const> Get() const;
    void Log() const override;
};
