

#include "DiscoveryRequest.h"
#include "elements/UnrecognizedElement.h"
#include "lassert.h"
#include "logging.h"

WritableDiscoveryRequest::WritableDiscoveryRequest(
    const DiscoveryType::Type discovery_type,
    const WritableWTPBoardData &wtp_board_data,
    const WritableWTPDescriptor &wtp_descriptor,
    const WTPFrameTunnelMode &wtp_frame_tunnel_mode,
    const WTPMACType::Type mac_type,
    const nonstd::span<const WTPRadioInformation> &wtp_radio_info,
    WritableVendorSpecificPayloadArray &vendor_specific_payloads,
    const uint16_t probe_mtu_size)
    : discovery_type{ discovery_type }, wtp_board_data{ wtp_board_data },
      wtp_descriptor{ wtp_descriptor }, wtp_frame_tunnel_mode{ wtp_frame_tunnel_mode },
      wtp_mac_type{ mac_type }, wtp_radio_informations{ wtp_radio_info },
      vendor_specific_payloads{ vendor_specific_payloads },
      padding{ CalcMTUPadding(probe_mtu_size) } {
}

ControlHeader::MessageType WritableDiscoveryRequest::GetMessageType() const {
    return ControlHeader::DiscoveryRequest;
}

ControlHeader::MessageType WritableDiscoveryRequest::GetResponseMessageType() const {
    return ControlHeader::DiscoveryResponse;
}

uint16_t WritableDiscoveryRequest::CalcTotalSize() {
    uint16_t total = discovery_type.GetTotalLength();
    total += wtp_board_data.GetTotalLength();
    total += wtp_descriptor.GetTotalLength();
    total += wtp_frame_tunnel_mode.GetTotalLength();
    total += wtp_mac_type.GetTotalLength();
    total += wtp_radio_informations.GetTotalLength();
    total += vendor_specific_payloads.GetTotalLength();

    return total;
}

uint16_t WritableDiscoveryRequest::CalcMessageLength(uint16_t probe_mtu_size) {
    if (probe_mtu_size == no_probe_mtu_size) {
        return CalcTotalSize();
    }

    uint16_t length = probe_mtu_size;
    length -= sizeof(ControlHeader);
    return length;
}

uint16_t WritableDiscoveryRequest::CalcMTUPadding(uint16_t probe_mtu_size) {
    if (probe_mtu_size == no_probe_mtu_size) {
        return no_probe_mtu_size;
    }
    uint16_t header_size = sizeof(ClearHeader);
    header_size += sizeof(ControlHeader);
    header_size += sizeof(MTUDiscoveryPadding);
    ASSERT(header_size < probe_mtu_size);

    uint16_t total = CalcTotalSize() + header_size;
    ASSERT(total < probe_mtu_size);
    return probe_mtu_size - total;
}

void WritableDiscoveryRequest::Serialize(RawData *raw_data) const {
    discovery_type.Serialize(raw_data);
    wtp_board_data.Serialize(raw_data);
    wtp_descriptor.Serialize(raw_data);
    wtp_frame_tunnel_mode.Serialize(raw_data);
    wtp_mac_type.Serialize(raw_data);
    wtp_radio_informations.Serialize(raw_data);
    vendor_specific_payloads.Serialize(raw_data);
    if (padding.GetLength() != no_probe_mtu_size) {
        padding.Serialize(raw_data);
    }
}

ReadableDiscoveryRequest::ReadableDiscoveryRequest() : unknown_elements{} {
    discovery_type = nullptr;
    wtp_frame_tunnel_mode = nullptr;
    wtp_mac_type = nullptr;
    padding = nullptr;
}

ControlHeader::MessageType ReadableDiscoveryRequest::GetMessageType() const {
    return ControlHeader::DiscoveryRequest;
}

bool ReadableDiscoveryRequest::Deserialize(RawData *raw_data) {
    while (raw_data->current + sizeof(ElementHeader) <= raw_data->end) {
        ElementHeader *element = (ElementHeader *)raw_data->current;

        switch (element->GetElementType()) {
            case ElementHeader::ElementType::DiscoveryType:
                discovery_type = DiscoveryType::Deserialize(raw_data);
                if (discovery_type == nullptr) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::WTPBoardData:
                if (!wtp_board_data.Deserialize(raw_data)) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::WTPDescriptor:
                if (!wtp_descriptor.Deserialize(raw_data)) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::WTPFrameTunnelMode:
                wtp_frame_tunnel_mode = WTPFrameTunnelMode::Deserialize(raw_data);
                if (wtp_frame_tunnel_mode == nullptr) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::WTPMACType:
                wtp_mac_type = WTPMACType::Deserialize(raw_data);
                if (wtp_mac_type == nullptr) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::WTPRadioInformation:
                if (!wtp_radio_informations.Deserialize(raw_data)) {
                    return false;
                }
                break;
            case ElementHeader::ElementType::VendorSpecificPayload:
                if (!vendor_specific_payloads.Deserialize(raw_data)) {
                    return false;
                }
                break;

            case ElementHeader::ElementType::MTUDiscoveryPadding:
                padding = MTUDiscoveryPadding::Deserialize(raw_data);
                if (padding == nullptr) {
                    return false;
                }
                break;

            default: {
                auto unknownElement = UnrecognizedElement::Deserialize(raw_data);
                if (unknownElement == nullptr) {
                    return false;
                }
                unknown_elements++;
                log_w("ReadableDiscoveryRequest::Deserialize unhandled element type: 0x%04X",
                      unknownElement->GetElementType());

                break;
            }
        }
    }
    return discovery_type != nullptr && wtp_board_data.header != nullptr
        && wtp_descriptor.header != nullptr && wtp_frame_tunnel_mode != nullptr
        && wtp_mac_type != nullptr && wtp_radio_informations.Get().size() > 0;
}

void ReadableDiscoveryRequest::Log() const {
    log_i("----------------------------------");
    log_i("ME DiscoveryRequest:");

    ASSERT(discovery_type != nullptr);
    discovery_type->Log();

    wtp_board_data.Log();
    wtp_descriptor.Log();

    ASSERT(wtp_frame_tunnel_mode != nullptr);
    wtp_frame_tunnel_mode->Log();

    ASSERT(wtp_mac_type != nullptr);
    wtp_mac_type->Log();

    wtp_radio_informations.Log();

    vendor_specific_payloads.Log();
    if (padding != nullptr) {
        padding->Log();
    }

    if (unknown_elements > 0) {
        log_i("  UnknownElements count: %lu", unknown_elements);
    }
    log_i("----------------------------------");
}