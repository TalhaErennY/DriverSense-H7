/*
 * stm32h7sxx_eth_net.c
 *
 *  Created on: 12 Haz 2026
 *      Author: talha
 */

#include "stm32h7sxx_eth_net.h"

/******************************************************************************************************
 * 								Private Helper Prototypes
 ******************************************************************************************************/
//Read Functions
static uint16_t ReadU16BE(const uint8_t *pData, uint32_t offset);
static uint16_t ReadU16LE(const uint8_t *pData, uint32_t offset);
static uint32_t ReadU32BE(const uint8_t *pData, uint32_t offset);
static uint32_t ReadU32LE(const uint8_t *pData, uint32_t offset);
static float ReadF32LE(const uint8_t *pData, uint32_t offset);
static float ReadF32BE(const uint8_t *pData, uint32_t offset);

//Write Functions
static void WriteU16BE(uint8_t *pData, uint32_t offset, uint16_t value);
static void WriteU16LE(uint8_t *pData, uint32_t offset, uint16_t value);
static void WriteU32BE(uint8_t *pData, uint32_t offset, uint32_t value);
static void WriteU32LE(uint8_t *pData, uint32_t offset, uint32_t value);

/******************************************************************************************************
 * 								Ethernet Frame Parser Functions
 ******************************************************************************************************/
/****************************************************************************
 * @fn				- ETHNET_GetEtherType
 *
 * @brief			- This function checks and returns the EtherType field of
 * 					  a raw Ethernet II frame
 *
 * @param[in]		- pointer to raw Ethernet frame buffer
 * @param[in]		- raw Ethernet frame length
 * @param[in]		-
 *
 * @return			- EtherType value, 0 if frame is invalid or not Ethernet II
 *
 * @Note			- EtherType is located at byte 12 and byte 13 of the
 * 					  Ethernet frame.
 * 					- If this field is less than 0x0600, the frame is interpreted
 * 					  as IEEE 802.3 length frame and 0 is returned.
 *					-
 *
 */
uint16_t ETHNET_GetEtherType(const uint8_t *pFrame, uint32_t FrameLength){
	uint16_t type_or_len;

	if(pFrame == 0) return 0U;
	if(FrameLength < ETHNET_HEADER_LEN) return 0U;

	type_or_len = ReadU16BE(pFrame, ETHNET_TYPE_OFFSET);

	if(type_or_len < ETHNET_FRAME_TYPE_MIN){
		//IEEE 802.3 Length frame
		return 0U;
	}

	return type_or_len;
}

/****************************************************************************
 * @fn				- ETHNET_ParseEthernetFrame
 *
 * @brief			- This function parses Ethernet II / IEEE 802.3 frame header
 *
 * @param[in]		- pointer to raw Ethernet frame buffer
 * @param[in]		- raw Ethernet frame length
 * @param[in]		- pointer to parsed Ethernet frame structure
 *
 * @return			- 1 if frame header is valid, 0 otherwise
 *
 * @Note			- For Ethernet II frames, pOut->ethertype contains real EtherType.
 * 					- For IEEE 802.3 frames, pOut->type_or_len contains payload length
 * 					  and pOut->ethertype is 0.
 *					-
 *
 */
uint8_t ETHNET_ParseEthernetFrame(const uint8_t *pFrame, uint32_t FrameLength, ETHNET_Frame_t* pOut){
	uint16_t type_or_len;

	if((pFrame == 0) || (pOut == 0)) return 0U;
	if(FrameLength < ETHNET_HEADER_LEN) return 0U;

	type_or_len = ReadU16BE(pFrame, ETHNET_TYPE_OFFSET);

	pOut->frame 	= pFrame;
	pOut->frame_len = FrameLength;

	pOut->dst_mac	= &pFrame[ETHNET_DST_MAC_OFFSET];
	pOut->src_mac	= &pFrame[ETHNET_SRC_MAC_OFFSET];

	pOut->type_or_len  = type_or_len;
	pOut->ethertype    = 0U;
	pOut->frame_format = ETHNET_FRAME_UNKNOWN;

	pOut->payload     = &pFrame[ETHNET_HEADER_LEN];
	pOut->payload_len = FrameLength - ETHNET_HEADER_LEN;

	if(type_or_len >= ETHNET_FRAME_TYPE_MIN){
		//Ethernet II Frame
		pOut->frame_format = ETHNET_FRAME_ETHERNET_II;
		pOut->ethertype    = type_or_len;
	} else if(type_or_len <= ETHNET_8023_MAX_LEN){
		//IEEE 802.3 Frame
		pOut->frame_format = ETHNET_FRAME_IEEE_802_3;

		//Length check
		if(type_or_len > pOut->payload_len) return 0U;

		pOut->payload_len = type_or_len;
	} else{
		return 0U;
	}

	return 1U;
}

/******************************************************************************************************
 * 								UDP / IPv4 Functions
 ******************************************************************************************************/
/****************************************************************************
 * @fn				- ETHNET_GetUDPPayload
 *
 * @brief			- This function extracts UDP payload pointer and payload
 * 					  length from a raw Ethernet frame
 *
 * @param[in]		- pointer to raw Ethernet frame buffer
 * @param[in]		- raw Ethernet frame length
 * @param[in]		- expected UDP destination port
 * @param[in]		- pointer to payload pointer variable
 * @param[in]		- pointer to payload length variable
 *
 * @return			- 1 if valid UDP payload is found, 0 otherwise
 *
 * @Note			- This function checks Ethernet II, IPv4, UDP protocol and
 * 					  destination port.
 * 					- UDP payload is not null-terminated.
 *					- User must use returned payload length.
 *
 */
uint8_t ETHNET_GetUDPPayload(const uint8_t* pFrame, uint32_t FrameLength, uint16_t ExpectedPort, const uint8_t **ppPayload, uint32_t *pPayloadLength){
	uint16_t ethertype;

	uint8_t ip_header_len;
	uint8_t ip_version;
	uint8_t ip_protocol;

	uint32_t udp_header_offset;
	uint32_t udp_payload_offset;

	uint16_t udp_dest_port;
	uint16_t udp_len;

	if((pFrame == 0) || (ppPayload == 0) || (pPayloadLength == 0)) return 0U;

	*ppPayload = 0;
	*pPayloadLength = 0;

	/*
	 * Check Ethernet Frame Type
	 */
	ethertype = ETHNET_GetEtherType(pFrame, FrameLength);

	if(ethertype != ETHNET_ETHERTYPE_IPV4){
		return 0U;
	}

	/*
	 * Minimum Ethernet + IPv4 + UDP Header Length Check
	 */
	if(FrameLength < (ETHNET_IPV4_MIN_HEADER_LEN + ETHNET_UDP_HEADER_LEN + ETHNET_HEADER_LEN)){
		return 0U;
	}

	/*
	 * IPv4 header starts after Ethernet header.
	 * pFrame[14] example:
	 * 0x45 -> Version = 4, IHL = 5
	 */
	ip_version 		= (uint8_t)(pFrame[ETHNET_HEADER_LEN] >> 4U);		   	// Upper 4 bits show the version
	ip_header_len 	= (uint8_t)((pFrame[ETHNET_HEADER_LEN] & 0x0FU) * 4U); 	// This shows how many 32-bit word exists

	//Checks
	if(ip_version != 4U) return 0U;
	if(ip_header_len < ETHNET_IPV4_MIN_HEADER_LEN) return 0U;
	if(FrameLength < (ETHNET_HEADER_LEN + ip_header_len + ETHNET_UDP_HEADER_LEN)) return 0U;

	/*
	 * IPv4 protocol field is at offset 9 inside IPv4 header
	 * UDP = 17
	 */
	ip_protocol = (uint8_t)(pFrame[ETHNET_HEADER_LEN + 9U]);

	if(ip_protocol != ETHNET_IPV4_PROTOCOL_UDP) return 0U;

	/*
	 * UDP Header starts after Ethernet Header + IPv4 Header
	 */
	udp_header_offset = ETHNET_HEADER_LEN + ip_header_len;

	/*
	 * UDP Destiantion port is byte 2-3 inside UDP Header
	 *
	 * Since the UDP data is being written in Big Endian use Read16BE function
	 */
	udp_dest_port = ReadU16BE(pFrame, udp_header_offset + 2U);

	if(udp_dest_port != ExpectedPort) return 0U;

	/*
	 * UDP Length is byte 4-5 inside UDP header
	 *
	 * Header + Payload
	 */
	udp_len = ReadU16BE(pFrame, udp_header_offset + 4U);

	if(udp_len < ETHNET_UDP_HEADER_LEN) return 0U;

	udp_payload_offset = udp_header_offset + ETHNET_UDP_HEADER_LEN;

	/*
	 * Overflow check
	 */
	if((udp_payload_offset + (uint32_t)(udp_len - ETHNET_UDP_HEADER_LEN)) > FrameLength) return 0U;

	*ppPayload      = &pFrame[udp_payload_offset];
	*pPayloadLength = (uint32_t)(udp_len - ETHNET_UDP_HEADER_LEN);

	return 1U;
}

/****************************************************************************
 * @fn				- ETHNET_ParseARPPacket
 *
 * @brief			- This parses the incoming ARP Packet
 *
 * @param[in]		- Pointer variable for payload
 * @param[in]		- Payload length
 * @param[in]		- ETHNET ARP_PACK
 * @param[in]		-
 * @param[in]		-
 *
 * @return			-
 *
 * @Note			-
 * 					-
 *					-
 *
 */
uint8_t ETHNET_ParseARPPacket(const uint8_t *pPayload, uint32_t PayloadLen, ETHNET_ARP_Packet_t *pARP){
	//Pre-flight checks
	if((pPayload == 0) || pARP == 0) return 0U;
	if(PayloadLen < ETHNET_ARP_PACKET_LEN) return 0U;

	pARP->htype = ReadU16BE(pPayload, ETHNET_ARP_HTYPE_OFFSET);
	pARP->ptype = ReadU16BE(pPayload, ETHNET_ARP_PTYPE_OFFSET);
	pARP->hlen  = pPayload[ETHNET_ARP_HLEN_OFFSET];
	pARP->plen  = pPayload[ETHNET_ARP_PLEN_OFFSET];
	pARP->oper  = ReadU16BE(pPayload, ETHNET_ARP_OPER_OFFSET);

	/*
	 * This parser currently supports only Ethnernet + IPv4 ARP
	 */
	if(pARP->htype != ETHNET_ARP_HTYPE_ETHERNET) return 0U;
	if(pARP->ptype != ETHNET_ARP_PTYPE_IPV4) return 0U;
	if(pARP->hlen  != ETHNET_ARP_HLEN_ETHERNET) return 0U;
	if(pARP->plen  != ETHNET_ARP_PLEN_IPV4) return 0U;

	pARP->sender_mac = &pPayload[ETHNET_ARP_SHA_OFFSET];
	pARP->sender_ip	 = &pPayload[ETHNET_ARP_SPA_OFFSET];
	pARP->target_mac = &pPayload[ETHNET_ARP_THA_OFFSET];
	pARP->target_ip  = &pPayload[ETHNET_ARP_TPA_OFFSET];

	return 1U;
}

/****************************************************************************
 * @fn				- ETHNET_IsARPRequestForIP
 *
 * @brief			-
 *
 * @param[in]		- Pointer variable for payload
 * @param[in]		- Payload length
 * @param[in]		- ETHNET ARP_PACK
 * @param[in]		-
 * @param[in]		-
 *
 * @return			-
 *
 * @Note			-
 * 					-
 *					-
 *
 */
uint8_t ETHNET_IsARPRequestForIP(const ETHNET_ARP_Packet_t *pARP, const uint8_t ip[4]){
	if((pARP == 0) || (ip == 0)) return 0U;
	if(pARP->oper != ETHNET_ARP_OPER_REQUEST) return 0U;
	if(pARP->target_ip == 0) return 0U;

	if((pARP->target_ip[0] == ip[0]) && (pARP->target_ip[1] == ip[1]) &&
	   (pARP->target_ip[2] == ip[2]) && (pARP->target_ip[3] == ip[3])) {
		return 1U;
	}

	return 0U;

}

/****************************************************************************
 * @fn				- ETHNET_BuildARPReply
 *
 * @brief			- This function builds an Ethernet II ARP reply frame
 *
 * @param[in]		- pointer to output Ethernet frame buffer
 * @param[in]		- STM32 MAC address
 * @param[in]		- STM32 IPv4 address
 * @param[in]		- parsed ARP request packet
 *
 * @return			- ARP reply Ethernet frame length, 0 if build fails
 *
 * @Note			- This function only builds Ethernet + IPv4 ARP reply.
 * 					- Returned frame length is 60 bytes because Ethernet minimum
 * 					  frame length without FCS is 60 bytes.
 *					- FCS/CRC is added by Ethernet MAC hardware.
 *
 */
uint32_t ETHNET_BuildARPReply(uint8_t *pOutFrame, const uint8_t myMAC[ETHNET_MAC_ADDR_LEN], const uint8_t myIP[ETHNET_IPV4_ADDR_LEN], const ETHNET_ARP_Packet_t *pRequest){
	uint32_t arp_offset;

	if((pOutFrame == 0U) || (myMAC == 0U) || (myIP == 0U) || (pRequest == 0U)) return 0U;

	if(pRequest->sender_mac == 0U) return 0U;
	if(pRequest->sender_ip == 0U) return 0U;

	/*
	 * Clear Minimum Ethernet Area
	 */
	for(uint32_t i = 0U; i < 60U; i++){
		pOutFrame[i] = 0;
	}

	/*
	 * Ethernet II header
	 *
	 * Destination MAC = ARP request sender MAC
	 * Source MAC      = STM32 MAC
	 * EtherType       = ARP, 0x0806
	 */
	for(uint32_t i = 0U; i < ETHNET_MAC_ADDR_LEN; i++){
		pOutFrame[ETHNET_DST_MAC_OFFSET + i] = pRequest->sender_mac[i];
		pOutFrame[ETHNET_SRC_MAC_OFFSET + i] = myMAC[i];
	}

	/*
	 * Write EtherType to Frame
	 */
	WriteU16BE(pOutFrame, (ETHNET_TYPE_OFFSET), ETHNET_ETHERTYPE_ARP);

	/*
	 * ARP Header
	 */
	arp_offset = ETHNET_HEADER_LEN;

	/*
	 * ARP fixed fields
	 *
	 * HTYPE = Ethernet, 0x0001
	 * PTYPE = IPv4,     0x0800
	 * HLEN  = 6
	 * PLEN  = 4
	 * OPER  = Reply,    0x0002
	 */
	WriteU16BE(pOutFrame, (arp_offset + ETHNET_ARP_HTYPE_OFFSET), ETHNET_ARP_HTYPE_ETHERNET);
	WriteU16BE(pOutFrame, (arp_offset + ETHNET_ARP_PTYPE_OFFSET), ETHNET_ARP_PTYPE_IPV4);
	pOutFrame[arp_offset + ETHNET_ARP_HLEN_OFFSET] = ETHNET_ARP_HLEN_ETHERNET;
	pOutFrame[arp_offset + ETHNET_ARP_PLEN_OFFSET] = ETHNET_ARP_PLEN_IPV4;
	WriteU16BE(pOutFrame, (arp_offset + ETHNET_ARP_OPER_OFFSET), ETHNET_ARP_OPER_REPLY);

	/*
	 * Sender Fields in ARP Reply
	 */
	for(uint8_t i = 0U; i < ETHNET_MAC_ADDR_LEN; i++){
		pOutFrame[arp_offset + ETHNET_ARP_SHA_OFFSET + i] = myMAC[i];
	}

	for(uint8_t i = 0U; i < ETHNET_IPV4_ADDR_LEN; i++){
		pOutFrame[arp_offset + ETHNET_ARP_SPA_OFFSET + i] = myIP[i];
	}

	/*
	 * Target Fields in ARP Reply
	 */
	for(uint8_t i = 0U; i < ETHNET_MAC_ADDR_LEN; i++){
		pOutFrame[arp_offset + ETHNET_ARP_THA_OFFSET + i] = pRequest->sender_mac[i];
	}

	for(uint8_t i = 0U; i < ETHNET_IPV4_ADDR_LEN; i++){
		pOutFrame[arp_offset + ETHNET_ARP_TPA_OFFSET + i] = pRequest->sender_ip[i];
	}

	/*
	 * Ethernet header 14 + ARP packet 28 = 42 bytes
	 * But minimum Ethernet Frame without FCS is 60 bytes
	 * Remaining bytes are already zero because buffer was cleared
	 */
	return 60U;

}

/******************************************************************************************************
 * 								MAC Helper Functions
 ******************************************************************************************************/

/****************************************************************************
 * @fn				- ETHNET_IsBroadcastMAC
 *
 * @brief			- This function checks whether given MAC address is broadcast MAC
 *
 * @param[in]		- pointer to MAC address
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- 1 if MAC address is FF:FF:FF:FF:FF:FF, 0 otherwise
 *
 * @Note			- none
 *
 */
uint8_t ETHNET_IsBroadcastMAC(const uint8_t *pMAC){
	if(pMAC == 0) return 0U;

	for(uint8_t i = 0U; i < ETHNET_MAC_ADDR_LEN; i++){
		if(pMAC[i] != 0xFFU){
			return 0U;
		}
	}

	return 1U;
}

/****************************************************************************
 * @fn				- ETHNET_IsSameMAC
 *
 * @brief			- This function compares two MAC addresses
 *
 * @param[in]		- pointer to first MAC address
 * @param[in]		- pointer to second MAC address
 * @param[in]		-
 *
 * @return			- 1 if MAC addresses are same, 0 otherwise
 *
 * @Note			- none
 *
 */
uint8_t ETHNET_IsSameMAC(const uint8_t *pMAC1, const uint8_t *pMAC2){
	if((pMAC1 == 0) || (pMAC2 == 0)) return 0U;

	for(uint8_t i = 0U; i < ETHNET_MAC_ADDR_LEN; i++){
		if(pMAC1[i] != pMAC2[i]) return 0U;
	}

	return 1U;
}


/******************************************************************************************************
 * 								Private Helper Functions
 ******************************************************************************************************/
/*
 * Read 16-bit big-endian unsigned value from byte buffer.
 */
static uint16_t ReadU16BE(const uint8_t *pData, uint32_t offset){
	return ((uint16_t)pData[offset] << 8U) | ((uint16_t)pData[offset + 1U]);
}

/*
 * Read 16-bit little-endian unsigned value from byte buffer.
 */
static uint16_t ReadU16LE(const uint8_t *pData, uint32_t offset){
	return ((uint16_t)pData[offset + 1U] << 8U) | ((uint16_t)pData[offset]);
}

/*
 * Read 32-bit big-endian unsigned value from byte buffer.
 */
static uint32_t ReadU32BE(const uint8_t *pData, uint32_t offset){
	return ((uint32_t)pData[offset] << 24U) 	 |
			((uint32_t)pData[offset + 1U] << 16U)|
			((uint32_t)pData[offset + 2U] << 8U) |
			((uint32_t)pData[offset + 3U]);

}

/*
 * Read 32-bit little-endian unsigned value from byte buffer.
 */
static uint32_t ReadU32LE(const uint8_t *pData, uint32_t offset){
	return ((uint32_t)pData[offset]) 			 |
		   ((uint32_t)pData[offset + 1U] << 8U)  |
		   ((uint32_t)pData[offset + 2U] << 16U) |
		   ((uint32_t)pData[offset + 3U] << 24U);
}

/*
 * Read 32-bit little-endian float value from byte buffer.
 *
 * <!Enable FPU before using this function.>
 */
static float ReadF32LE(const uint8_t *pData, uint32_t offset){
	union{
		uint32_t u32;
		float f32;
	} value;

	value.u32 = ReadU32LE(pData, offset);

	return value.f32;
}

/*
 * Read 32-bit big-endian float value from byte buffer.
 *
 * <!Enable FPU before using this function.>
 */
static float ReadF32BE(const uint8_t *pData, uint32_t offset){
	union{
		uint32_t u32;
		float f32;
	} value;

	value.u32 = ReadU32BE(pData, offset);

	return value.f32;
}

/*
 * Write 16-bit big-endian unsigned value to byte buffer.
 */
static void WriteU16BE(uint8_t *pData, uint32_t offset, uint16_t value){
	pData[offset]		= (uint8_t)(value >> 8U);
	pData[offset + 1U]	= (uint8_t)(value);
}

/*
 * Write 16-bit little-endian unsigned value to byte buffer.
 */
static void WriteU16LE(uint8_t *pData, uint32_t offset, uint16_t value){
	pData[offset]		= (uint8_t)(value);
	pData[offset + 1U]	= (uint8_t)(value >> 8U);
}

/*
 * Write 32-bit big-endian unsigned value to byte buffer.
 */
static void WriteU32BE(uint8_t *pData, uint32_t offset, uint32_t value){
	pData[offset]		= (uint8_t)(value >> 24U);
	pData[offset + 1U]	= (uint8_t)(value >> 16U);
	pData[offset + 2U]	= (uint8_t)(value >> 8U);
	pData[offset + 3U]	= (uint8_t)(value);
}

/*
 * Write 32-bit little-endian unsigned value to byte buffer.
 */
static void WriteU32LE(uint8_t *pData, uint32_t offset, uint32_t value){
	pData[offset]		= (uint8_t)(value);
	pData[offset + 1U]	= (uint8_t)(value >> 8U);
	pData[offset + 2U]	= (uint8_t)(value >> 16U);
	pData[offset + 3U]	= (uint8_t)(value >> 24U);
}
