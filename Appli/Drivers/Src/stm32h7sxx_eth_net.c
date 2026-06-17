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

/****************************************************************************
 * @fn				- ETHNET_BuildICMPEchoReply
 *
 * @brief			- This function builds an Ethernet II IPv4 ICMP echo
 * 					  reply frame from an incoming ICMP echo request
 *
 * @param[in]		- pointer to output Ethernet frame buffer
 * @param[in]		- pointer to incoming Ethernet frame buffer
 * @param[in]		- incoming Ethernet frame length
 * @param[in]		- local MAC address
 * @param[in]		- local IPv4 address
 *
 * @return			- ICMP echo reply Ethernet frame length, 0 if build fails
 *
 * @Note			- This function supports IPv4 packets without IP options.
 * 					- Ethernet, IPv4 and ICMP checksums are handled here.
 *					- FCS/CRC is added by Ethernet MAC hardware.
 *
 */
uint32_t ETHNET_BuildICMPEchoReply(uint8_t *pOut, uint32_t OutSize, const uint8_t *pRequest, uint32_t RequestLen, const uint8_t myMAC[ETHNET_MAC_ADDR_LEN], const uint8_t myIP[ETHNET_IPV4_ADDR_LEN]){
	uint32_t ip_offset;
	uint32_t icmp_offset;
	uint32_t frame_copy_len;
	uint32_t reply_len;

	uint16_t ip_total_len;

	uint8_t ip_header_len;
	uint8_t ip_ver;
	uint8_t ip_protocol;

	if((pOut == 0U) || (pRequest == 0U) || (myMAC == 0U) || (myIP == 0U)) return 0U;
	if(OutSize < 60U) return 0U;
	if(RequestLen < (ETHNET_HEADER_LEN + ETHNET_IPV4_MIN_HEADER_LEN + ETHNET_ICMP_HEADER_LEN)) return 0U;

	/*
	 * IPv4 header starts after Ethernet header.
	 */
	ip_offset = ETHNET_HEADER_LEN;

	ip_ver = (uint8_t)(pRequest[ip_offset + ETHNET_IPV4_VER_IHL_OFFSET] >> 4U);
	ip_header_len = (uint8_t)((pRequest[ip_offset + ETHNET_IPV4_VER_IHL_OFFSET] & 0x0FU) * 4U);

	if(ip_ver != 4U) return 0U;

	/*
	 * For now, only IPv4 header without options is supported.
	 */
	if(ip_header_len != ETHNET_IPV4_MIN_HEADER_LEN) return 0U;

	ip_total_len = ReadU16BE(pRequest, ip_offset + ETHNET_IPV4_TOTAL_LEN_OFFSET);

	if(ip_total_len < (ETHNET_IPV4_MIN_HEADER_LEN + ETHNET_ICMP_HEADER_LEN)) return 0U;

	/*
	 * Check whether received Ethernet frame really contains complete IPv4 packet.
	 */
	if((ETHNET_HEADER_LEN + (uint32_t)ip_total_len) > RequestLen) return 0U;

	ip_protocol = pRequest[ip_offset + ETHNET_IPV4_PROTOCOL_OFFSET];

	if(ip_protocol != ETHNET_IPV4_PROTOCOL_ICMP) return 0U;

	/*
	 * ICMP header starts after IPv4 header.
	 */
	icmp_offset = ip_offset + ip_header_len;

	if((icmp_offset + ETHNET_ICMP_HEADER_LEN) > RequestLen) return 0U;

	if(pRequest[icmp_offset + ETHNET_ICMP_TYPE_OFFSET] != ETHNET_ICMP_TYPE_ECHO_REQUEST) return 0U;
	if(pRequest[icmp_offset + ETHNET_ICMP_CODE_OFFSET] != ETHNET_ICMP_CODE_ECHO) return 0U;

	/*
	 * Ethernet frame length without FCS.
	 */
	frame_copy_len = ETHNET_HEADER_LEN + (uint32_t)ip_total_len;
	reply_len = frame_copy_len;

	if(reply_len < 60U){
		reply_len = 60U;
	}

	if(reply_len > OutSize) return 0U;

	/*
	 * Clear output frame.
	 */
	for(uint32_t i = 0U; i < reply_len; i++){
		pOut[i] = 0U;
	}

	/*
	 * Copy request frame first.
	 * Then modify Ethernet, IPv4 and ICMP fields.
	 */
	for(uint32_t i = 0U; i < frame_copy_len; i++){
		pOut[i] = pRequest[i];
	}

	/*
	 * Ethernet Header
	 *
	 * Dst MAC -> Request Src MAC
	 * Src MAC -> Local MAC
	 * EtherType -> IPv4
	 */
	for(uint8_t i = 0U; i < ETHNET_MAC_ADDR_LEN; i++){
		pOut[ETHNET_DST_MAC_OFFSET + i] = pRequest[ETHNET_SRC_MAC_OFFSET + i];
		pOut[ETHNET_SRC_MAC_OFFSET + i] = myMAC[i];
	}

	WriteU16BE(pOut, ETHNET_TYPE_OFFSET, ETHNET_ETHERTYPE_IPV4);

	/*
	 * IPv4 Header
	 *
	 * Dst IP -> Request Src IP
	 * Src IP -> Local IP
	 */
	for(uint8_t i = 0U; i < ETHNET_IPV4_ADDR_LEN; i++){
		pOut[ip_offset + ETHNET_IPV4_DST_ADDR_OFFSET + i] = pRequest[ip_offset + ETHNET_IPV4_SRC_ADDR_OFFSET + i];
		pOut[ip_offset + ETHNET_IPV4_SRC_ADDR_OFFSET + i] = myIP[i];
	}

	/*
	 * TTL can be reset for generated reply.
	 */
	pOut[ip_offset + ETHNET_IPV4_TTL_OFFSET] = 64U;

	/*
	 * Recalculate IPv4 header checksum.
	 */
	pOut[ip_offset + ETHNET_IPV4_CHECKSUM_OFFSET] = 0U;
	pOut[ip_offset + ETHNET_IPV4_CHECKSUM_OFFSET + 1U] = 0U;

	WriteU16BE(pOut, (ip_offset + ETHNET_IPV4_CHECKSUM_OFFSET),	ETHNET_CalcCheckSum16(&pOut[ip_offset], ip_header_len));

	/*
	 * ICMP Header
	 *
	 * Type = Echo Reply
	 * Code = 0
	 * Identifier, sequence number and payload are kept from request.
	 */
	pOut[icmp_offset + ETHNET_ICMP_TYPE_OFFSET] = ETHNET_ICMP_TYPE_ECHO_REPLY;
	pOut[icmp_offset + ETHNET_ICMP_CODE_OFFSET] = ETHNET_ICMP_CODE_ECHO;

	pOut[icmp_offset + ETHNET_ICMP_CHECKSUM_OFFSET] = 0U;
	pOut[icmp_offset + ETHNET_ICMP_CHECKSUM_OFFSET + 1U] = 0U;

	WriteU16BE(pOut, (icmp_offset + ETHNET_ICMP_CHECKSUM_OFFSET), ETHNET_CalcCheckSum16(&pOut[icmp_offset], (uint32_t)ip_total_len - ip_header_len));

	return reply_len;
}

/****************************************************************************
 * @fn				- ETHNET_IsIPv4PacketForIP
 *
 * @brief			- This function checks whether an Ethernet frame contains
 * 					  an IPv4 packet addressed to the given IPv4 address
 *
 * @param[in]		- pointer to raw Ethernet frame buffer
 * @param[in]		- raw Ethernet frame length
 * @param[in]		- local IPv4 address
 *
 * @return			- 1 if IPv4 destination address matches, 0 otherwise
 *
 * @Note			- This function checks only Ethernet II + IPv4 destination IP.
 * 					- IPv4 checksum validation can be added separately.
 *					-
 *
 */
uint8_t ETHNET_IsIPv4PacketForIP(const uint8_t *pFrame, uint32_t FrameLen, const uint8_t ip[ETHNET_IPV4_ADDR_LEN]){
	uint16_t ethertype;
	uint8_t ip_ver;
	uint8_t ip_header_len;
	uint32_t ip_offset;

	if((pFrame == 0U) || (FrameLen == 0U) || (ip == 0U)) return 0U;
	if(FrameLen < (ETHNET_HEADER_LEN + ETHNET_IPV4_MIN_HEADER_LEN)) return 0U;

	ethertype = ETHNET_GetEtherType(pFrame, FrameLen);

	if(ethertype != ETHNET_ETHERTYPE_IPV4) return 0U;

	ip_offset = ETHNET_HEADER_LEN;

	ip_ver = (uint8_t)(pFrame[ip_offset + ETHNET_IPV4_VER_IHL_OFFSET] >> 4U);
	ip_header_len = (uint8_t)((pFrame[ip_offset + ETHNET_IPV4_VER_IHL_OFFSET] & 0x0FU) * 4U);

	if(ip_ver != 4U) return 0U;
	if(ip_header_len < ETHNET_IPV4_MIN_HEADER_LEN) return 0U;
	if(FrameLen < (ETHNET_HEADER_LEN + ip_header_len)) return 0U;

	for(uint8_t i = 0U; i < ETHNET_IPV4_ADDR_LEN; i++){
		if(pFrame[ip_offset + ETHNET_IPV4_DST_ADDR_OFFSET + i] != ip[i]){
			return 0U;
		}
	}

	return 1U;

}

/****************************************************************************
 * @fn				- ETHNET_CalcChecksum16
 *
 * @brief			- This function calculates the standard 16-bit one's
 * 					  complement checksum
 *
 * @param[in]		- pointer to input data buffer
 * @param[in]		- data length in bytes
 * @param[in]		-
 *
 * @return			- calculated 16-bit checksum value
 *
 * @Note			- This checksum is used by IPv4 header and ICMP packets.
 * 					- Checksum field must be cleared before calculation.
 *					- Data is interpreted as big-endian 16-bit words.
 *
 */
uint16_t ETHNET_CalcCheckSum16(const uint8_t * pData, uint32_t Len){
	if((pData == 0) || (Len == 0)) return 0U;

	uint32_t sum = 0U;
	uint16_t word;

	while(Len > 1U){
		word = ((uint16_t)pData[0] << 8U) | ((uint16_t)pData[1]);
		sum += word;

		pData += 2U;
		Len -= 2U;
	}

	if(Len > 0U){
		word = ((uint16_t)pData[0] << 8U);
		sum += word;
	}

	while((sum >> 16U) != 0U){
		sum = (sum & 0xFFFFU) + (sum >> 16U);
	}

	return (uint16_t)(~sum);
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
