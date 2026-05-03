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

## Element Classification

This section describes how CAPWAP protocol elements are organized by their structure and usage patterns.

### CAPWAP Core Elements (RFC 5415)

Elements are grouped based on their structural characteristics and how they appear in messages.

#### Group 1: Complex Elements with Sub-elements (Single Instance)
Single-occurrence elements containing multiple fields, where the last field is an array of nested sub-elements.

- 4.6.1. AC Descriptor ([src/elements/ACDescriptor.h](src/elements/ACDescriptor.h))
- 4.6.40. WTP Board Data ([src/elements/WTPBoardData.h](src/elements/WTPBoardData.h))
- 4.6.41. WTP Descriptor ([src/elements/WTPDescriptor.h](src/elements/WTPDescriptor.h))

#### Group 2: Simple Value Lists (Single Instance)
Single-occurrence elements consisting exclusively of an array of uniform fields.

- 4.6.2. AC IPv4 List ([src/elements/ACIPv4List.h](src/elements/ACIPv4List.h))

#### Group 3: Variable-Length Single Fields (Single Instance)
Single-occurrence elements whose body consists entirely of one dynamically-sized field.

- 4.6.4. AC Name ([src/elements/ACName.h](src/elements/ACName.h))
- 4.6.30. Location Data ([src/elements/LocationData.h](src/elements/LocationData.h))
- 4.6.32. MTU Discovery Padding ([src/elements/MTUDiscoveryPadding.h](src/elements/MTUDiscoveryPadding.h))
- 4.6.45. WTP Name ([src/elements/WTPName.h](src/elements/WTPName.h))

#### Group 4: Structures with Dynamic Tail (Array in Message)
Elements that can appear multiple times in a message. Consist of static fields and end with a variable-length field.

- 4.6.5. AC Name with Priority ([src/elements/ACNameWithPriority.h](src/elements/ACNameWithPriority.h))
- 4.6.17. Decryption Error Report ([src/elements/DecryptionErrorReport.h](src/elements/DecryptionErrorReport.h))
- 4.6.20. Delete Station ([src/elements/DeleteStation.h](src/elements/DeleteStation.h))
- 4.6.22. Duplicate IPv4 Address ([src/elements/DuplicateIPv4Address.h](src/elements/DuplicateIPv4Address.h))
- 4.6.36. Returned Message Element ([src/elements/ReturnedMessageElement.h](src/elements/ReturnedMessageElement.h))
- 4.6.39. Vendor Specific Payload ([src/elements/VendorSpecificPayload.h](src/elements/VendorSpecificPayload.h))

#### Group 5: Static Structures (Single Instance)
Single-occurrence elements with strictly fixed size.

- 4.6.6. AC Timestamp ([src/elements/ACTimestamp.h](src/elements/ACTimestamp.h))
- 4.6.13. CAPWAP Timers ([src/elements/CAPWAPTimers.h](src/elements/CAPWAPTimers.h))
- 4.6.14. CAPWAP Transport Protocol ([src/elements/CapwapTransportProtocol.h](src/elements/CapwapTransportProtocol.h))
- 4.6.21. Discovery Type ([src/elements/DiscoveryType.h](src/elements/DiscoveryType.h))
- 4.6.24. Idle Timeout ([src/elements/IdleTimeout.h](src/elements/IdleTimeout.h))
- 4.6.25. ECN Support ([src/elements/ECNSupport.h](src/elements/ECNSupport.h))
- 4.6.28. Image Information ([src/elements/ImageInformation.h](src/elements/ImageInformation.h))
- 4.6.29. Initiate Download ([src/elements/InitiateDownload.h](src/elements/InitiateDownload.h))
- 4.6.31. Maximum Message Length ([src/elements/MaximumMessageLength.h](src/elements/MaximumMessageLength.h))
- 4.6.35. Result Code ([src/elements/ResultCode.h](src/elements/ResultCode.h))
- 4.6.37. Session ID ([src/elements/SessionId.h](src/elements/SessionId.h))
- 4.6.38. Statistics Timer ([src/elements/StatisticsTimer.h](src/elements/StatisticsTimer.h))
- 4.6.42. WTP Fallback ([src/elements/WTPFallback.h](src/elements/WTPFallback.h))
- 4.6.43. WTP Frame Tunnel Mode ([src/elements/WTPFrameTunnelMode.h](src/elements/WTPFrameTunnelMode.h))
- 4.6.44. WTP MAC Type ([src/elements/WTPMACType.h](src/elements/WTPMACType.h))
- 4.6.47. WTP Reboot Statistics ([src/elements/WTPRebootStatistics.h](src/elements/WTPRebootStatistics.h))
- 4.6.48. WTP Static IP Address Information ([src/elements/WTPStaticIPAddressInformation.h](src/elements/WTPStaticIPAddressInformation.h))

#### Group 6: Structures with Dynamic Tail (Single Instance)
Single-occurrence elements in a message. Have several static fields and end with a variable-length field.

- 4.6.7. Add MAC ACL Entry ([src/elements/AddMacAclEntry.h](src/elements/AddMacAclEntry.h))
- 4.6.19. Delete MAC ACL Entry ([src/elements/DeleteMacAclEntry.h](src/elements/DeleteMacAclEntry.h))
- 4.6.26. Image Data ([src/elements/ImageData.h](src/elements/ImageData.h))
- 4.6.27. Image Identifier ([src/elements/ImageIdentifier.h](src/elements/ImageIdentifier.h))

#### Group 8: Static Structures (Array in Message)
Strictly fixed-size elements that can appear multiple times in a message.

- 4.6.9. CAPWAP Control IPv4 Address ([src/elements/CAPWAPControlIPv4Address.h](src/elements/CAPWAPControlIPv4Address.h))
- 4.6.11. CAPWAP Local IPv4 Address ([src/elements/CAPWAPLocalIPv4Address.h](src/elements/CAPWAPLocalIPv4Address.h))
- 4.6.18. Decryption Error Report Period ([src/elements/DecryptionErrorReportPeriod.h](src/elements/DecryptionErrorReportPeriod.h))
- 4.6.33. Radio Administrative State ([src/elements/RadioAdministrativeState.h](src/elements/RadioAdministrativeState.h))
- 4.6.34. Radio Operational State ([src/elements/RadioOperationalState.h](src/elements/RadioOperationalState.h))
- 4.6.46. WTP Radio Statistics ([src/elements/WTPRadioStatistics.h](src/elements/WTPRadioStatistics.h))

### IEEE 802.11 Binding Elements (RFC 5416)

Elements specific to IEEE 802.11 wireless binding, grouped by structure.

#### Group 1: Structures with Dynamic Field in the Middle (Array in Message)
Elements that can appear multiple times in a message, containing a variable-length field in the middle (whose length is specified by a preceding field) and ending with other static or dynamic fields.

- 6.1. IEEE 802.11 Add WLAN ([src/elements/IEEE80211/AddWlan.h](src/elements/IEEE80211/AddWlan.h))

#### Group 2: Static Structures (Array in Message)
Strictly fixed-size elements that can appear multiple times in a message (typically for each Radio ID, WLAN ID, or Station MAC).

- 6.3. IEEE 802.11 Assigned WTP BSSID ([src/elements/IEEE80211/AssignedWtpBssid.h](src/elements/IEEE80211/AssignedWtpBssid.h))
- 6.4. IEEE 802.11 Delete WLAN ([src/elements/IEEE80211/DeleteWlan.h](src/elements/IEEE80211/DeleteWlan.h))
- 6.5. IEEE 802.11 Direct Sequence Control ([src/elements/IEEE80211/DirectSequenceControl.h](src/elements/IEEE80211/DirectSequenceControl.h))
- 6.7. IEEE 802.11 MAC Operation ([src/elements/IEEE80211/MACOperation.h](src/elements/IEEE80211/MACOperation.h))
- 6.8. IEEE 802.11 MIC Countermeasures ([src/elements/IEEE80211/MICCountermeasures.h](src/elements/IEEE80211/MICCountermeasures.h))
- 6.9. IEEE 802.11 Multi-Domain Capability ([src/elements/IEEE80211/MultiDomainCapability.h](src/elements/IEEE80211/MultiDomainCapability.h))
- 6.10. IEEE 802.11 OFDM Control ([src/elements/IEEE80211/OFDMControl.h](src/elements/IEEE80211/OFDMControl.h))
- 6.12. IEEE 802.11 RSNA Error Report From Station ([src/elements/IEEE80211/RSNAErrorReportFromStation.h](src/elements/IEEE80211/RSNAErrorReportFromStation.h))
- 6.14. IEEE 802.11 Station QoS Profile ([src/elements/IEEE80211/StationQoSProfile.h](src/elements/IEEE80211/StationQoSProfile.h))
- 6.16. IEEE 802.11 Statistics ([src/elements/IEEE80211/Statistics.h](src/elements/IEEE80211/Statistics.h))
- 6.18. IEEE 802.11 Tx Power ([src/elements/IEEE80211/TxPower.h](src/elements/IEEE80211/TxPower.h))
- 6.20. IEEE 802.11 Update Station QoS ([src/elements/IEEE80211/UpdateStationQoS.h](src/elements/IEEE80211/UpdateStationQoS.h))
- 6.22. IEEE 802.11 WTP Quality of Service ([src/elements/IEEE80211/WTPQualityOfService.h](src/elements/IEEE80211/WTPQualityOfService.h))
- 6.23. IEEE 802.11 WTP Radio Configuration ([src/elements/IEEE80211/WTPRadioConfiguration.h](src/elements/IEEE80211/WTPRadioConfiguration.h))
- 6.24. IEEE 802.11 WTP Radio Fail Alarm Indication ([src/elements/IEEE80211/WTPRadioFailAlarmIndication.h](src/elements/IEEE80211/WTPRadioFailAlarmIndication.h))
- 6.25. IEEE 802.11 WTP Radio Information ([src/elements/IEEE80211/WTPRadioInformation.h](src/elements/IEEE80211/WTPRadioInformation.h))

#### Group 3: Structures with Dynamic Tail (Array in Message)
Elements that can appear multiple times in a message. Consist of initial static fields and end with a variable-length field (array of values, string, or raw data).

- 6.2. IEEE 802.11 Antenna ([src/elements/IEEE80211/Antenna.h](src/elements/IEEE80211/Antenna.h))
- 6.6. IEEE 802.11 Information Element ([src/elements/IEEE80211/InformationElement.h](src/elements/IEEE80211/InformationElement.h))
- 6.11. IEEE 802.11 Rate Set ([src/elements/IEEE80211/RateSet.h](src/elements/IEEE80211/RateSet.h))
- 6.13. IEEE 802.11 Station ([src/elements/IEEE80211/Station.h](src/elements/IEEE80211/Station.h))
- 6.15. IEEE 802.11 Station Session Key ([src/elements/IEEE80211/StationSessionKey.h](src/elements/IEEE80211/StationSessionKey.h))
- 6.17. IEEE 802.11 Supported Rates ([src/elements/IEEE80211/SupportedRates.h](src/elements/IEEE80211/SupportedRates.h))
- 6.19. IEEE 802.11 Tx Power Level ([src/elements/IEEE80211/TxPowerLevel.h](src/elements/IEEE80211/TxPowerLevel.h))
- 6.21. IEEE 802.11 Update WLAN ([src/elements/IEEE80211/UpdateWlan.h](src/elements/IEEE80211/UpdateWlan.h))
