/*
 * 014_SPI_Loopback.c
 *
 *  Created on: 7 Eki 2026
 *      Author: talha
 */

#include "stm32h7sxx.h"
#include "stm32h7sxx_gpio_driver.h"
#include "stm32h7sxx_spi_driver.h"
#include "stm32h7sxx_rcc_driver.h"

volatile uint8_t dbg_tx = 0U;
volatile uint8_t dbg_rx = 0U;
volatile uint8_t dbg_status = 0U;

void Test_014_SPI_Loopback(void){
	SPI_Handle_t SPI1Handle;

	SPI1Handle.pSPIx = SPI1;

	SPI1Handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI1Handle.SPIConfig.SPI_BusConfig  = SPI_BUS_CONFIG_FULL_DUPLEX;
	SPI1Handle.SPIConfig.SPI_SclkSpeed  = SPI_SCLK_SPEED_DIV256;
	SPI1Handle.SPIConfig.SPI_DataSize   = SPI_DATA_SIZE_8BIT;
	SPI1Handle.SPIConfig.SPI_CPOL       = SPI_CPOL_LOW;
	SPI1Handle.SPIConfig.SPI_CPHA       = SPI_CPHA_FIRST_EDGE;
	SPI1Handle.SPIConfig.SPI_SSM        = SPI_SSM_EN;

	/*
	 * GPIO configuration must be done here.
	 * SCK  -> SPI1_SCK
	 * MOSI -> SPI1_MOSI
	 * MISO -> SPI1_MISO
	 */
	SPI2S1_GPIOInits();

	RCC_SystemClock_Config_280MHz_HSI();

	SPI_Init(&SPI1Handle);

	dbg_tx = 0xA5U;

	dbg_status = SPI_TransferByte(SPI1, dbg_tx, (uint8_t *)&dbg_rx);

	while(1);

}
