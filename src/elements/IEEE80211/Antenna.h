#pragma once

#include "Helpers.h"
#include "IElement.h"
#include "elements/ElementHeader.h"
#include "span.hpp"
#include <array>
#include <cstdint>
#include <vector>

struct __attribute__((packed)) Antenna : ElementHeader {
  public:
    static const size_t max_antenna_count = 255;

    enum Diversity : uint8_t { Disabled = 0, Enabled = 1 };

    enum Combiner : uint8_t { SectorizedLeft = 1, SectorizedRight = 2, Omni = 3, MIMO = 4 };

    enum AntennaSelection : uint8_t { Internal = 1, External = 2 };

  protected:
    // Radio ID: An 8-bit value representing the radio, whose value is between 0 and 31.
    uint8_t radio_id;

    // Diversity: An 8-bit value specifying whether the antenna is to provide receiver diversity.
    Diversity diversity;

    // Combiner: An 8-bit value specifying the combiner selection.
    Combiner combiner;

    // Antenna Count: An 8-bit value specifying the number of Antenna Selection fields.
    uint8_t antenna_count;

  public:
    // Antenna Selection: One 8-bit antenna configuration value per antenna in the WTP.
    AntennaSelection antenna_selection[];

    Antenna(const Antenna &) = default;
    Antenna(uint8_t radio_id, Diversity diversity, Combiner combiner, uint8_t antenna_count);

    uint8_t GetRadioID() const;
    Diversity GetDiversity() const;
    Combiner GetCombiner() const;
    uint8_t GetAntennaCount() const;
    bool Validate() const;
    void Serialize(RawData *raw_data) const;
    static Antenna *Deserialize(RawData *raw_data);
};

struct WritableAntennaArray : IWritableElement {
  public:
    struct Item {
        nonstd::span<const Antenna::AntennaSelection> selections;
        Antenna header;
        Item(const Item &) = default;
        Item(uint8_t radio_id,
             Antenna::Diversity diversity,
             Antenna::Combiner combiner,
             nonstd::span<const Antenna::AntennaSelection> sel)
            : selections{ sel },
              header{ radio_id, diversity, combiner, (uint8_t)selections.size() } {};
    };

  private:
    std::vector<Item> items;

  public:
    WritableAntennaArray(const WritableAntennaArray &) = delete;
    WritableAntennaArray();

    void Add(uint8_t radio_id,
             Antenna::Diversity diversity,
             Antenna::Combiner combiner,
             nonstd::span<const Antenna::AntennaSelection> selections);
    bool Empty() const;
    void Clear();

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableAntennaArray : IReadableElement {
  public:
    static const size_t max_count = 32;

  protected:
    std::array<const Antenna *, max_count> items;
    size_t count;

  public:
    ReadableAntennaArray(const ReadableAntennaArray &) = delete;
    ReadableAntennaArray();

    bool Deserialize(RawData *raw_data) override final;
    nonstd::span<const Antenna *const> Get() const;
    void Log() const override final;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};
