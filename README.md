# vi-capwap-protocol

C++17 library for CAPWAP protocol (Control and Provisioning of Wireless Access Points) packet definitions according to RFC 5415 and RFC 5416.

This repository is designed to be included as a git submodule in other projects.

## Structure

```
src/
├── *.h, *.cpp          # CAPWAP message types (DiscoveryRequest, JoinRequest, etc.)
└── elements/           # CAPWAP message elements
    ├── *.h, *.cpp      # Generic elements (ACDescriptor, WTPDescriptor, etc.)
    └── IEEE80211/      # IEEE 802.11 binding elements (RFC 5416)
tests/
├── unit_tests/         # Unit tests
└── benchmarks/         # Performance benchmarks
docs/
├── rfc5415.txt         # CAPWAP Protocol Specification
└── rfc5416.txt         # CAPWAP Binding for IEEE 802.11
```

## Usage as Submodule

```bash
git submodule add <repository-url> path/to/vi-capwap-protocol
git submodule update --init --recursive
```

Then include the necessary headers in your project:
```cpp
#include "vi-capwap-protocol/src/DiscoveryRequest.h"
#include "vi-capwap-protocol/src/elements/ACDescriptor.h"
```

## Data Lifetime Management

**Important**: This library is designed for zero-copy operation. CAPWAP messages and elements do **not** take ownership of the data passed to them.

When passing data via `nonstd::span<const uint8_t>` to writable arrays (e.g., `WritableRateSetArray`, `WritableStationArray`), the caller must ensure that the referenced data remains valid until `Serialize()` is called.

### Example - Correct Usage

```cpp
std::vector<uint8_t> rate_set_data = { 0x82, 0x84, 0x8B, 0x96 };

WritableRateSetArray rate_sets;
rate_sets.Add(1, rate_set_data);  // span references rate_set_data
rate_sets.Serialize(&raw_data);   // rate_set_data must still be valid here
```

### Example - Incorrect Usage

```cpp
WritableRateSetArray rate_sets;
{
    std::vector<uint8_t> rate_set_data = { 0x82, 0x84, 0x8B, 0x96 };
    rate_sets.Add(1, rate_set_data);
}  // rate_set_data destroyed - span now points to invalid memory!
rate_sets.Serialize(&raw_data);  // Undefined behavior
```

## Building Tests

```bash
# Build and run unit tests
make build_tests
./output/tests/unit_tests/vi-capwap-tests

# Build and run benchmarks
make build_benchmarks
./output/tests/benchmarks/vi-capwap-benchmarks
```

## Code Quality

```bash
# Check code formatting
make check_format

# Apply code formatting
make apply_format

# Static analysis
make code_check
```

## RFC Compliance

- [RFC 5415](docs/rfc5415.txt) - CAPWAP Protocol Specification
- [RFC 5416](docs/rfc5416.txt) - CAPWAP Binding for IEEE 802.11

### Radio Information Requirements

When constructing CAPWAP messages with radio information, the following constraints must be satisfied:

1. **Maximum radios constraint**: `max_radios >= radios_in_use`
   - The maximum number of radios supported by WTP must be greater than or equal to the number of radios currently in use

2. **Radio info array size**: `radios_in_use <= wtp_radio_info.size()`
   - The `wtp_radio_info` array must contain at least `radios_in_use` elements

These requirements are enforced by runtime assertions in the library.
