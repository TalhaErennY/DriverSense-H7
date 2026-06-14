/*
 * stm32h7sxx_eth_net.h
 *
 *  Created on: 12 Haz 2026
 *      Author: talha
 */

#ifndef INC_STM32H7SXX_ETH_NET_H_
#define INC_STM32H7SXX_ETH_NET_H_

#include <stdint.h>
#include "stm32h7sxx.h"

/*
 * @ETHNET_ADDRESS_LENGTH
 * Ethernet address length macros
 */
#define ETHNET_MAC_ADDR_LEN             6U
#define ETHNET_IPV4_ADDR_LEN            4U

/*
 * @ETHNET_FRAME_OFFSETS
 * Ethernet frame offset selection macros
 */
#define ETHNET_DST_MAC_OFFSET           0U
#define ETHNET_SRC_MAC_OFFSET           6U
#define ETHNET_TYPE_OFFSET              12U
#define ETHNET_HEADER_LEN               14U

/*
 * @ETHNET_FRAME_FORMAT_LIMITS
 * Ethernet II / IEEE 802.3 type-length field limit macros
 *
 * If type_or_len >= 0x0600, the field is interpreted as EtherType.
 * If type_or_len <= 1500, the field is interpreted as IEEE 802.3 payload length.
 */
#define ETHNET_FRAME_TYPE_MIN           0x0600U
#define ETHNET_8023_MAX_LEN             1500U

/*
 * @ETHNET_ETHERTYPES
 * Ethernet II EtherType selection macros
 */
#define ETHNET_ETHERTYPE_IPV4           0x0800U
#define ETHNET_ETHERTYPE_ARP            0x0806U
#define ETHNET_ETHERTYPE_IPV6           0x86DDU
#define ETHNET_ETHERTYPE_VLAN           0x8100U

/*
 * @ETHNET_FRAME_FORMATS
 * Parsed Ethernet frame format selection macros
 */
#define ETHNET_FRAME_UNKNOWN            0U
#define ETHNET_FRAME_ETHERNET_II        1U
#define ETHNET_FRAME_IEEE_802_3         2U

/*
 * @ETHNET_IPV4_SELECTION
 * IPv4 protocol and header selection macros
 */
#define ETHNET_IPV4_MIN_HEADER_LEN      20U
#define ETHNET_IPV4_PROTOCOL_UDP        17U

/*
 * @ETHNET_UDP_SELECTION
 * UDP header length macro
 */
#define ETHNET_UDP_HEADER_LEN           8U

/*
 * Ethernet II Header Struct
 *
 * This struct is only for layout reference.
 * Parser functions use byte offsets because Ethernet fields are big-endian.
 */
typedef struct __attribute__((packed)){
	uint8_t dst_mac[ETHNET_MAC_ADDR_LEN];
	uint8_t src_mac[ETHNET_MAC_ADDR_LEN];
	uint16_t ethertype;
} EthernetII_Header_t;

/*
 * Parsed Ethernet frame selection
 */
typedef struct{
	const uint8_t *frame;
	uint32_t frame_len;

	const uint8_t *dst_mac;
	const uint8_t *src_mac;

	uint16_t type_or_len;
	uint16_t ethertype;

	uint8_t frame_format;

	const uint8_t *payload;
	uint32_t payload_len;
} ETHNET_Frame_t;

/*
 * ARP Packet Offsets
 */
#define ETHNET_ARP_HTYPE_OFFSET				0U		// Hardware Type: 1 for Ethernet
#define ETHNET_ARP_PTYPE_OFFSET				2U		// Protocol Type: 0x0800 for IPv4
#define ETHNET_ARP_HLEN_OFFSET				4U		// Hardware Address Length: 6 for Ethernet
#define ETHNET_ARP_PLEN_OFFSET				5U		// Protocol Address Length: 4 for IPv4
#define ETHNET_ARP_OPER_OFFSET				6U		// Operation (Sender): 1 for request, 2 for reply
#define ETHNET_ARP_SHA_OFFSET				8U		// Sender Hardware Address: MAC Address of Sender (Source)
#define ETHNET_ARP_SPA_OFFSET				14U		// Sender Protocol Address: IPv4 address of Sender (Source)
#define ETHNET_ARP_THA_OFFSET				18U		// Target Hardware Address: MAC Address of Target (Destination)
#define ETHNET_ARP_TPA_OFFSET				24U		// Target Protocol Address: IPv4 Address of Target (Destination)

/*
 * ARP Packet Selection
 */
#define ETHNET_ARP_PACKET_LEN           	28U

#define ETHNET_ARP_HTYPE_ETHERNET         	0x0001U
#define ETHNET_ARP_PTYPE_IPV4             	0x0800U

#define ETHNET_ARP_HLEN_ETHERNET          	6U
#define ETHNET_ARP_PLEN_IPV4              	4U

#define ETHNET_ARP_OPER_REQUEST         	0x0001U
#define ETHNET_ARP_OPER_REPLY           	0x0002U

/*
 * Parsed ARP Packet Selection
 */
typedef struct{
	uint16_t htype;
	uint16_t ptype;
	uint8_t hlen;
	uint8_t plen;
	uint16_t oper;

	const uint8_t *sender_mac;
	const uint8_t *sender_ip;
	const uint8_t *target_mac;
	const uint8_t *target_ip;
} ETHNET_ARP_Packet_t;

/******************************************************************************************************
 * 								APIs Supported by This Driver
 *       For more information about the APIs, check the function definitions in stm32h7sxx_eth_net.c
 ******************************************************************************************************/

/*
 * Ethernet Frame Parser Functions
 */
uint16_t ETHNET_GetEtherType(const uint8_t *pFrame, uint32_t FrameLength);
uint8_t ETHNET_ParseEthernetFrame(const uint8_t *pFrame, uint32_t FrameLength, ETHNET_Frame_t *pOut);

/*
 * UDP / IPv4 Parser Functions
 */
uint8_t ETHNET_GetUDPPayload(const uint8_t *pFrame, uint32_t FrameLength, uint16_t ExpectedDestPort, const uint8_t **ppPayload, uint32_t *pPayloadLength);

/*
 * ARP Functions
 */
uint8_t ETHNET_ParseARPPacket(const uint8_t *pPayload, uint32_t PayloadLen, ETHNET_ARP_Packet_t *pArp);
uint32_t ETHNET_BuildARPReply(uint8_t *pOutFrame, const uint8_t myMAC[ETHNET_MAC_ADDR_LEN], const uint8_t myIP[ETHNET_IPV4_ADDR_LEN], const ETHNET_ARP_Packet_t *pRequest);

/*
 * Helper Functions
 */
uint8_t ETHNET_IsBroadcastMAC(const uint8_t *pMAC);
uint8_t ETHNET_IsSameMAC(const uint8_t *pMAC1, const uint8_t *pMAC2);
uint8_t ETHNET_IsARPRequestForIP(const ETHNET_ARP_Packet_t *pARP, const uint8_t ip[4]);


#endif /* INC_STM32H7SXX_ETH_NET_H_ */
