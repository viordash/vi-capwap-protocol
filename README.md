# vi-capwap-protocol

Header-only C++17 library for CAPWAP protocol (Control and Provisioning of Wireless Access Points) packet definitions according to RFC 5415 and RFC 5416.

This repository is designed to be included as a git submodule in other projects.

## Structure

```
src/
├── *.h, *.cpp          # CAPWAP message types (DiscoveryRequest, JoinRequest, etc.)
└── elements/           # CAPWAP message elements (ACDescriptor, WTPDescriptor, etc.)
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
