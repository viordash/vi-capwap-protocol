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

    // An IEEE 802.11b radio.
    uint8_t B : 1;
    // An IEEE 802.11a radio.
    uint8_t A : 1;
    // An IEEE 802.11g radio.
    uint8_t G : 1;
    // An IEEE 802.11n radio.
    uint8_t N : 1;

    uint8_t Reservd_3 : 4;

    WTPRadioInformation(const WTPRadioInformation &) = delete;
    WTPRadioInformation(uint8_t radio_id, bool b, bool a, bool g, bool n);

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
