#include "elements/MacAddress.h"
#include "ClearHeader.h"
#include "lassert.h"
#include "logging.h"
#include <arpa/inet.h>
#include <string.h>

WritableMacAddress::WritableMacAddress(const nonstd::span<const uint8_t> &mac_address)
    : MACAddress{ mac_address } {
}

bool ReadableMacAddress::Validate() const {
    switch (Length) {
        case RadioMACAddress::mac_EUI48_size:
        case RadioMACAddress::mac_EUI64_size:
            return true;
        default:
            break;
    }
    return false;
}

MacAddress::MacAddress(const nonstd::span<const uint8_t> &mac_address) {
    Length = mac_address.size();
    ASSERT(sizeof(Address) >= Length);
    memcpy(Address, mac_address.data(), Length);
}

MacAddress::MacAddress(const WritableMacAddress &mac_address) {
    Length = mac_address.MACAddress.size();
    ASSERT(sizeof(Address) >= Length);
    memcpy(Address, mac_address.MACAddress.data(), Length);
}

void MacAddress::Log(const size_t index, const uint8_t length, const uint8_t *mac_address) {
    switch (length) {
        case RadioMACAddress::mac_EUI48_size:
            log_i("     #%lu, [EUI48] %02X:%02X:%02X:%02X:%02X:%02X",
                  index,
                  mac_address[0],
                  mac_address[1],
                  mac_address[2],
                  mac_address[3],
                  mac_address[4],
                  mac_address[5]);
            break;

        case RadioMACAddress::mac_EUI64_size:
            log_i("     #%lu, [EUI64] %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
                  index,
                  mac_address[0],
                  mac_address[1],
                  mac_address[2],
                  mac_address[3],
                  mac_address[4],
                  mac_address[5],
                  mac_address[6],
                  mac_address[7]);
            break;

        default:
            log_i("     #%lu, [EUIXX] Length: %u", index, length);
            break;
    }
}

bool MacAddress::operator==(const MacAddress &other) const {
    return Length == other.Length && memcmp(Address, other.Address, Length) == 0;
}