#pragma once
#include "ClearHeader.h"
#include "ControlHeader.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>

struct __attribute__((packed)) WTPRadioInformation : ElementHeader {
    //The Radio Identifier, whose value is between one (1) and 31, which typically refers to an interface index on the WTP
    uint8_t RadioID;
    uint8_t Reservd_0;
    uint8_t Reservd_1;
    uint8_t Reservd_2;

    uint8_t B : 1;  // Bit 0: 802.11b (Wi-Fi 1) - 2.4 GHz
    uint8_t A : 1;  // Bit 1: 802.11a (Wi-Fi 2) - 5 GHz
    uint8_t G : 1;  // Bit 2: 802.11g (Wi-Fi 3) - 2.4 GHz
    uint8_t N : 1;  // Bit 3: 802.11n (Wi-Fi 4) - 2.4 / 5 GHz
    uint8_t AC : 1; // Bit 4: 802.11ac (Wi-Fi 5) - 5 GHz
    uint8_t AX : 1; // Bit 5: 802.11ax (Wi-Fi 6/6E) - 2.4 / 5 / 6 GHz
    uint8_t BE : 1; // Bit 6: 802.11be (Wi-Fi 7) - 2.4 / 5 / 6 GHz
    uint8_t Reservd_3 : 1;

    WTPRadioInformation(const WTPRadioInformation &) = delete;
    WTPRadioInformation(uint8_t radio_id,
                        bool b,
                        bool a,
                        bool g,
                        bool n,
                        bool ac,
                        bool ax,
                        bool be);

    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static WTPRadioInformation *Deserialize(RawData *raw_data);
    uint16_t GetTotalLength() const;
};

struct WritableWTPRadioInformationArray {
  private:
    const nonstd::span<const WTPRadioInformation> items;

  public:
    WritableWTPRadioInformationArray(const WritableWTPRadioInformationArray &) = delete;
    WritableWTPRadioInformationArray(const nonstd::span<const WTPRadioInformation> &items);

    void Serialize(RawData *raw_data) const;
    uint16_t GetTotalLength() const;
    void Log() const;
};

struct ReadableWTPRadioInformationArray {
  public:
    static const size_t max_count = 32; //Radio ID

  protected:
    std::array<const WTPRadioInformation *, max_count> items;
    size_t count;

  public:
    ReadableWTPRadioInformationArray(const ReadableWTPRadioInformationArray &) = delete;
    ReadableWTPRadioInformationArray();

    bool Deserialize(RawData *raw_data);
    nonstd::span<const WTPRadioInformation *const> Get() const;
    void Log() const;
};
