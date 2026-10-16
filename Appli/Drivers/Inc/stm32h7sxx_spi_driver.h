/*
 * stm32h7sxx_spi_driver.h
 *
 *  Created on: 7 Eki 2026
 *      Author: talha
 */

#ifndef INC_STM32H7SXX_SPI_DRIVER_H_
#define INC_STM32H7SXX_SPI_DRIVER_H_

#include "stm32h7sxx.h"
#include <stdint.h>

/*
 * Configuration Structure for SPIx Peripheral
 */
typedef struct{
	uint8_t SPI_DeviceMode;
	uint8_t SPI_BusConfig;
	uint8_t SPI_SclkSpeed;
	uint8_t SPI_DataSize;
	uint8_t SPI_CPOL;
	uint8_t SPI_CPHA;
	uint8_t SPI_SSM;
} SPI_Config_t;

/*
 * Handle Structure for SPIx Peripheral
 */
typedef struct{
	SPI_RegDef_t *pSPIx;
	SPI_Config_t SPIConfig;
} SPI_Handle_t;

/*
 * @SPI_DEVICE_MODES
 * SPI device mode selection macros
 */
#define SPI_DEVICE_MODE_SLAVE           0U
#define SPI_DEVICE_MODE_MASTER          1U

/*
 * @SPI_BUS_CONFIGS
 * SPI communication mode selection macros
 */
#define SPI_BUS_CONFIG_FULL_DUPLEX      0U
#define SPI_BUS_CONFIG_SIMPLEX_TX       1U
#define SPI_BUS_CONFIG_SIMPLEX_RX       2U
#define SPI_BUS_CONFIG_HALF_DUPLEX      3U

/*
 * @SPI_PROTOCOLS
 * SPI protocol selection macros
 */
#define SPI_PROTOCOL_MOTOROLA           0U
#define SPI_PROTOCOL_TI                 1U

/*
 * @SPI_DATA_SIZES
 * SPI data frame size selection macros
 *
 * DSIZE register value = Number of bits - 1
 */
#define SPI_DATA_SIZE_4BIT              3U
#define SPI_DATA_SIZE_5BIT              4U
#define SPI_DATA_SIZE_6BIT              5U
#define SPI_DATA_SIZE_7BIT              6U
#define SPI_DATA_SIZE_8BIT              7U
#define SPI_DATA_SIZE_9BIT              8U
#define SPI_DATA_SIZE_10BIT             9U
#define SPI_DATA_SIZE_11BIT             10U
#define SPI_DATA_SIZE_12BIT             11U
#define SPI_DATA_SIZE_13BIT             12U
#define SPI_DATA_SIZE_14BIT             13U
#define SPI_DATA_SIZE_15BIT             14U
#define SPI_DATA_SIZE_16BIT             15U
#define SPI_DATA_SIZE_17BIT             16U
#define SPI_DATA_SIZE_18BIT             17U
#define SPI_DATA_SIZE_19BIT             18U
#define SPI_DATA_SIZE_20BIT             19U
#define SPI_DATA_SIZE_21BIT             20U
#define SPI_DATA_SIZE_22BIT             21U
#define SPI_DATA_SIZE_23BIT             22U
#define SPI_DATA_SIZE_24BIT             23U
#define SPI_DATA_SIZE_25BIT             24U
#define SPI_DATA_SIZE_26BIT             25U
#define SPI_DATA_SIZE_27BIT             26U
#define SPI_DATA_SIZE_28BIT             27U
#define SPI_DATA_SIZE_29BIT             28U
#define SPI_DATA_SIZE_30BIT             29U
#define SPI_DATA_SIZE_31BIT             30U
#define SPI_DATA_SIZE_32BIT             31U

/*
 * @SPI_FIFO_THRESHOLDS
 * SPI FIFO threshold selection macros
 */
#define SPI_FIFO_THRESHOLD_1DATA        0U
#define SPI_FIFO_THRESHOLD_2DATA        1U
#define SPI_FIFO_THRESHOLD_3DATA        2U
#define SPI_FIFO_THRESHOLD_4DATA        3U
#define SPI_FIFO_THRESHOLD_5DATA        4U
#define SPI_FIFO_THRESHOLD_6DATA        5U
#define SPI_FIFO_THRESHOLD_7DATA        6U
#define SPI_FIFO_THRESHOLD_8DATA        7U
#define SPI_FIFO_THRESHOLD_9DATA        8U
#define SPI_FIFO_THRESHOLD_10DATA       9U
#define SPI_FIFO_THRESHOLD_11DATA       10U
#define SPI_FIFO_THRESHOLD_12DATA       11U
#define SPI_FIFO_THRESHOLD_13DATA       12U
#define SPI_FIFO_THRESHOLD_14DATA       13U
#define SPI_FIFO_THRESHOLD_15DATA       14U
#define SPI_FIFO_THRESHOLD_16DATA       15U

/*
 * @SPI_SCLK_SPEEDS
 * SPI master baud rate prescaler selection macros
 */
#define SPI_SCLK_SPEED_DIV2             0U
#define SPI_SCLK_SPEED_DIV4             1U
#define SPI_SCLK_SPEED_DIV8             2U
#define SPI_SCLK_SPEED_DIV16            3U
#define SPI_SCLK_SPEED_DIV32            4U
#define SPI_SCLK_SPEED_DIV64            5U
#define SPI_SCLK_SPEED_DIV128           6U
#define SPI_SCLK_SPEED_DIV256           7U

/*
 * @SPI_CLOCK_POLARITIES
 * SPI clock polarity selection macros
 */
#define SPI_CPOL_LOW                    0U
#define SPI_CPOL_HIGH                   1U

/*
 * @SPI_CLOCK_PHASES
 * SPI clock phase selection macros
 */
#define SPI_CPHA_FIRST_EDGE             0U
#define SPI_CPHA_SECOND_EDGE            1U

/*
 * @SPI_DATA_ORDERS
 * SPI data order selection macros
 */
#define SPI_DATA_ORDER_MSB_FIRST        0U
#define SPI_DATA_ORDER_LSB_FIRST        1U

/*
 * @SPI_SSM_MODES
 * SPI slave select management selection macros
 */
#define SPI_SSM_DI 						0U
#define SPI_SSM_EN		                1U

/*
 * @SPI_SSI_LEVELS
 * SPI internal slave select level macros
 */
#define SPI_SSI_ACTIVE                  0U
#define SPI_SSI_INACTIVE                1U

/*
 * @SPI_STATUS_FLAGS
 * SPI SR flag selection macros
 */
#define SPI_FLAG_RXP                    (1U << SPI_SR_RXP)
#define SPI_FLAG_TXP                    (1U << SPI_SR_TXP)
#define SPI_FLAG_DXP                    (1U << SPI_SR_DXP)
#define SPI_FLAG_EOT                    (1U << SPI_SR_EOT)
#define SPI_FLAG_TXTF                   (1U << SPI_SR_TXTF)
#define SPI_FLAG_UDR                    (1U << SPI_SR_UDR)
#define SPI_FLAG_OVR                    (1U << SPI_SR_OVR)
#define SPI_FLAG_CRCE                   (1U << SPI_SR_CRCE)
#define SPI_FLAG_TIFRE                  (1U << SPI_SR_TIFRE)
#define SPI_FLAG_MODF                   (1U << SPI_SR_MODF)
#define SPI_FLAG_SUSP                   (1U << SPI_SR_SUSP)
#define SPI_FLAG_TXC                    (1U << SPI_SR_TXC)

/*
 * @SPI_INTERRUPT_SOURCES
 * SPI interrupt source selection macros
 */
#define SPI_IT_RXP                      (1U << SPI_IER_RXPIE)
#define SPI_IT_TXP                      (1U << SPI_IER_TXPIE)
#define SPI_IT_DXP                      (1U << SPI_IER_DXPIE)
#define SPI_IT_EOT                      (1U << SPI_IER_EOTIE)
#define SPI_IT_TXTF                     (1U << SPI_IER_TXTFIE)
#define SPI_IT_UDR                      (1U << SPI_IER_UDRIE)
#define SPI_IT_OVR                      (1U << SPI_IER_OVRIE)
#define SPI_IT_CRCE                     (1U << SPI_IER_CRCEIE)
#define SPI_IT_TIFRE                    (1U << SPI_IER_TIFREIE)
#define SPI_IT_MODF                     (1U << SPI_IER_MODFIE)

/*
 * @SPI_RETURN_STATUS
 * SPI return status macros
 */
#define SPI_OK                          0U
#define SPI_ERROR_TIMEOUT               1U
#define SPI_ERROR_OVR                   2U
#define SPI_ERROR_MODF                  3U
#define SPI_ERROR_CRC                   4U
#define SPI_ERROR_INVALID_ARG           5U

/*
 * @SPI_TIMEOUT_VALUE
 * Software timeout value for blocking wait loops
 */
#define SPI_TIMEOUT                     100000U

#define SPI_DUMMY_BYTE		0xFFU

/******************************************************************************************************
 * 								APIs Supported by This Driver
 *       For more information about the APIs, check the function definitions in stm32h7sxx_spi_driver.c
 ******************************************************************************************************/
/*
 * Peripheral Clock Setup
 */
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t ENorDI);

/*
 * SPI Enable-Disable
 */
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t ENorDI);

/*
 * Communication Control
 */
void SPI_StartCommunication(SPI_RegDef_t *pSPIx);

/*
 * Init - DeInit
 */
void SPI_Init(SPI_Handle_t *pSPIHandle);
void SPI_DeInit(SPI_RegDef_t *pSPIx);

/*
 * Data Send and Receive
 */
uint8_t SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len);
uint8_t SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len);
uint8_t SPI_Transfer(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint8_t *pRxBuffer, uint32_t Len);
uint8_t SPI_TransferByte(SPI_RegDef_t *pSPIx, uint8_t TxData, uint8_t *pRxData);

/*
 * Get Status (True-False)
 */
uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName);

/*
 * IRQ Configuration and ISR Handling
 */
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t ENorDI);
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority);
void SPI_IRQHandling(SPI_Handle_t *pSPIHandle);

/*
 * Easy Access Functions
 */
void SPI2S1_GPIOInits(void);

#endif /* INC_STM32H7SXX_SPI_DRIVER_H_ */
