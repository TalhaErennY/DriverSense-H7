/*
 * stm32h7sxx_eth_driver.h
 *
 *  Created on: 7 Haz 2026
 *      Author: talha
 */

#ifndef INC_STM32H7SXX_ETH_DRIVER_H_
#define INC_STM32H7SXX_ETH_DRIVER_H_

#include "stm32h7sxx.h"
#include <stdint.h>

/*
 * @PHY_REGISTER_ADDRESSES
 * LAN8742A PHY register address selection macros
 *
 * These are standard Clause 22 registers inside the PHY.
 */
#define PHY_REG_BCR                    0U
#define PHY_REG_BSR                    1U
#define PHY_REG_ID1                    2U
#define PHY_REG_ID2                    3U

/*
 * @PHY_BSR_BITS
 * PHY Basic Status Register bit selection macros
 */
#define PHY_BSR_EXTENDED_CAPABILITY		(1U << 0)
#define PHY_BSR_JABBER_DETECT			(1U << 1)
#define PHY_BSR_LINK_STATUS				(1U << 2)
#define PHY_BSR_AUTO_NEG_ABILITY		(1U << 3)
#define PHY_BSR_REMOTE_FAULT			(1U << 4)
#define PHY_BSR_AUTO_NEG_COMPLETE		(1U << 5)
#define PHY_BSR_10BASE_T_HALF			(1U << 11)
#define PHY_BSR_10BASE_T_FULL			(1U << 12)
#define PHY_BSR_100BASE_TX_HALF			(1U << 13)
#define PHY_BSR_100BASE_TX_FULL			(1U << 14)

/*
 * @ETH_PHY_ADDRESS
 * Default LAN8742A PHY address
 *
 * On many Nucleo Ethernet boards this is 0.
 * If it does not work, ETH_PHY_FindAddress() scans all addresses from 0 to 31.
 */
#define LAN8742A_PHY_ADDR              0U

/*
 * @ETH_MDIO_OPERATION_COMMANDS
 * ETH MACMDIOAR operation command selection macros
 *
 * GOC[1:0]:
 * 01 -> Write
 * 11 -> Read
 */
#define ETH_MDIO_GOC_WRITE             1U
#define ETH_MDIO_GOC_READ              3U

/*
 * @ETH_MDIO_CLOCK_RANGE
 * MDIO clock range configuration macro
 */
#define ETH_MDIO_CSR_CLOCK_RANGE       4U

/*
 * @ETH_TIMEOUT_VALUE
 * Ethernet timeout value for blocking wait loops
 */
#define ETH_TIMEOUT_VALUE				1000000UL

/*
 * @ETH_DMA_CONFIGURATION
 * Ethernet RX/TX DMA descriptor and buffer configuration macros
 */
#define ETH_RX_DESC_CNT                 4U
#define ETH_RX_BUF_SIZE                 1536U

#define ETH_TX_DESC_CNT                 4U
#define ETH_TX_BUF_SIZE                 1536U

/*
 * Ethernet DMA descriptor
 *
 * Normal descriptor has 4 words.
 */
typedef struct{
	__vo uint32_t DESC0;
	__vo uint32_t DESC1;
	__vo uint32_t DESC2;
	__vo uint32_t DESC3;
}ETH_DMADesc_t;

/*
 * @ETH_MAC_ADDRESS_LENGTH
 * Ethernet MAC address length macro
 */
#define ETH_MAC_ADDR_LEN				6U

#define ETH_CACHE_LINE_SIZE             32U

#define ETH_ALIGN_DOWN(addr)            ((addr) & ~(ETH_CACHE_LINE_SIZE - 1U))
#define ETH_ALIGN_UP(size)              (((size) + ETH_CACHE_LINE_SIZE - 1U) & ~(ETH_CACHE_LINE_SIZE - 1U))

/******************************************************************************************************
 * 								APIs Supported by This Driver
 *       For more information about the APIs, check the function definitions in stm32h7sxx_eth_driver.c
 ******************************************************************************************************/

/*
 * Peripheral Clock / Interface / Reset Setup
 */
void ETH_PeriClockControl(uint8_t ENorDI);
void ETH_GPIO_RMII_Init(void);
void ETH_SelectRMII(void);
uint8_t ETH_SoftwareReset(void);

/*
 * MPU / Cache Configuration
 */
void ETH_MPU_ConfigNonCacheable(void);

/*
 * MAC Configuration
 */
void ETH_SetMACAddress(const uint8_t *pMACAddr);
void ETH_MAC_FilterConfig(uint32_t Value, uint32_t FilterMask);
void ETH_MAC_OPModeConfig(uint32_t Value, uint32_t ModeMask);

/*
 * MTL Configuration
 */
void ETH_MTL_RXQueueConfig(uint32_t Value, uint32_t ModeMask);
void ETH_MTL_TXQueueConfig(uint32_t Value, uint32_t ModeMask);

/*
 * DMA TX Descriptor / TX Path Configuration
 */
void ETH_TXDesc_Init(void);
void ETH_DMA_TXConfig(void);
void ETH_DMA_TXStart(void);
uint8_t ETH_SendRawFrame(const uint8_t *pFrame, uint32_t FrameLength);

/*
 * DMA RX Descriptor / RX Path Configuration
 */
void ETH_RXDesc_Init(void);
void ETH_DMA_RXConfig(void);
void ETH_DMA_RXStart(void);
uint8_t ETH_ReadRawFrame(uint8_t **ppFrame, uint32_t *pLength);
void ETH_ReleaseRXDescriptor(void);

/*
 * PHY / MDIO Register Access
 */
uint16_t ETH_PHY_Read(uint8_t PHYAddr, uint8_t RegAddr);
void ETH_PHY_Write(uint8_t PHYAddr, uint8_t RegAddr, uint16_t Value);

/*
 * PHY Status / Link Helper Functions
 */
uint8_t ETH_PHY_FindAddress(uint8_t *pPHYAddr);
uint8_t ETH_PHY_IsLinkUp(uint8_t PHYAddr);
uint8_t ETH_PHY_IsAutoNegComplete(uint8_t PHYAddr);
uint8_t ETH_PHY_WaitForLink(uint8_t PHYAddr, uint32_t Timeout);
uint8_t ETH_PHY_WaitAutoNegotiation(uint8_t PHYAddr, uint32_t Timeout);

/*
 * Debug Helper and Test Functions
 */
ETH_DMADesc_t* ETH_GetRXDescBase(void);
uint8_t* ETH_GetRXBufferBase(void);
uint32_t ETH_GetRXCurrentDescIndex(void);
uint32_t ETH_GetRXDesc3(uint32_t desc_index);
void ETH_MAC_FilterConfigForTest(void);
void ETH_MAC_RXEnable(void);
void ETH_MTL_RXQueueConfigRSF(void);

#endif /* INC_STM32H7SXX_ETH_DRIVER_H_ */
