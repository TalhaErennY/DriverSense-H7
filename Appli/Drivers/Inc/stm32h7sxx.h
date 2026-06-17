/*
 * @file    stm32h7sxx.h
 * @brief   Device-level register definitions for STM32H7Sxx MCUs.
 *
 * This header provides base addresses, register layout definitions,
 * peripheral instances, clock-control helper macros, bit positions, and
 * IRQ number definitions used by the bare-metal driver layer.
 *
 * Target family : STM32H7Rx/7Sx
 * Reference     : RM0477, DS14359, PM0253
 *
 * @author  Talha
 * @date    16-May-2026
 */

#ifndef INC_STM32H7SXX_H_
#define INC_STM32H7SXX_H_

#include <stdint.h>

/* Register access qualifier for memory-mapped peripherals. */
#define __vo volatile

/* -------------------------------------------------------------------------- */
/* Cortex-M7 NVIC definitions                                                 */
/* -------------------------------------------------------------------------- */

/* NVIC core peripheral register base addresses. */
#define NVIC_ISER_BASEADDR              (0xE000E100UL)
#define NVIC_ICER_BASEADDR              (0xE000E180UL)
#define NVIC_IPR_BASEADDR               (0xE000E400UL)

/* NVIC interrupt enable registers are exposed as 32-bit register arrays. */
#define NVIC_ISER                       ((__vo uint32_t*)NVIC_ISER_BASEADDR)
#define NVIC_ICER                       ((__vo uint32_t*)NVIC_ICER_BASEADDR)

/* NVIC interrupt priority registers are exposed as byte-addressable fields. */
#define NVIC_IPR                        ((__vo uint8_t*)NVIC_IPR_BASEADDR)

/* STM32H7 NVIC implements 4 priority bits: logical range 0..15. */
#define NVIC_PRIO_BITS_IMPLEMENTED      4U
#define NVIC_PRIORITY_SHIFT             (8U - NVIC_PRIO_BITS_IMPLEMENTED)
#define NVIC_PRIORITY_MAX               ((1U << NVIC_PRIO_BITS_IMPLEMENTED) - 1U)

/* -------------------------------------------------------------------------- */
/* Generic bit manipulation macros                                            */
/* -------------------------------------------------------------------------- */

/* Generic register bit manipulation helpers. MASK and VAL must be pre-shifted. */
#define SET_BIT(REG, MASK)              ((REG) |= (MASK))
#define CLEAR_BIT(REG, MASK)            ((REG) &= ~(MASK))
#define READ_BIT(REG, MASK)             ((REG) & (MASK))
#define MODIFY_REG(REG, MASK, VAL)      ((REG) = (((REG) & ~(MASK)) | ((VAL) & (MASK))))

/* -------------------------------------------------------------------------- */
/* Memory base addresses                                                      */
/* -------------------------------------------------------------------------- */

/* Device memory region base addresses. */

#define FLASH_BASEADDR					(0x08000000UL)
#define DTCM_RAM_BASEADDR				(0x20000000UL)
#define AXI_SRAM1_BASEADDR				(0x24000000UL)
#define AXI_SRAM2_BASEADDR				(0x24020000UL)
#define AXI_SRAM3_BASEADDR				(0x24040000UL)
#define AXI_SRAM4_BASEADDR				(0x24080000UL)
#define AXI_GFXMMU_REMAP_BASEADDR		(0x25000000UL)
#define AHB_SRAM1_BASEADDR				(0x30000000UL)
#define AHB_SRAM2_BASEADDR				(0x30004000UL)
#define BKUP_SRAM_BASEADDR				(0x38800000UL)

#define SRAM 							DTCM_RAM_BASEADDR

/* -------------------------------------------------------------------------- */
/* AHB/APB bus peripheral base addresses                                      */
/* -------------------------------------------------------------------------- */
#define PERIPH_BASEADDR					(0x40000000UL)

#define APB1_BASEADDR					PERIPH_BASEADDR
#define AHB1_BASEADDR					(0x40020000UL)
#define APB2_BASEADDR					(0x42000000UL)
#define AHB2_BASEADDR					(0x48000000UL)
#define AHB3_BASEADDR					(0x48020000UL)
#define APB5_BASEADDR					(0x50000000UL)
#define AHB5_BASEADDR					(0x52000000UL)
#define APB4_BASEADDR					(0x58000000UL)
#define AHB4_BASEADDR					(0x58020000UL)

/* -------------------------------------------------------------------------- */
/* APB1 peripheral base addresses                                             */
/* -------------------------------------------------------------------------- */
#define TIM2_BASEADDR					((APB1_BASEADDR) + 0x0000UL)
#define TIM3_BASEADDR					((APB1_BASEADDR) + 0x0400UL)
#define TIM4_BASEADDR					((APB1_BASEADDR) + 0x0800UL)
#define TIM5_BASEADDR					((APB1_BASEADDR) + 0x0C00UL)
#define TIM6_BASEADDR					((APB1_BASEADDR) + 0x1000UL)
#define TIM7_BASEADDR					((APB1_BASEADDR) + 0x1400UL)
#define TIM12_BASEADDR					((APB1_BASEADDR) + 0x1800UL)
#define TIM13_BASEADDR					((APB1_BASEADDR) + 0x1C00UL)
#define TIM14_BASEADDR					((APB1_BASEADDR) + 0x2000UL)
#define LPTIM1_BASEADDR					((APB1_BASEADDR) + 0x2400UL)
#define WWDG_BASEADDR					((APB1_BASEADDR) + 0x2C00UL)
#define SPI2S2_BASEADDR					((APB1_BASEADDR) + 0x3800UL)
#define SPI2S3_BASEADDR					((APB1_BASEADDR) + 0x3C00UL)
#define SPDIFRX1_BASEADDR				((APB1_BASEADDR) + 0x4000UL)
#define USART2_BASEADDR					((APB1_BASEADDR) + 0x4400UL)
#define USART3_BASEADDR					((APB1_BASEADDR) + 0x4800UL)
#define UART4_BASEADDR					((APB1_BASEADDR) + 0x4C00UL)
#define UART5_BASEADDR					((APB1_BASEADDR) + 0x5000UL)
#define I2C1_BASEADDR					((APB1_BASEADDR) + 0x5400UL)
#define I3C1_BASEADDR					((APB1_BASEADDR) + 0x5400UL)
#define I2C2_BASEADDR					((APB1_BASEADDR) + 0x5800UL)
#define I2C3_BASEADDR					((APB1_BASEADDR) + 0x5C00UL)
#define CEC_BASEADDR					((APB1_BASEADDR) + 0x6C00UL)
#define UART7_BASEADDR					((APB1_BASEADDR) + 0x7800UL)
#define UART8_BASEADDR					((APB1_BASEADDR) + 0x7C00UL)
#define CRS_BASEADDR					((APB1_BASEADDR) + 0x8400UL)
#define MDIOS_BASEADDR					((APB1_BASEADDR) + 0x9400UL)
#define FDCAN1_BASEADDR					((APB1_BASEADDR) + 0xA000UL)
#define FDCAN2_BASEADDR					((APB1_BASEADDR) + 0xA400UL)
#define FDCAN_MSG_RAM_BASEADDR			((APB1_BASEADDR) + 0xAC00UL)
#define UCPD1_BASEADDR					((APB1_BASEADDR) + 0xEC00UL)

/* -------------------------------------------------------------------------- */
/* AHB1 peripheral base addresses                                             */
/* -------------------------------------------------------------------------- */

#define GPDMA1_BASEADDR					((AHB1_BASEADDR) + 0x1000UL)
#define ADC1_ADC2_BASEADDR				((AHB1_BASEADDR) + 0x2000UL)
#define ETH1_BASEADDR					((AHB1_BASEADDR) + 0x8000UL)
#define ADF1_BASEADDR					((AHB1_BASEADDR) + 0xF000UL)
#define OTG_HS_BASEADDR					((AHB1_BASEADDR) + 0x20000UL)
#define OTG_FS_BASEADDR					((AHB1_BASEADDR) + 0x60000UL)

/* -------------------------------------------------------------------------- */
/* APB2 peripheral base addresses                                             */
/* -------------------------------------------------------------------------- */
#define TIM1_BASEADDR					((APB2_BASEADDR) + 0x0000UL)
#define USART1_BASEADDR					((APB2_BASEADDR) + 0x1000UL)
#define SPI2S1_BASEADDR					((APB2_BASEADDR) + 0x3000UL)
#define SPI4_BASEADDR					((APB2_BASEADDR) + 0x3400UL)
#define TIM15_BASEADDR					((APB2_BASEADDR) + 0x4000UL)
#define TIM16_BASEADDR					((APB2_BASEADDR) + 0x4400UL)
#define TIM17_BASEADDR					((APB2_BASEADDR) + 0x4800UL)
#define TIM9_BASEADDR					((APB2_BASEADDR) + 0x4C00UL)
#define SPI5_BASEADDR					((APB2_BASEADDR) + 0x5000UL)
#define SAI1_BASEADDR					((APB2_BASEADDR) + 0x5800UL)
#define SAI2_BASEADDR					((APB2_BASEADDR) + 0x5C00UL)

/* -------------------------------------------------------------------------- */
/* AHB2 peripheral base addresses                                             */
/* -------------------------------------------------------------------------- */
#define PSSI_BASEADDR					((AHB2_BASEADDR) + 0x0400UL)
#define SDMMC2_BASEADDR					((AHB2_BASEADDR) + 0x2400UL)
#define DLYB_SDMMC2_BASEADDR			((AHB2_BASEADDR) + 0x2800UL)
#define CORDIC_BASEADDR					((AHB2_BASEADDR) + 0x4400UL)

/* -------------------------------------------------------------------------- */
/* AHB3 peripheral base addresses                                             */
/* -------------------------------------------------------------------------- */
#define RNG_BASEADDR					((AHB3_BASEADDR) + 0x0000UL)
#define HASH_BASEADDR					((AHB3_BASEADDR) + 0x0400UL)
#define CRYP_BASEADDR					((AHB3_BASEADDR) + 0x0800UL)
#define SAES_BASEADDR					((AHB3_BASEADDR) + 0x1000UL)
#define PKA_RAM_BASEADDR				((AHB3_BASEADDR) + 0x2000UL)

/* -------------------------------------------------------------------------- */
/* APB5 peripheral base addresses                                             */
/* -------------------------------------------------------------------------- */
#define LTDC_BASEADDR					((APB5_BASEADDR) + 0x1000UL)
#define DCMIPP_BASEADDR					((APB5_BASEADDR) + 0x2000UL)
#define GFXTIM_BASEADDR					((APB5_BASEADDR) + 0x4000UL)

/* -------------------------------------------------------------------------- */
/* AHB5 peripheral base addresses                                             */
/* -------------------------------------------------------------------------- */
#define HPDMA1_BASEADDR					((AHB5_BASEADDR) + 0x0000UL)
#define DMA2D_BASEADDR					((AHB5_BASEADDR) + 0x1000UL)
#define FLASH_REG_BASEADDR				((AHB5_BASEADDR) + 0x2000UL)		// FLASH control-register block, not Flash memory
#define JPEG_BASEADDR					((AHB5_BASEADDR) + 0x3000UL)
#define FMC_BASEADDR					((AHB5_BASEADDR) + 0x4000UL)
#define XSPI1_BASEADDR					((AHB5_BASEADDR) + 0x5000UL)
#define SDMMC1_BASEADDR					((AHB5_BASEADDR) + 0x7000UL)
#define DLYB_SDMMC1_BASEADDR			((AHB5_BASEADDR) + 0x8000UL)
#define ECC_DIAG_D1_BASEADDR			((AHB5_BASEADDR) + 0x9000UL)
#define XSPI2_BASEADDR					((AHB5_BASEADDR) + 0xA000UL)
#define XSPIM1_BASEADDR					((AHB5_BASEADDR) + 0xB400UL)
#define MCE1_BASEADDR					((AHB5_BASEADDR) + 0xB800UL)
#define MCE2_BASEADDR					((AHB5_BASEADDR) + 0xBC00UL)
#define MCE3_BASEADDR					((AHB5_BASEADDR) + 0xC000UL)
#define GFXMMU_BASEADDR					((AHB5_BASEADDR) + 0x10000UL)
#define GPU2D_BASEADDR					((AHB5_BASEADDR) + 0x14000UL)
#define ICACHE_BASEADDR					((AHB5_BASEADDR) + 0x15000UL)

/* -------------------------------------------------------------------------- */
/* APB4 peripheral base addresses                                             */
/* -------------------------------------------------------------------------- */
#define EXTI_BASEADDR					((APB4_BASEADDR) + 0x0000UL)
#define SBS_BASEADDR					((APB4_BASEADDR) + 0x0400UL)
#define LPUART1_BASEADDR				((APB4_BASEADDR) + 0x0C00UL)
#define SPI2S6_BASEADDR					((APB4_BASEADDR) + 0x1400UL)
#define LPTIM2_BASEADDR					((APB4_BASEADDR) + 0x2400UL)
#define LPTIM3_BASEADDR					((APB4_BASEADDR) + 0x2800UL)
#define LPTIM4_BASEADDR					((APB4_BASEADDR) + 0x2C00UL)
#define LPTIM5_BASEADDR					((APB4_BASEADDR) + 0x3000UL)
#define VREFBUF_BASEADDR				((APB4_BASEADDR) + 0x3C00UL)
#define RTC_BASEADDR					((APB4_BASEADDR) + 0x4000UL)
#define TAMP_BASEADDR					((APB4_BASEADDR) + 0x4400UL)
#define IWDG_BASEADDR					((APB4_BASEADDR) + 0x4800UL)
#define DTS_BASEADDR					((APB4_BASEADDR) + 0x6800UL)

/* -------------------------------------------------------------------------- */
/* AHB4 peripheral base addresses                                             */
/* -------------------------------------------------------------------------- */
#define GPIOA_BASEADDR					((AHB4_BASEADDR) + 0x0000UL)
#define GPIOB_BASEADDR					((AHB4_BASEADDR) + 0x0400UL)
#define GPIOC_BASEADDR					((AHB4_BASEADDR) + 0x0800UL)
#define GPIOD_BASEADDR					((AHB4_BASEADDR) + 0x0C00UL)
#define GPIOE_BASEADDR					((AHB4_BASEADDR) + 0x1000UL)
#define GPIOF_BASEADDR					((AHB4_BASEADDR) + 0x1400UL)
#define GPIOG_BASEADDR					((AHB4_BASEADDR) + 0x1800UL)
#define GPIOH_BASEADDR					((AHB4_BASEADDR) + 0x1C00UL)
#define GPIOM_BASEADDR					((AHB4_BASEADDR) + 0x3000UL)
#define GPION_BASEADDR					((AHB4_BASEADDR) + 0x3400UL)
#define GPIOO_BASEADDR					((AHB4_BASEADDR) + 0x3800UL)
#define GPIOP_BASEADDR					((AHB4_BASEADDR) + 0x3C00UL)
#define RCC_BASEADDR					((AHB4_BASEADDR) + 0x4400UL)
#define PWR_BASEADDR					((AHB4_BASEADDR) + 0x4800UL)
#define CRC_BASEADDR					((AHB4_BASEADDR) + 0x4C00UL)
#define ECC_DIAG_D2_BASEADDR			((AHB4_BASEADDR) + 0x7000UL)

/* -------------------------------------------------------------------------- */
/* Peripheral register layout definitions                                     */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* ADC register layout                                                        */
/* -------------------------------------------------------------------------- */
/* ADC register map. */
typedef struct{
	__vo uint32_t ISR;			//0x000 ADC_ISR
	__vo uint32_t IER;			//0x004 ADC_IER
	__vo uint32_t CR;			//0x008 ADC_CR
	__vo uint32_t CFGR;			//0x00C ADC_CFGR
	__vo uint32_t CFGR2;			//0x010 ADC_CFGR2
	__vo uint32_t SMPR1;			//0x014 ADC_SMPR1
	__vo uint32_t SMPR2;			//0x018 ADC_SMPR2
	uint32_t reserved_1;			//0x01C ---
	__vo uint32_t TR1;			//0x020 ADC_TR1
	__vo uint32_t TR2;			//0x024 ADC_TR2
	__vo uint32_t TR3;			//0x028 ADC_TR3
	uint32_t reserved_2;			//0x02C ---
	__vo uint32_t SQR1;			//0x030 ADC_SQR1
	__vo uint32_t SQR2;			//0x034 ADC_SQR2
	__vo uint32_t SQR3;			//0x038 ADC_SQR3
	__vo uint32_t SQR4;			//0x03C ADC_SQR4
	__vo uint32_t DR;			//0x040 ADC_DR
	uint32_t reserved_3[2];		//0x044-0x048 ---
	__vo uint32_t JSQR;			//0x04C ADC_JSQR
	uint32_t reserved_4[20];		//0x050-0x09C ---
	__vo uint32_t AWD2CR;			//0x0A0 ADC_AWD2CR
	__vo uint32_t AWD3CR;			//0x0A4 ADC_AWD3CR
	uint32_t reserved_5[2];		//0x0A8-0x0AC ---
	__vo uint32_t DIFSEL;			//0x0B0 ADC_DIFSEL
	__vo uint32_t CALFACT;			//0x0B4 ADC_CALFACT
	uint32_t reserved_6[4];		//0x0B8-0x0C4 ---
	__vo uint32_t OR;			//0x0C8 ADC_OR
	uint32_t reserved_7[141];		//0x0CC-0x2FC ---
	__vo uint32_t CSR;			//0x300 ADC_CSR
	uint32_t reserved_8;			//0x304 ---
	__vo uint32_t CCR;			//0x308 ADC_CCR
	__vo uint32_t CDR;			//0x30C ADC_CDR
} ADC_RegDef_t;

/* -------------------------------------------------------------------------- */
/* ADF register layout                                                        */
/* -------------------------------------------------------------------------- */
/* ADF register map. */
typedef struct{
	__vo uint32_t GCR;			//0x000 ADF_GCR
	__vo uint32_t CKGCR;			//0x004 ADF_CKGCR
	uint32_t reserved_1[30];		//0x008-0x07C ---
	__vo uint32_t SITF0CR;			//0x080 ADF_SITF0CR
	__vo uint32_t BSMX0CR;			//0x084 ADF_BSMX0CR
	__vo uint32_t DFLT0CR;			//0x088 ADF_DFLT0CR
	__vo uint32_t DFLT0CICR;			//0x08C ADF_DFLT0CICR
	__vo uint32_t DFLT0RSFR;			//0x090 ADF_DFLT0RSFR
	uint32_t reserved_2[4];		//0x094-0x0A0 ---
	__vo uint32_t DLY0CR;			//0x0A4 ADF_DLY0CR
	uint32_t reserved_3;			//0x0A8 ---
	__vo uint32_t DFLT0IER;			//0x0AC ADF_DFLT0IER
	__vo uint32_t DFLT0ISR;			//0x0B0 ADF_DFLT0ISR
	uint32_t reserved_4;			//0x0B4 ---
	__vo uint32_t SADCR;			//0x0B8 ADF_SADCR
	__vo uint32_t SADCFGR;			//0x0BC ADF_SADCFGR
	__vo uint32_t SADSDLVR;			//0x0C0 ADF_SADSDLVR
	__vo uint32_t SADANLVR;			//0x0C4 ADF_SADANLVR
	uint32_t reserved_5[10];		//0x0C8-0x0EC ---
	__vo uint32_t DFLT0DR;			//0x0F0 ADF_DFLT0DR
} ADF_RegDef_t;

/* -------------------------------------------------------------------------- */
/* CEC register layout                                                        */
/* -------------------------------------------------------------------------- */
/* CEC register map. */
typedef struct{
	__vo uint32_t CR;			//0x000 CEC_CR
	__vo uint32_t CFGR;			//0x004 CEC_CFGR
	__vo uint32_t TXDR;			//0x008 CEC_TXDR
	__vo uint32_t RXDR;			//0x00C CEC_RXDR
	__vo uint32_t ISR;			//0x010 CEC_ISR
	__vo uint32_t IER;			//0x014 CEC_IER
} CEC_RegDef_t;

/* -------------------------------------------------------------------------- */
/* CORDIC register layout                                                     */
/* -------------------------------------------------------------------------- */
/* CORDIC register map. */
typedef struct{
	__vo uint32_t CSR;			//0x000 CORDIC_CSR
	__vo uint32_t WDATA;			//0x004 CORDIC_WDATA
	__vo uint32_t RDATA;			//0x008 CORDIC_RDATA
} CORDIC_RegDef_t;

/* -------------------------------------------------------------------------- */
/* CRC register layout                                                        */
/* -------------------------------------------------------------------------- */
/* CRC register map. */
typedef struct{
	__vo uint32_t DR;			//0x000 CRC_DR
	__vo uint32_t IDR;			//0x004 CRC_IDR
	__vo uint32_t CR;			//0x008 CRC_CR
	uint32_t reserved_1;			//0x00C ---
	__vo uint32_t INIT;			//0x010 CRC_INIT
	__vo uint32_t POL;			//0x014 CRC_POL
} CRC_RegDef_t;

/* -------------------------------------------------------------------------- */
/* CRS register layout                                                        */
/* -------------------------------------------------------------------------- */
/* CRS register map. */
typedef struct{
	__vo uint32_t CR;			//0x000 CRS_CR
	__vo uint32_t CFGR;			//0x004 CRS_CFGR
	__vo uint32_t ISR;			//0x008 CRS_ISR
	__vo uint32_t ICR;			//0x00C CRS_ICR
} CRS_RegDef_t;

/* -------------------------------------------------------------------------- */
/* CRYP register layout                                                       */
/* -------------------------------------------------------------------------- */
/* CRYP register map. */
typedef struct{
	__vo uint32_t CR;			//0x000 CRYP_CR
	__vo uint32_t SR;			//0x004 CRYP_SR
	__vo uint32_t DINR;			//0x008 CRYP_DINR
	__vo uint32_t DOUTR;			//0x00C CRYP_DOUTR
	__vo uint32_t DMACR;			//0x010 CRYP_DMACR
	__vo uint32_t IMSCR;			//0x014 CRYP_IMSCR
	__vo uint32_t RISR;			//0x018 CRYP_RISR
	__vo uint32_t MISR;			//0x01C CRYP_MISR
	__vo uint32_t K0LR;			//0x020 CRYP_K0LR
	__vo uint32_t K0RR;			//0x024 CRYP_K0RR
	__vo uint32_t K1LR;			//0x028 CRYP_K1LR
	__vo uint32_t K1RR;			//0x02C CRYP_K1RR
	__vo uint32_t K2LR;			//0x030 CRYP_K2LR
	__vo uint32_t K2RR;			//0x034 CRYP_K2RR
	__vo uint32_t K3LR;			//0x038 CRYP_K3LR
	__vo uint32_t K3RR;			//0x03C CRYP_K3RR
	__vo uint32_t IV0LR;			//0x040 CRYP_IV0LR
	__vo uint32_t IV0RR;			//0x044 CRYP_IV0RR
	__vo uint32_t IV1LR;			//0x048 CRYP_IV1LR
	__vo uint32_t IV1RR;			//0x04C CRYP_IV1RR
} CRYP_RegDef_t;

/* -------------------------------------------------------------------------- */
/* DCMIPP register layout                                                     */
/* -------------------------------------------------------------------------- */
/* DCMIPP register map. */
typedef struct{
	__vo uint32_t IPGR1;			//0x000 DCMIPP_IPGR1
	__vo uint32_t IPGR2;			//0x004 DCMIPP_IPGR2
	__vo uint32_t IPGR3;			//0x008 DCMIPP_IPGR3
	uint32_t reserved_1[4];		//0x00C-0x018 ---
	__vo uint32_t IPGR8;			//0x01C DCMIPP_IPGR8
	uint32_t reserved_2[57];		//0x020-0x100 ---
	__vo uint32_t PRCR;			//0x104 DCMIPP_PRCR
	__vo uint32_t PRESCR;			//0x108 DCMIPP_PRESCR
	__vo uint32_t PRESUR;			//0x10C DCMIPP_PRESUR
	uint32_t reserved_3[57];		//0x110-0x1F0 ---
	__vo uint32_t PRIER;			//0x1F4 DCMIPP_PRIER
	__vo uint32_t PRSR;			//0x1F8 DCMIPP_PRSR
	__vo uint32_t PRFCR;			//0x1FC DCMIPP_PRFCR
	uint32_t reserved_4;			//0x200 ---
	__vo uint32_t CMCR;			//0x204 DCMIPP_CMCR
	__vo uint32_t CMFRCR;			//0x208 DCMIPP_CMFRCR
	uint32_t reserved_5[121];		//0x20C-0x3EC ---
	__vo uint32_t CMIER;			//0x3F0 DCMIPP_CMIER
	__vo uint32_t CMSR1;			//0x3F4 DCMIPP_CMSR1
	__vo uint32_t CMSR2;			//0x3F8 DCMIPP_CMSR2
	__vo uint32_t CMFCR;			//0x3FC DCMIPP_CMFCR
	uint32_t reserved_6;			//0x400 ---
	__vo uint32_t P0FSCR;			//0x404 DCMIPP_P0FSCR
	uint32_t reserved_7[62];		//0x408-0x4FC ---
	__vo uint32_t P0FCTCR;			//0x500 DCMIPP_P0FCTCR
	__vo uint32_t P0SCSTR;			//0x504 DCMIPP_P0SCSTR
	__vo uint32_t P0SCSZR;			//0x508 DCMIPP_P0SCSZR
	uint32_t reserved_8[41];		//0x50C-0x5AC ---
	__vo uint32_t P0DCCNTR;			//0x5B0 DCMIPP_P0DCCNTR
	__vo uint32_t P0DCLMTR;			//0x5B4 DCMIPP_P0DCLMTR
	uint32_t reserved_9[2];		//0x5B8-0x5BC ---
	__vo uint32_t P0PPCR;			//0x5C0 DCMIPP_P0PPCR
	__vo uint32_t P0PPM0AR1;			//0x5C4 DCMIPP_P0PPM0AR1
	__vo uint32_t P0PPM0AR2;			//0x5C8 DCMIPP_P0PPM0AR2
	uint32_t reserved_10[10];		//0x5CC-0x5F0 ---
	__vo uint32_t P0IER;			//0x5F4 DCMIPP_P0IER
	__vo uint32_t P0SR;			//0x5F8 DCMIPP_P0SR
	__vo uint32_t P0FCR;			//0x5FC DCMIPP_P0FCR
	uint32_t reserved_11[64];		//0x600-0x6FC ---
	__vo uint32_t P0CFCTCR;			//0x700 DCMIPP_P0CFCTCR
	__vo uint32_t P0CSCSTR;			//0x704 DCMIPP_P0CSCSTR
	__vo uint32_t P0CSCSZR;			//0x708 DCMIPP_P0CSCSZR
	uint32_t reserved_12[45];		//0x70C-0x7BC ---
	__vo uint32_t P0CPPCR;			//0x7C0 DCMIPP_P0CPPCR
	__vo uint32_t P0CPPM0AR1;			//0x7C4 DCMIPP_P0CPPM0AR1
	__vo uint32_t P0CPPM0AR2;			//0x7C8 DCMIPP_P0CPPM0AR2
} DCMIPP_RegDef_t;

/* -------------------------------------------------------------------------- */
/* DLYB register layout                                                       */
/* -------------------------------------------------------------------------- */
/* DLYB register map. */
typedef struct{
	__vo uint32_t CR;			//0x000 DLYB_CR
	__vo uint32_t CFGR;			//0x004 DLYB_CFGR
} DLYB_RegDef_t;

/* -------------------------------------------------------------------------- */
/* DMA2D register layout                                                      */
/* -------------------------------------------------------------------------- */
/* DMA2D register map. */
typedef struct{
	__vo uint32_t CR;			//0x000 DMA2D_CR
	__vo uint32_t ISR;			//0x004 DMA2D_ISR
	__vo uint32_t IFCR;			//0x008 DMA2D_IFCR
	__vo uint32_t FGMAR;			//0x00C DMA2D_FGMAR
	__vo uint32_t FGOR;			//0x010 DMA2D_FGOR
	__vo uint32_t BGMAR;			//0x014 DMA2D_BGMAR
	__vo uint32_t BGOR;			//0x018 DMA2D_BGOR
	__vo uint32_t FGPFCCR;			//0x01C DMA2D_FGPFCCR
	__vo uint32_t FGCOLR;			//0x020 DMA2D_FGCOLR
	__vo uint32_t BGPFCCR;			//0x024 DMA2D_BGPFCCR
	__vo uint32_t BGCOLR;			//0x028 DMA2D_BGCOLR
	__vo uint32_t FGCMAR;			//0x02C DMA2D_FGCMAR
	__vo uint32_t BGCMAR;			//0x030 DMA2D_BGCMAR
	__vo uint32_t OPFCCR;			//0x034 DMA2D_OPFCCR
	__vo uint32_t OCOLR;			//0x038 DMA2D_OCOLR
	__vo uint32_t OMAR;			//0x03C DMA2D_OMAR
	__vo uint32_t OOR;			//0x040 DMA2D_OOR
	__vo uint32_t NLR;			//0x044 DMA2D_NLR
	__vo uint32_t LWR;			//0x048 DMA2D_LWR
	__vo uint32_t AMTCR;			//0x04C DMA2D_AMTCR
} DMA2D_RegDef_t;

/* -------------------------------------------------------------------------- */
/* DTS register layout                                                        */
/* -------------------------------------------------------------------------- */
/* DTS register map. */
typedef struct{
	__vo uint32_t CFGR1;			//0x000 DTS_CFGR1
	uint32_t reserved_1;			//0x004 ---
	__vo uint32_t T0VALR1;			//0x008 DTS_T0VALR1
	uint32_t reserved_2;			//0x00C ---
	__vo uint32_t RAMPVALR;			//0x010 DTS_RAMPVALR
	__vo uint32_t ITR1;			//0x014 DTS_ITR1
	uint32_t reserved_3;			//0x018 ---
	__vo uint32_t DR;			//0x01C DTS_DR
	__vo uint32_t SR;			//0x020 DTS_SR
	__vo uint32_t ITENR;			//0x024 DTS_ITENR
	__vo uint32_t ICIFR;			//0x028 DTS_ICIFR
	__vo uint32_t OR;			//0x02C DTS_OR
} DTS_RegDef_t;

/* -------------------------------------------------------------------------- */
/* ETH register layout                                                        */
/* -------------------------------------------------------------------------- */
/* ETH register map. */
typedef struct{
	__vo uint32_t MACCR;                 //0x000 ETH_MACCR
	__vo uint32_t MACECR;                //0x004 ETH_MACECR
	__vo uint32_t MACPFR;                //0x008 ETH_MACPFR
	__vo uint32_t MACWTR;                //0x00C ETH_MACWTR
	__vo uint32_t MACHT0R;               //0x010 ETH_MACHT0R
	__vo uint32_t MACHT1R;               //0x014 ETH_MACHT1R
	uint32_t reserved_1[14];             //0x018-0x04C
	__vo uint32_t MACVTR;                //0x050 ETH_MACVTR
	uint32_t reserved_2;                 //0x054
	__vo uint32_t MACVHTR;               //0x058 ETH_MACVHTR
	uint32_t reserved_3;                 //0x05C
	__vo uint32_t MACVIR;                //0x060 ETH_MACVIR
	__vo uint32_t MACIVIR;               //0x064 ETH_MACIVIR
	uint32_t reserved_4[2];              //0x068-0x06C
	__vo uint32_t MACQTXFCR;             //0x070 ETH_MACQTXFCR
	uint32_t reserved_5[7];              //0x074-0x08C
	__vo uint32_t MACRXFCR;              //0x090 ETH_MACRXFCR
	uint32_t reserved_6[7];              //0x094-0x0AC
	__vo uint32_t MACISR;                //0x0B0 ETH_MACISR
	__vo uint32_t MACIER;                //0x0B4 ETH_MACIER
	__vo uint32_t MACRXTXSR;             //0x0B8 ETH_MACRXTXSR
	uint32_t reserved_7;                 //0x0BC
	__vo uint32_t MACPCSR;               //0x0C0 ETH_MACPCSR
	__vo uint32_t MACRWKPFR;             //0x0C4 ETH_MACRWKPFR
	uint32_t reserved_8[2];              //0x0C8-0x0CC
	__vo uint32_t MACLCSR;               //0x0D0 ETH_MACLCSR
	__vo uint32_t MACLTCR;               //0x0D4 ETH_MACLTCR
	__vo uint32_t MACLETR;               //0x0D8 ETH_MACLETR
	__vo uint32_t MAC1USTCR;             //0x0DC ETH_MAC1USTCR
	uint32_t reserved_9[12];             //0x0E0-0x10C
	__vo uint32_t MACVR;                 //0x110 ETH_MACVR
	__vo uint32_t MACDR;                 //0x114 ETH_MACDR
	uint32_t reserved_10;                //0x118
	__vo uint32_t MACHWF0R;              //0x11C ETH_MACHWF0R
	__vo uint32_t MACHWF1R;              //0x120 ETH_MACHWF1R
	__vo uint32_t MACHWF2R;              //0x124 ETH_MACHWF2R
	__vo uint32_t MACHWF3R;              //0x128 ETH_MACHWF3R
	uint32_t reserved_11[53];            //0x12C-0x1FC
	__vo uint32_t MACMDIOAR;             //0x200 ETH_MACMDIOAR
	__vo uint32_t MACMDIODR;             //0x204 ETH_MACMDIODR
	uint32_t reserved_12[2];             //0x208-0x20C
	__vo uint32_t MACARPAR;              //0x210 ETH_MACARPAR
	uint32_t reserved_13[7];             //0x214-0x22C
	__vo uint32_t MACCSRSWCR;            //0x230 ETH_MACCSRSWCR
	uint32_t reserved_14[51];            //0x234-0x2FC

	/*
	 * MAC address registers
	 * MACA0 holds the primary MAC address.
	 * MACA1..MACA3 are additional address filters.
	 */
	__vo uint32_t MACA0HR;               //0x300 ETH_MACA0HR
	__vo uint32_t MACA0LR;               //0x304 ETH_MACA0LR
	__vo uint32_t MACA1HR;               //0x308 ETH_MACA1HR
	__vo uint32_t MACA1LR;               //0x30C ETH_MACA1LR
	__vo uint32_t MACA2HR;               //0x310 ETH_MACA2HR
	__vo uint32_t MACA2LR;               //0x314 ETH_MACA2LR
	__vo uint32_t MACA3HR;               //0x318 ETH_MACA3HR
	__vo uint32_t MACA3LR;               //0x31C ETH_MACA3LR

	uint32_t reserved_15[248];           //0x320-0x6FC

	__vo uint32_t MMC_CONTROL;           //0x700 ETH_MMC_CONTROL
	__vo uint32_t MMC_RX_INTERRUPT;      //0x704 ETH_MMC_RX_INTERRUPT
	__vo uint32_t MMC_TX_INTERRUPT;      //0x708 ETH_MMC_TX_INTERRUPT
	__vo uint32_t MMC_RX_INTERRUPT_MASK; //0x70C ETH_MMC_RX_INTERRUPT_MASK
	__vo uint32_t MMC_TX_INTERRUPT_MASK; //0x710 ETH_MMC_TX_INTERRUPT_MASK
	uint32_t reserved_16[14];            //0x714-0x748
	__vo uint32_t TX_SINGLE_COLLISION_GOOD_PACKETS;    //0x74C
	__vo uint32_t TX_MULTIPLE_COLLISION_GOOD_PACKETS;  //0x750
	uint32_t reserved_17[5];             //0x754-0x764
	__vo uint32_t TX_PACKET_COUNT_GOOD;  //0x768
	uint32_t reserved_18[10];            //0x76C-0x790
	__vo uint32_t RX_CRC_ERROR_PACKETS;  //0x794
	__vo uint32_t RX_ALIGNMENT_ERROR_PACKETS; //0x798
	uint32_t reserved_19[10];            //0x79C-0x7C0
	__vo uint32_t RX_UNICAST_PACKETS_GOOD; //0x7C4
	uint32_t reserved_20[9];             //0x7C8-0x7E8
	__vo uint32_t TX_LPI_USEC_CNTR;      //0x7EC
	__vo uint32_t TX_LPI_TRAN_CNTR;      //0x7F0
	__vo uint32_t RX_LPI_USEC_CNTR;      //0x7F4
	__vo uint32_t RX_LPI_TRAN_CNTR;      //0x7F8
	uint32_t reserved_21[65];            //0x7FC-0x8FC

	__vo uint32_t MACL3L4C0R;            //0x900 ETH_MACL3L4C0R
	__vo uint32_t MACL4A0R;              //0x904 ETH_MACL4A0R
	uint32_t reserved_22[2];             //0x908-0x90C
	__vo uint32_t MACL3A00R;             //0x910 ETH_MACL3A00R
	__vo uint32_t MACL3A10R;             //0x914 ETH_MACL3A10R
	__vo uint32_t MACL3A20R;             //0x918 ETH_MACL3A20R
	__vo uint32_t MACL3A30R;             //0x91C ETH_MACL3A30R
	uint32_t reserved_23[4];             //0x920-0x92C
	__vo uint32_t MACL3L4C1R;            //0x930 ETH_MACL3L4C1R
	__vo uint32_t MACL4A1R;              //0x934 ETH_MACL4A1R
	uint32_t reserved_24[2];             //0x938-0x93C
	__vo uint32_t MACL3A01R;             //0x940 ETH_MACL3A01R
	__vo uint32_t MACL3A11R;             //0x944 ETH_MACL3A11R
	__vo uint32_t MACL3A21R;             //0x948 ETH_MACL3A21R
	__vo uint32_t MACL3A31R;             //0x94C ETH_MACL3A31R
	uint32_t reserved_25[108];           //0x950-0xAFC

	__vo uint32_t MACTSCR;               //0xB00 ETH_MACTSCR
	__vo uint32_t MACSSIR;               //0xB04 ETH_MACSSIR
	__vo uint32_t MACSTSR;               //0xB08 ETH_MACSTSR
	__vo uint32_t MACSTNR;               //0xB0C ETH_MACSTNR
	__vo uint32_t MACSTSUR;              //0xB10 ETH_MACSTSUR
	__vo uint32_t MACSTNUR;              //0xB14 ETH_MACSTNUR
	__vo uint32_t MACTSAR;               //0xB18 ETH_MACTSAR
	uint32_t reserved_26;                //0xB1C
	__vo uint32_t MACTSSR;               //0xB20 ETH_MACTSSR
	uint32_t reserved_27[3];             //0xB24-0xB2C
	__vo uint32_t MACTXTSSNR;            //0xB30 ETH_MACTXTSSNR
	__vo uint32_t MACTXTSSSR;            //0xB34 ETH_MACTXTSSSR
	uint32_t reserved_28[2];             //0xB38-0xB3C
	__vo uint32_t MACACR;                //0xB40 ETH_MACACR
	uint32_t reserved_29;                //0xB44
	__vo uint32_t MACATSNR;              //0xB48 ETH_MACATSNR
	__vo uint32_t MACATSSR;              //0xB4C ETH_MACATSSR
	__vo uint32_t MACTSIACR;             //0xB50 ETH_MACTSIACR
	__vo uint32_t MACTSEACR;             //0xB54 ETH_MACTSEACR
	__vo uint32_t MACTSICNR;             //0xB58 ETH_MACTSICNR
	__vo uint32_t MACTSECNR;             //0xB5C ETH_MACTSECNR
	uint32_t reserved_30[4];             //0xB60-0xB6C
	__vo uint32_t MACPPSCR;              //0xB70 ETH_MACPPSCR
	uint32_t reserved_31[3];             //0xB74-0xB7C
	__vo uint32_t MACPPSTTSR;            //0xB80 ETH_MACPPSTTSR
	__vo uint32_t MACPPSTTNR;            //0xB84 ETH_MACPPSTTNR
	__vo uint32_t MACPPSIR;              //0xB88 ETH_MACPPSIR
	__vo uint32_t MACPPSWR;              //0xB8C ETH_MACPPSWR
	uint32_t reserved_32[12];            //0xB90-0xBBC
	__vo uint32_t MACPOCR;               //0xBC0 ETH_MACPOCR
	__vo uint32_t MACSPI0R;              //0xBC4 ETH_MACSPI0R
	__vo uint32_t MACSPI1R;              //0xBC8 ETH_MACSPI1R
	__vo uint32_t MACSPI2R;              //0xBCC ETH_MACSPI2R
	__vo uint32_t MACLMIR;               //0xBD0 ETH_MACLMIR
	uint32_t reserved_33[11];            //0xBD4-0xBFC

	__vo uint32_t MTLOMR;                //0xC00 ETH_MTLOMR
	uint32_t reserved_34[7];             //0xC04-0xC1C
	__vo uint32_t MTLISR;                //0xC20 ETH_MTLISR
	uint32_t reserved_35[55];            //0xC24-0xCFC
	__vo uint32_t MTLTXQOMR;             //0xD00 ETH_MTLTXQOMR
	__vo uint32_t MTLTXQUR;              //0xD04 ETH_MTLTXQUR
	__vo uint32_t MTLTXQDR;              //0xD08 ETH_MTLTXQDR
	uint32_t reserved_36[8];             //0xD0C-0xD28
	__vo uint32_t MTLQICSR;              //0xD2C ETH_MTLQICSR
	__vo uint32_t MTLRXQOMR;             //0xD30 ETH_MTLRXQOMR
	__vo uint32_t MTLRXQMPOCR;           //0xD34 ETH_MTLRXQMPOCR
	__vo uint32_t MTLRXQDR;              //0xD38 ETH_MTLRXQDR
	uint32_t reserved_37[177];           //0xD3C-0xFFC

	__vo uint32_t DMAMR;                 //0x1000 ETH_DMAMR
	__vo uint32_t DMASBMR;               //0x1004 ETH_DMASBMR
	__vo uint32_t DMAISR;                //0x1008 ETH_DMAISR
	__vo uint32_t DMADSR;                //0x100C ETH_DMADSR
	uint32_t reserved_38[60];            //0x1010-0x10FC

	__vo uint32_t DMACCR;                //0x1100 ETH_DMACCR
	__vo uint32_t DMACTXCR;              //0x1104 ETH_DMACTXCR
	__vo uint32_t DMACRXCR;              //0x1108 ETH_DMACRXCR
	uint32_t reserved_39[2];             //0x110C-0x1110
	__vo uint32_t DMACTXDLAR;            //0x1114 ETH_DMACTXDLAR
	uint32_t reserved_40;                //0x1118
	__vo uint32_t DMACRXDLAR;            //0x111C ETH_DMACRXDLAR
	__vo uint32_t DMACTXDTPR;            //0x1120 ETH_DMACTXDTPR
	uint32_t reserved_41;                //0x1124
	__vo uint32_t DMACRXDTPR;            //0x1128 ETH_DMACRXDTPR
	__vo uint32_t DMACTXRLR;             //0x112C ETH_DMACTXRLR
	__vo uint32_t DMACRXRLR;             //0x1130 ETH_DMACRXRLR
	__vo uint32_t DMACIER;               //0x1134 ETH_DMACIER
	__vo uint32_t DMACRXIWTR;            //0x1138 ETH_DMACRXIWTR
	uint32_t reserved_42[2];             //0x113C-0x1140
	__vo uint32_t DMACCATXDR;            //0x1144 ETH_DMACCATXDR
	uint32_t reserved_43;                //0x1148
	__vo uint32_t DMACCARXDR;            //0x114C ETH_DMACCARXDR
	uint32_t reserved_44;                //0x1150
	__vo uint32_t DMACCATXBR;            //0x1154 ETH_DMACCATXBR
	uint32_t reserved_45;                //0x1158
	__vo uint32_t DMACCARXBR;            //0x115C ETH_DMACCARXBR
	__vo uint32_t DMACSR;                //0x1160 ETH_DMACSR
	uint32_t reserved_46[2];             //0x1164-0x1168
	__vo uint32_t DMACMFCR;              //0x116C ETH_DMACMFCR
} ETH_RegDef_t;

/* -------------------------------------------------------------------------- */
/* EXTI register layout                                                       */
/* -------------------------------------------------------------------------- */
/* EXTI register map. */
typedef struct{
	__vo uint32_t RTSR1;			//0x000 EXTI_RTSR1
	__vo uint32_t FTSR1;			//0x004 EXTI_FTSR1
	__vo uint32_t SWIER1;			//0x008 EXTI_SWIER1
	uint32_t reserved_1[5];		//0x00C-0x01C ---
	__vo uint32_t RTSR2;			//0x020 EXTI_RTSR2
	__vo uint32_t FTSR2;			//0x024 EXTI_FTSR2
	__vo uint32_t SWIER2;			//0x028 EXTI_SWIER2
	uint32_t reserved_2[21];		//0x02C-0x07C ---
	__vo uint32_t IMR1;			//0x080 EXTI_IMR1
	__vo uint32_t EMR1;			//0x084 EXTI_EMR1
	__vo uint32_t PR1;			//0x088 EXTI_PR1
	uint32_t reserved_3;			//0x08C ---
	__vo uint32_t IMR2;			//0x090 EXTI_IMR2
	__vo uint32_t EMR2;			//0x094 EXTI_EMR2
	__vo uint32_t PR2;			//0x098 EXTI_PR2
	uint32_t reserved_4;			//0x09C ---
	__vo uint32_t IMR3;			//0x0A0 EXTI_IMR3
	__vo uint32_t EMR3;			//0x0A4 EXTI_EMR3
} EXTI_RegDef_t;

/* -------------------------------------------------------------------------- */
/* FDCAN register layout                                                      */
/* -------------------------------------------------------------------------- */
/* FDCAN register map. */
typedef struct{
	__vo uint32_t CREL;			//0x000 FDCAN_CREL
	__vo uint32_t ENDN;			//0x004 FDCAN_ENDN
	uint32_t reserved_1;			//0x008 ---
	__vo uint32_t DBTP;			//0x00C FDCAN_DBTP
	__vo uint32_t TEST;			//0x010 FDCAN_TEST
	__vo uint32_t RWD;			//0x014 FDCAN_RWD
	__vo uint32_t CCCR;			//0x018 FDCAN_CCCR
	__vo uint32_t NBTP;			//0x01C FDCAN_NBTP
	__vo uint32_t TSCC;			//0x020 FDCAN_TSCC
	__vo uint32_t TSCV;			//0x024 FDCAN_TSCV
	__vo uint32_t TOCC;			//0x028 FDCAN_TOCC
	__vo uint32_t TOCV;			//0x02C FDCAN_TOCV
	uint32_t reserved_2[4];		//0x030-0x03C ---
	__vo uint32_t ECR;			//0x040 FDCAN_ECR
	__vo uint32_t PSR;			//0x044 FDCAN_PSR
	__vo uint32_t TDCR;			//0x048 FDCAN_TDCR
	uint32_t reserved_3;			//0x04C ---
	__vo uint32_t IR;			//0x050 FDCAN_IR
	__vo uint32_t IE;			//0x054 FDCAN_IE
	__vo uint32_t ILS;			//0x058 FDCAN_ILS
	__vo uint32_t ILE;			//0x05C FDCAN_ILE
	uint32_t reserved_4[8];		//0x060-0x07C ---
	__vo uint32_t RXGFC;			//0x080 FDCAN_RXGFC
	__vo uint32_t XIDAM;			//0x084 FDCAN_XIDAM
	__vo uint32_t HPMS;			//0x088 FDCAN_HPMS
	uint32_t reserved_5;			//0x08C ---
	__vo uint32_t RXF0S;			//0x090 FDCAN_RXF0S
	__vo uint32_t RXF0A;			//0x094 FDCAN_RXF0A
	__vo uint32_t RXF1S;			//0x098 FDCAN_RXF1S
	__vo uint32_t RXF1A;			//0x09C FDCAN_RXF1A
	uint32_t reserved_6[8];		//0x0A0-0x0BC ---
	__vo uint32_t TXBC;			//0x0C0 FDCAN_TXBC
	__vo uint32_t TXFQS;			//0x0C4 FDCAN_TXFQS
	__vo uint32_t TXBRP;			//0x0C8 FDCAN_TXBRP
	__vo uint32_t TXBAR;			//0x0CC FDCAN_TXBAR
	__vo uint32_t TXBCR;			//0x0D0 FDCAN_TXBCR
	__vo uint32_t TXBTO;			//0x0D4 FDCAN_TXBTO
	__vo uint32_t TXBCF;			//0x0D8 FDCAN_TXBCF
	__vo uint32_t TXBTIE;			//0x0DC FDCAN_TXBTIE
	__vo uint32_t TXBCIE;			//0x0E0 FDCAN_TXBCIE
	__vo uint32_t TXEFS;			//0x0E4 FDCAN_TXEFS
	__vo uint32_t TXEFA;			//0x0E8 FDCAN_TXEFA
	uint32_t reserved_7[5];		//0x0EC-0x0FC ---
	__vo uint32_t CKDIV;			//0x100 FDCAN_CKDIV
} FDCAN_RegDef_t;

/* -------------------------------------------------------------------------- */
/* FLASH register layout                                                      */
/* -------------------------------------------------------------------------- */
/* FLASH register map. */
typedef struct{
	__vo uint32_t ACR;			//0x000 FLASH_ACR
	__vo uint32_t KEYR;			//0x004 FLASH_KEYR
	uint32_t reserved_1[2];		//0x008-0x00C ---
	__vo uint32_t CR;			//0x010 FLASH_CR
	__vo uint32_t SR;			//0x014 FLASH_SR
	uint32_t reserved_2[2];		//0x018-0x01C ---
	__vo uint32_t IER;			//0x020 FLASH_IER
	__vo uint32_t ISR;			//0x024 FLASH_ISR
	__vo uint32_t ICR;			//0x028 FLASH_ICR
	uint32_t reserved_3;			//0x02C ---
	__vo uint32_t CRCCR;			//0x030 FLASH_CRCCR
	__vo uint32_t CRCSADDR;			//0x034 FLASH_CRCSADDR
	__vo uint32_t CRCEADDR;			//0x038 FLASH_CRCEADDR
	__vo uint32_t CRCDATAR;			//0x03C FLASH_CRCDATAR
	__vo uint32_t ECCSFADDR;			//0x040 FLASH_ECCSFADDR
	__vo uint32_t ECCDFADDR;			//0x044 FLASH_ECCDFADDR
	uint32_t reserved_4[46];		//0x048-0x0FC ---
	__vo uint32_t OPTKEYR;			//0x100 FLASH_OPTKEYR
	__vo uint32_t OPTCR;			//0x104 FLASH_OPTCR
	__vo uint32_t OPTISR;			//0x108 FLASH_OPTISR
	__vo uint32_t OPTICR;			//0x10C FLASH_OPTICR
	__vo uint32_t OBKCR;			//0x110 FLASH_OBKCR
	uint32_t reserved_5[59];		//0x114-0x1FC ---
	__vo uint32_t NVSR;			//0x200 FLASH_NVSR
	__vo uint32_t NVSRP;			//0x204 FLASH_NVSRP
	__vo uint32_t ROTSR;			//0x208 FLASH_ROTSR
	__vo uint32_t ROTSRP;			//0x20C FLASH_ROTSRP
	__vo uint32_t OTPLSR;			//0x210 FLASH_OTPLSR
	__vo uint32_t OTPLSRP;			//0x214 FLASH_OTPLSRP
	__vo uint32_t WRPSR;			//0x218 FLASH_WRPSR
	__vo uint32_t WRPSRP;			//0x21C FLASH_WRPSRP
	uint32_t reserved_6[4];		//0x220-0x22C ---
	__vo uint32_t HDPSR;			//0x230 FLASH_HDPSR
	__vo uint32_t HDPSRP;			//0x234 FLASH_HDPSRP
	uint32_t reserved_7[6];		//0x238-0x24C ---
	__vo uint32_t EPOCHSR;			//0x250 FLASH_EPOCHSR
	__vo uint32_t EPOCHSRP;			//0x254 FLASH_EPOCHSRP
	uint32_t reserved_8[2];		//0x258-0x25C ---
	__vo uint32_t OBW1SR;			//0x260 FLASH_OBW1SR
	__vo uint32_t OBW1SRP;			//0x264 FLASH_OBW1SRP
	__vo uint32_t OBW2SR;			//0x268 FLASH_OBW2SR
	__vo uint32_t OBW2SRP;			//0x26C FLASH_OBW2SRP
} FLASH_RegDef_t;

/* -------------------------------------------------------------------------- */
/* FMC register layout                                                        */
/* -------------------------------------------------------------------------- */
/* FMC register map. */
typedef struct{
	uint32_t reserved_1[33];		//0x000-0x080 ---
	__vo uint32_t SR;			//0x084 FMC_SR
	__vo uint32_t PMEM;			//0x088 FMC_PMEM
	__vo uint32_t PATT;			//0x08C FMC_PATT
	uint32_t reserved_2;			//0x090 ---
	__vo uint32_t ECCR;			//0x094 FMC_ECCR
	uint32_t reserved_3[46];		//0x098-0x14C ---
	__vo uint32_t SDCMR;			//0x150 FMC_SDCMR
	__vo uint32_t SDRTR;			//0x154 FMC_SDRTR
	__vo uint32_t SDSR;			//0x158 FMC_SDSR
} FMC_RegDef_t;

/* -------------------------------------------------------------------------- */
/* GFXMMU register layout                                                     */
/* -------------------------------------------------------------------------- */
/* GFXMMU register map. */
typedef struct{
	__vo uint32_t CR;			//0x000 GFXMMU_CR
	__vo uint32_t SR;			//0x004 GFXMMU_SR
	__vo uint32_t FCR;			//0x008 GFXMMU_FCR
	uint32_t reserved_1;			//0x00C ---
	__vo uint32_t DVR;			//0x010 GFXMMU_DVR
	__vo uint32_t DAR;			//0x014 GFXMMU_DAR
} GFXMMU_RegDef_t;

/* -------------------------------------------------------------------------- */
/* GFXTIM register layout                                                     */
/* -------------------------------------------------------------------------- */
/* GFXTIM register map. */
typedef struct{
	__vo uint32_t CR;			//0x000 GFXTIM_CR
	__vo uint32_t CGCR;			//0x004 GFXTIM_CGCR
	__vo uint32_t TCR;			//0x008 GFXTIM_TCR
	__vo uint32_t TDR;			//0x00C GFXTIM_TDR
	__vo uint32_t EVCR;			//0x010 GFXTIM_EVCR
	__vo uint32_t EVSR;			//0x014 GFXTIM_EVSR
	uint32_t reserved_1[2];		//0x018-0x01C ---
	__vo uint32_t WDGTCR;			//0x020 GFXTIM_WDGTCR
	uint32_t reserved_2[3];		//0x024-0x02C ---
	__vo uint32_t ISR;			//0x030 GFXTIM_ISR
	__vo uint32_t ICR;			//0x034 GFXTIM_ICR
	__vo uint32_t IER;			//0x038 GFXTIM_IER
	__vo uint32_t TSR;			//0x03C GFXTIM_TSR
	__vo uint32_t LCCRR;			//0x040 GFXTIM_LCCRR
	__vo uint32_t FCCRR;			//0x044 GFXTIM_FCCRR
	uint32_t reserved_3[2];		//0x048-0x04C ---
	__vo uint32_t ATR;			//0x050 GFXTIM_ATR
	__vo uint32_t AFCR;			//0x054 GFXTIM_AFCR
	__vo uint32_t ALCR;			//0x058 GFXTIM_ALCR
	uint32_t reserved_4;			//0x05C ---
	__vo uint32_t AFCC1R;			//0x060 GFXTIM_AFCC1R
	uint32_t reserved_5[3];		//0x064-0x06C ---
	__vo uint32_t ALCC1R;			//0x070 GFXTIM_ALCC1R
	__vo uint32_t ALCC2R;			//0x074 GFXTIM_ALCC2R
	uint32_t reserved_6[2];		//0x078-0x07C ---
	__vo uint32_t RFC1R;			//0x080 GFXTIM_RFC1R
	__vo uint32_t RFC1RR;			//0x084 GFXTIM_RFC1RR
	__vo uint32_t RFC2R;			//0x088 GFXTIM_RFC2R
	__vo uint32_t RFC2RR;			//0x08C GFXTIM_RFC2RR
	uint32_t reserved_7[4];		//0x090-0x09C ---
	__vo uint32_t WDGCR;			//0x0A0 GFXTIM_WDGCR
	__vo uint32_t WDGRR;			//0x0A4 GFXTIM_WDGRR
	__vo uint32_t WDGPAR;			//0x0A8 GFXTIM_WDGPAR
} GFXTIM_RegDef_t;

/* -------------------------------------------------------------------------- */
/* GPDMA register layout                                                      */
/* -------------------------------------------------------------------------- */
/* GPDMA register map. */
typedef struct{
	uint32_t reserved_1;			//0x000 ---
	__vo uint32_t PRIVCFGR;			//0x004 GPDMA_PRIVCFGR
	__vo uint32_t RCFGLOCKR;			//0x008 GPDMA_RCFGLOCKR
	__vo uint32_t MISR;			//0x00C GPDMA_MISR
} GPDMA_RegDef_t;

/* -------------------------------------------------------------------------- */
/* GPIO register layout                                                       */
/* -------------------------------------------------------------------------- */
/* GPIO register map. */
typedef struct{
	__vo uint32_t MODER;            //0x00 GPIO mode register
	__vo uint32_t OTYPER;           //0x04 GPIO output type register
	__vo uint32_t OSPEEDR;          //0x08 GPIO output speed register
	__vo uint32_t PUPDR;            //0x0C GPIO pull-up/pull-down register
	__vo uint32_t IDR;              //0x10 GPIO input data register
	__vo uint32_t ODR;              //0x14 GPIO output data register
	__vo uint32_t BSRR;             //0x18 GPIO bit set/reset register
	__vo uint32_t LCKR;             //0x1C GPIO configuration lock register
	__vo uint32_t AFRL;             //0x20 GPIO alternate function low register
	__vo uint32_t AFRH;             //0x24 GPIO alternate function high register
	__vo uint32_t BRR;              //0x28 GPIO bit reset register
} GPIO_RegDef_t;

/* -------------------------------------------------------------------------- */
/* HASH register layout                                                       */
/* -------------------------------------------------------------------------- */
/* HASH register map. */
typedef struct{
	__vo uint32_t CR;			//0x000 HASH_CR
	__vo uint32_t DIN;			//0x004 HASH_DIN
	__vo uint32_t STR;			//0x008 HASH_STR
	uint32_t reserved_1[5];		//0x00C-0x01C ---
	__vo uint32_t IMR;			//0x020 HASH_IMR
	__vo uint32_t SR;			//0x024 HASH_SR
} HASH_RegDef_t;

/* -------------------------------------------------------------------------- */
/* HPDMA register layout                                                      */
/* -------------------------------------------------------------------------- */
/* HPDMA register map. */
typedef struct{
	uint32_t reserved_1;			//0x000 ---
	__vo uint32_t PRIVCFGR;			//0x004 HPDMA_PRIVCFGR
	__vo uint32_t RCFGLOCKR;			//0x008 HPDMA_RCFGLOCKR
	__vo uint32_t MISR;			//0x00C HPDMA_MISR
} HPDMA_RegDef_t;

/* -------------------------------------------------------------------------- */
/* I2C register layout                                                        */
/* -------------------------------------------------------------------------- */
/* I2C register map. */
typedef struct{
	__vo uint32_t CR1;			//0x000 I2C_CR1
	__vo uint32_t CR2;			//0x004 I2C_CR2
	__vo uint32_t OAR1;			//0x008 I2C_OAR1
	__vo uint32_t OAR2;			//0x00C I2C_OAR2
	__vo uint32_t TIMINGR;			//0x010 I2C_TIMINGR
	__vo uint32_t TIMEOUTR;			//0x014 I2C_TIMEOUTR
	__vo uint32_t ISR;			//0x018 I2C_ISR
	__vo uint32_t ICR;			//0x01C I2C_ICR
	__vo uint32_t PECR;			//0x020 I2C_PECR
	__vo uint32_t RXDR;			//0x024 I2C_RXDR
	__vo uint32_t TXDR;			//0x028 I2C_TXDR
} I2C_RegDef_t;

/* -------------------------------------------------------------------------- */
/* I3C register layout                                                        */
/* -------------------------------------------------------------------------- */
/* I3C register map. */
typedef struct{
	__vo uint32_t CR;			//0x000 I3C_CR
	__vo uint32_t CFGR;			//0x004 I3C_CFGR
	uint32_t reserved_1[2];		//0x008-0x00C ---
	__vo uint32_t RDR;			//0x010 I3C_RDR
	__vo uint32_t RDWR;			//0x014 I3C_RDWR
	__vo uint32_t TDR;			//0x018 I3C_TDR
	__vo uint32_t TDWR;			//0x01C I3C_TDWR
	__vo uint32_t IBIDR;			//0x020 I3C_IBIDR
	__vo uint32_t TGTTDR;			//0x024 I3C_TGTTDR
	uint32_t reserved_2[2];		//0x028-0x02C ---
	__vo uint32_t SR;			//0x030 I3C_SR
	__vo uint32_t SER;			//0x034 I3C_SER
	uint32_t reserved_3[2];		//0x038-0x03C ---
	__vo uint32_t RMR;			//0x040 I3C_RMR
	uint32_t reserved_4[3];		//0x044-0x04C ---
	__vo uint32_t EVR;			//0x050 I3C_EVR
	__vo uint32_t IER;			//0x054 I3C_IER
	__vo uint32_t CEVR;			//0x058 I3C_CEVR
	uint32_t reserved_5;			//0x05C ---
	__vo uint32_t DEVR0;			//0x060 I3C_DEVR0
	uint32_t reserved_6[11];		//0x064-0x08C ---
	__vo uint32_t MAXRLR;			//0x090 I3C_MAXRLR
	__vo uint32_t MAXWLR;			//0x094 I3C_MAXWLR
	uint32_t reserved_7[2];		//0x098-0x09C ---
	__vo uint32_t TIMINGR0;			//0x0A0 I3C_TIMINGR0
	__vo uint32_t TIMINGR1;			//0x0A4 I3C_TIMINGR1
	__vo uint32_t TIMINGR2;			//0x0A8 I3C_TIMINGR2
	uint32_t reserved_8[5];		//0x0AC-0x0BC ---
	__vo uint32_t BCR;			//0x0C0 I3C_BCR
	__vo uint32_t DCR;			//0x0C4 I3C_DCR
	__vo uint32_t GETCAPR;			//0x0C8 I3C_GETCAPR
	__vo uint32_t CRCAPR;			//0x0CC I3C_CRCAPR
	__vo uint32_t GETMXDSR;			//0x0D0 I3C_GETMXDSR
	__vo uint32_t EPIDR;			//0x0D4 I3C_EPIDR
} I3C_RegDef_t;

/* -------------------------------------------------------------------------- */
/* ICACHE register layout                                                     */
/* -------------------------------------------------------------------------- */
/* ICACHE register map. */
typedef struct{
	__vo uint32_t CR;			//0x000 ICACHE_CR
	__vo uint32_t SR;			//0x004 ICACHE_SR
	__vo uint32_t IER;			//0x008 ICACHE_IER
	__vo uint32_t FCR;			//0x00C ICACHE_FCR
	__vo uint32_t HMONR;			//0x010 ICACHE_HMONR
	__vo uint32_t MMONR;			//0x014 ICACHE_MMONR
} ICACHE_RegDef_t;

/* -------------------------------------------------------------------------- */
/* IWDG register layout                                                       */
/* -------------------------------------------------------------------------- */
/* IWDG register map. */
typedef struct{
	__vo uint32_t KR;			//0x000 IWDG_KR
	__vo uint32_t PR;			//0x004 IWDG_PR
	__vo uint32_t RLR;			//0x008 IWDG_RLR
	__vo uint32_t SR;			//0x00C IWDG_SR
	__vo uint32_t WINR;			//0x010 IWDG_WINR
	__vo uint32_t EWCR;			//0x014 IWDG_EWCR
} IWDG_RegDef_t;

/* -------------------------------------------------------------------------- */
/* JPEG register layout                                                       */
/* -------------------------------------------------------------------------- */
/* JPEG register map. */
typedef struct{
	__vo uint32_t CONFR0;			//0x000 JPEG_CONFR0
	__vo uint32_t CONFR1;			//0x004 JPEG_CONFR1
	__vo uint32_t CONFR2;			//0x008 JPEG_CONFR2
	__vo uint32_t CONFR3;			//0x00C JPEG_CONFR3
	uint32_t reserved_1[8];		//0x010-0x02C ---
	__vo uint32_t CR;			//0x030 JPEG_CR
	__vo uint32_t SR;			//0x034 JPEG_SR
	__vo uint32_t CFR;			//0x038 JPEG_CFR
	uint32_t reserved_2;			//0x03C ---
	__vo uint32_t DIR;			//0x040 JPEG_DIR
	__vo uint32_t DOR;			//0x044 JPEG_DOR
} JPEG_RegDef_t;

/* -------------------------------------------------------------------------- */
/* LPTIM register layout                                                      */
/* -------------------------------------------------------------------------- */
/* LPTIM register map. */
typedef struct{
	__vo uint32_t ISR;			//0x000 LPTIMx_ISR
	__vo uint32_t ICR;			//0x004 LPTIMx_ICR
	__vo uint32_t DIER;			//0x008 LPTIMx_DIER
	__vo uint32_t CFGR;			//0x00C LPTIM_CFGR
	__vo uint32_t CR;			//0x010 LPTIM_CR
	__vo uint32_t CCR1;			//0x014 LPTIM_CCR1
	__vo uint32_t ARR;			//0x018 LPTIM_ARR
	__vo uint32_t CNT;			//0x01C LPTIM_CNT
	uint32_t reserved_1;			//0x020 ---
	__vo uint32_t CFGR2;			//0x024 LPTIM_CFGR2
	__vo uint32_t RCR;			//0x028 LPTIM_RCR
	__vo uint32_t CCMR1;			//0x02C LPTIM_CCMR1
	uint32_t reserved_2;			//0x030 ---
	__vo uint32_t CCR2;			//0x034 LPTIM_CCR2
} LPTIM_RegDef_t;

/* -------------------------------------------------------------------------- */
/* LPUART register layout                                                     */
/* -------------------------------------------------------------------------- */
/* LPUART register map. */
typedef struct{
	__vo uint32_t CR1;			//0x000 LPUART_CR1
	__vo uint32_t CR2;			//0x004 LPUART_CR2
	__vo uint32_t CR3;			//0x008 LPUART_CR3
	__vo uint32_t BRR;			//0x00C LPUART_BRR
	uint32_t reserved_1[2];		//0x010-0x014 ---
	__vo uint32_t RQR;			//0x018 LPUART_RQR
	__vo uint32_t ISR;			//0x01C LPUART_ISR
	__vo uint32_t ICR;			//0x020 LPUART_ICR
	__vo uint32_t RDR;			//0x024 LPUART_RDR
	__vo uint32_t TDR;			//0x028 LPUART_TDR
	__vo uint32_t PRESC;			//0x02C LPUART_PRESC
} LPUART_RegDef_t;

/* -------------------------------------------------------------------------- */
/* LTDC register layout                                                       */
/* -------------------------------------------------------------------------- */
/* LTDC register map. */
typedef struct{
	uint32_t reserved_1[2];		//0x000-0x004 ---
	__vo uint32_t SSCR;			//0x008 LTDC_SSCR
	__vo uint32_t BPCR;			//0x00C LTDC_BPCR
	__vo uint32_t AWCR;			//0x010 LTDC_AWCR
	__vo uint32_t TWCR;			//0x014 LTDC_TWCR
	__vo uint32_t GCR;			//0x018 LTDC_GCR
	uint32_t reserved_2[2];		//0x01C-0x020 ---
	__vo uint32_t SRCR;			//0x024 LTDC_SRCR
	uint32_t reserved_3;			//0x028 ---
	__vo uint32_t BCCR;			//0x02C LTDC_BCCR
	uint32_t reserved_4;			//0x030 ---
	__vo uint32_t IER;			//0x034 LTDC_IER
	__vo uint32_t ISR;			//0x038 LTDC_ISR
	__vo uint32_t ICR;			//0x03C LTDC_ICR
	__vo uint32_t LIPCR;			//0x040 LTDC_LIPCR
	__vo uint32_t CPSR;			//0x044 LTDC_CPSR
	__vo uint32_t CDSR;			//0x048 LTDC_CDSR
} LTDC_RegDef_t;

/* -------------------------------------------------------------------------- */
/* MCE register layout                                                        */
/* -------------------------------------------------------------------------- */
/* MCE register map. */
typedef struct{
	__vo uint32_t CR;			//0x000 MCE_CR
	__vo uint32_t SR;			//0x004 MCE_SR
	__vo uint32_t IASR;			//0x008 MCE_IASR
	__vo uint32_t IACR;			//0x00C MCE_IACR
	__vo uint32_t IAIER;			//0x010 MCE_IAIER
	uint32_t reserved_1[2];		//0x014-0x018 ---
	__vo uint32_t PRIVCFGR;			//0x01C MCE_PRIVCFGR
	__vo uint32_t IAESR;			//0x020 MCE_IAESR
	__vo uint32_t IADDR;			//0x024 MCE_IADDR
} MCE_RegDef_t;

/* -------------------------------------------------------------------------- */
/* MDIOS register layout                                                      */
/* -------------------------------------------------------------------------- */
/* MDIOS register map. */
typedef struct{
	__vo uint32_t CR;			//0x000 MDIOS_CR
	__vo uint32_t WRFR;			//0x004 MDIOS_WRFR
	__vo uint32_t CWRFR;			//0x008 MDIOS_CWRFR
	__vo uint32_t RDFR;			//0x00C MDIOS_RDFR
	__vo uint32_t CRDFR;			//0x010 MDIOS_CRDFR
	__vo uint32_t SR;			//0x014 MDIOS_SR
	__vo uint32_t CLRFR;			//0x018 MDIOS_CLRFR
} MDIOS_RegDef_t;

/* -------------------------------------------------------------------------- */
/* OTG register layout                                                        */
/* -------------------------------------------------------------------------- */
/* OTG register map. */
typedef struct{
	__vo uint32_t GOTGCTL;			//0x000 OTG_GOTGCTL
	__vo uint32_t GOTGINT;			//0x004 OTG_GOTGINT
	__vo uint32_t GAHBCFG;			//0x008 OTG_GAHBCFG
	__vo uint32_t GUSBCFG;			//0x00C OTG_GUSBCFG
	__vo uint32_t GRSTCTL;			//0x010 OTG_GRSTCTL
	__vo uint32_t GINTSTS;			//0x014 OTG_GINTSTS
	__vo uint32_t GINTMSK;			//0x018 OTG_GINTMSK
	__vo uint32_t GRXSTSR;			//0x01C OTG_GRXSTSR
	__vo uint32_t GRXSTSP;			//0x020 OTG_GRXSTSP
	__vo uint32_t GRXFSIZ;			//0x024 OTG_GRXFSIZ
	__vo uint32_t DIEPTXF0;			//0x028 OTG_DIEPTXF0
	__vo uint32_t HNPTXSTS;			//0x02C OTG_HNPTXSTS
	uint32_t reserved_1[2];		//0x030-0x034 ---
	__vo uint32_t GCCFG;			//0x038 OTG_GCCFG
	__vo uint32_t CID;			//0x03C OTG_CID
	uint32_t reserved_2[5];		//0x040-0x050 ---
	__vo uint32_t GLPMCFG;			//0x054 OTG_GLPMCFG
	uint32_t reserved_3[42];		//0x058-0x0FC ---
	__vo uint32_t HPTXFSIZ;			//0x100 OTG_HPTXFSIZ
	uint32_t reserved_4[191];		//0x104-0x3FC ---
	__vo uint32_t HCFG;			//0x400 OTG_HCFG
	__vo uint32_t HFIR;			//0x404 OTG_HFIR
	__vo uint32_t HFNUM;			//0x408 OTG_HFNUM
	uint32_t reserved_5;			//0x40C ---
	__vo uint32_t HPTXSTS;			//0x410 OTG_HPTXSTS
	__vo uint32_t HAINT;			//0x414 OTG_HAINT
	__vo uint32_t HAINTMSK;			//0x418 OTG_HAINTMSK
	uint32_t reserved_6[9];		//0x41C-0x43C ---
	__vo uint32_t HPRT;			//0x440 OTG_HPRT
	uint32_t reserved_7[239];		//0x444-0x7FC ---
	__vo uint32_t DCFG;			//0x800 OTG_DCFG
	__vo uint32_t DCTL;			//0x804 OTG_DCTL
	__vo uint32_t DSTS;			//0x808 OTG_DSTS
	uint32_t reserved_8;			//0x80C ---
	__vo uint32_t DIEPMSK;			//0x810 OTG_DIEPMSK
	__vo uint32_t DOEPMSK;			//0x814 OTG_DOEPMSK
	__vo uint32_t DAINT;			//0x818 OTG_DAINT
	__vo uint32_t DAINTMSK;			//0x81C OTG_DAINTMSK
	uint32_t reserved_9[4];		//0x820-0x82C ---
	__vo uint32_t DTHRCTL;			//0x830 OTG_DTHRCTL
	__vo uint32_t DIEPEMPMSK;			//0x834 OTG_DIEPEMPMSK
	__vo uint32_t DEACHINT;			//0x838 OTG_DEACHINT
	__vo uint32_t DEACHINTMSK;			//0x83C OTG_DEACHINTMSK
	uint32_t reserved_10;			//0x840 ---
	__vo uint32_t DIEPEACHM;			//0x844 OTG_DIEPEACHM
	uint32_t reserved_11[15];		//0x848-0x880 ---
	__vo uint32_t DOEPEACHMSK1;			//0x884 OTG_DOEPEACHMSK1
	uint32_t reserved_12[30];		//0x888-0x8FC ---
	__vo uint32_t DIEPCTL0;			//0x900 OTG_DIEPCTL0
	uint32_t reserved_13[3];		//0x904-0x90C ---
	__vo uint32_t DIEPTSIZ0;			//0x910 OTG_DIEPTSIZ0
	uint32_t reserved_14[123];		//0x914-0xAFC ---
	__vo uint32_t DIEPTSIZx;			//0xB00 OTG_DIEPTSIZx
	uint32_t reserved_15[3];		//0xB04-0xB0C ---
	__vo uint32_t DOEPTSIZ0;			//0xB10 OTG_DOEPTSIZ0
	uint32_t reserved_16[187];		//0xB14-0xDFC ---
	__vo uint32_t PCGCCTL;			//0xE00 OTG_PCGCCTL
	__vo uint32_t PCGCCTL1;			//0xE04 OTG_PCGCCTL1
} OTG_RegDef_t;

/* -------------------------------------------------------------------------- */
/* PKA register layout                                                        */
/* -------------------------------------------------------------------------- */
/* PKA register map. */
typedef struct{
	__vo uint32_t CR;			//0x000 PKA_CR
	__vo uint32_t SR;			//0x004 PKA_SR
	__vo uint32_t CLRFR;			//0x008 PKA_CLRFR
} PKA_RegDef_t;

/* -------------------------------------------------------------------------- */
/* PSSI register layout                                                       */
/* -------------------------------------------------------------------------- */
/* PSSI register map. */
typedef struct{
	__vo uint32_t CR;			//0x000 PSSI_CR
	__vo uint32_t SR;			//0x004 PSSI_SR
	__vo uint32_t RIS;			//0x008 PSSI_RIS
	__vo uint32_t IER;			//0x00C PSSI_IER
	__vo uint32_t MIS;			//0x010 PSSI_MIS
	__vo uint32_t ICR;			//0x014 PSSI_ICR
	uint32_t reserved_1[4];		//0x018-0x024 ---
	__vo uint32_t DR;			//0x028 PSSI_DR
} PSSI_RegDef_t;

/* -------------------------------------------------------------------------- */
/* PWR register layout                                                        */
/* -------------------------------------------------------------------------- */
/* PWR register map. */
typedef struct{
	__vo uint32_t CR1;			//0x000 PWR_CR1
	__vo uint32_t SR1;			//0x004 PWR_SR1
	__vo uint32_t CSR1;			//0x008 PWR_CSR1
	__vo uint32_t CSR2;			//0x00C PWR_CSR2
	__vo uint32_t CSR3;			//0x010 PWR_CSR3
	__vo uint32_t CSR4;			//0x014 PWR_CSR4
	uint32_t reserved_1[2];		//0x018-0x01C ---
	__vo uint32_t WKUPCR;			//0x020 PWR_WKUPCR
	__vo uint32_t WKUPFR;			//0x024 PWR_WKUPFR
	__vo uint32_t WKUPEPR;			//0x028 PWR_WKUPEPR
	__vo uint32_t UCPDR;			//0x02C PWR_UCPDR
	__vo uint32_t APCR;			//0x030 PWR_APCR
	__vo uint32_t PUCRN;			//0x034 PWR_PUCRN
	__vo uint32_t PDCRN;			//0x038 PWR_PDCRN
	__vo uint32_t PUCRO;			//0x03C PWR_PUCRO
	__vo uint32_t PDCRO;			//0x040 PWR_PDCRO
	__vo uint32_t PDCRP;			//0x044 PWR_PDCRP
} PWR_RegDef_t;

/* -------------------------------------------------------------------------- */
/* RAMECC register layout                                                     */
/* -------------------------------------------------------------------------- */
/* RAMECC register map. */
typedef struct{
	__vo uint32_t IER;			//0x000 RAMECC_IER
	uint32_t reserved_1[31];		//0x004-0x07C ---
	__vo uint32_t ECC;			//0x080 ECC
} RAMECC_RegDef_t;

/* -------------------------------------------------------------------------- */
/* RCC register layout                                                        */
/* -------------------------------------------------------------------------- */
/* RCC register map. */
typedef struct{
	__vo uint32_t CR;          		//0x000 Source Control Register
	__vo uint32_t HSICFGR;     		//0x004 HSI Calibration Register
	__vo uint32_t CRRCR;       		//0x008 Clock Recovery RC Register
	__vo uint32_t CSICFGR;     		//0x00C CSI Calibration Register
	__vo uint32_t CFGR;        		//0x010 Clock Configuration Register
	uint32_t reserved_1;  		//0x014 ---
	__vo uint32_t CDCFGR;      		//0x018 CPU Domain Clock Configuration Register
	__vo uint32_t BMCFGR;      		//0x01C AHB Clock Configuration Register
	__vo uint32_t APBCFGR;     		//0x020 APB Clocks Configuration Register
	uint32_t reserved_2;  		//0x024 ---
	__vo uint32_t PLLCKSELR;   		//0x028 PLLs Clock Source Selection Register
	__vo uint32_t PLLCFGR;     		//0x02C PLLs Configuration Register
	__vo uint32_t PLL1DIVR1;   		//0x030 PLL1 Dividers Register 1
	__vo uint32_t PLL1FRACR;   		//0x034 PLL1 Fractional Register
	__vo uint32_t PLL2DIVR1;   		//0x038 PLL2 Dividers Register 1
	__vo uint32_t PLL2FRACR;   		//0x03C PLL2 Fractional Register
	__vo uint32_t PLL3DIVR1;   		//0x040 PLL3 Dividers Register 1
	__vo uint32_t PLL3FRACR;   		//0x044 PLL3 Fractional Register
	uint32_t reserved_3;  		//0x048 ---
	__vo uint32_t CCIPR1;      		//0x04C Peripherals Clock Configuration Register 1
	__vo uint32_t CCIPR2;      		//0x050 Peripherals Clock Configuration Register 2
	__vo uint32_t CCIPR3;      		//0x054 Peripherals Clock Configuration Register 3
	__vo uint32_t CCIPR4;      		//0x058 Peripherals Clock Configuration Register 4
	uint32_t reserved_4;  		//0x05C ---
	__vo uint32_t CIER;        		//0x060 Clock Interrupt Enable Register
	__vo uint32_t CIFR;        		//0x064 Clock Interrupt Flag Register
	__vo uint32_t CICR;        		//0x068 Clock Interrupt Clear Register
	uint32_t reserved_5;  		//0x06C ---
	__vo uint32_t BDCR;        		//0x070 Backup Domain Control Register
	__vo uint32_t CSR;         		//0x074 Control/Status Register
	uint32_t reserved_6;  		//0x078 ---
	__vo uint32_t AHB5RSTR;    		//0x07C AHB5 Peripheral Reset Register
	__vo uint32_t AHB1RSTR;    		//0x080 AHB1 Peripheral Reset Register
	__vo uint32_t AHB2RSTR;    		//0x084 AHB2 Peripheral Reset Register
	__vo uint32_t AHB4RSTR;    		//0x088 AHB4 Peripheral Reset Register
	__vo uint32_t APB5RSTR;    		//0x08C APB5 Peripheral Reset Register
	__vo uint32_t APB1RSTR1;   		//0x090 APB1 Peripheral Reset Register 1
	__vo uint32_t APB1RSTR2;   		//0x094 APB1 Peripheral Reset Register 2
	__vo uint32_t APB2RSTR;    		//0x098 APB2 Peripheral Reset Register
	__vo uint32_t APB4RSTR;    		//0x09C APB4 Peripheral Reset Register
	uint32_t reserved_7;  		//0x0A0 ---
	__vo uint32_t AHB3RSTR;    		//0x0A4 AHB3 Peripheral Reset Register
	uint32_t reserved_8[2];   	//0x0A8-0x0AC ---
	__vo uint32_t CKGDISR;     		//0x0B0 Clock Gating Disable Register
	uint32_t reserved_9[3];   	//0x0B4-0x0BC ---
	__vo uint32_t PLL1DIVR2;   		//0x0C0 PLL1 Dividers Register 2
	__vo uint32_t PLL2DIVR2;   		//0x0C4 PLL2 Dividers Register 2
	__vo uint32_t PLL3DIVR2;   		//0x0C8 PLL3 Dividers Register 2
	__vo uint32_t PLL1SSCGR;   		//0x0CC PLL1 Spread Spectrum Clock Generation Register
	__vo uint32_t PLL2SSCGR;   		//0x0D0 PLL2 Spread Spectrum Clock Generation Register
	__vo uint32_t PLL3SSCGR;  		//0x0D4 PLL3 Spread Spectrum Clock Generation Register
	uint32_t reserved_10[10]; 	//0x0D8-0x0FC ---
	__vo uint32_t CKPROTR;     		//0x100 Clock Protection Register
	uint32_t reserved_11[11]; 	//0x104-0x12C ---
	__vo uint32_t RSR;         		//0x130 Reset Status Register
	__vo uint32_t AHB5ENR;     		//0x134 AHB5 Peripheral Clock Enable Register
	__vo uint32_t AHB1ENR;     		//0x138 AHB1 Peripheral Clock Enable Register
	__vo uint32_t AHB2ENR;     		//0x13C AHB2 Peripheral Clock Enable Register
	__vo uint32_t AHB4ENR;     		//0x140 AHB4 Peripheral Clock Enable Register
	__vo uint32_t APB5ENR;     		//0x144 APB5 Peripheral Clock Enable Register
	__vo uint32_t APB1ENR1;    		//0x148 APB1 Peripheral Clock Enable Register 1
	__vo uint32_t APB1ENR2;    		//0x14C APB1 Peripheral Clock Enable Register 2
	__vo uint32_t APB2ENR;     		//0x150 APB2 Peripheral Clock Enable Register
	__vo uint32_t APB4ENR;     		//0x154 APB4 Peripheral Clock Enable Register
	__vo uint32_t AHB3ENR;     		//0x158 AHB3 Peripheral Clock Enable Register
	__vo uint32_t AHB5LPENR;   		//0x15C AHB5 Peripheral Clock Enable In Low Power Mode Register
	__vo uint32_t AHB1LPENR;   		//0x160 AHB1 Peripheral Clock Enable In Low Power Mode Register
	__vo uint32_t AHB2LPENR;   		//0x164 AHB2 Peripheral Clock Enable In Low Power Mode Register
	__vo uint32_t AHB4LPENR;   		//0x168 AHB4 Peripheral Clock Enable In Low Power Mode Register
	__vo uint32_t AHB3LPENR;   		//0x16C AHB3 Peripheral Clock Enable In Low Power Mode Register
	__vo uint32_t APB1LPENR1;  		//0x170 APB1 Peripheral Clock Enable In Low Power Mode Register 1
	__vo uint32_t APB1LPENR2;  		//0x174 APB1 Peripheral Clock Enable In Low Power Mode Register 2
	__vo uint32_t APB2LPENR;   		//0x178 APB2 Peripheral Clock Enable In Low Power Mode Register
	__vo uint32_t APB4LPENR;   		//0x17C APB4 Peripheral Clock Enable In Low Power Mode Register
	__vo uint32_t APB5LPENR;   		//0x180 APB5 Peripheral Clock Enable In Low Power Mode Register
	uint32_t reserved_12[31]; 	//0x184-0x1FC ---
	__vo uint32_t TESTCR;      		//0x200 Test Control Register
} RCC_RegDef_t;

/* -------------------------------------------------------------------------- */
/* RNG register layout                                                        */
/* -------------------------------------------------------------------------- */
/* RNG register map. */
typedef struct{
	__vo uint32_t CR;			//0x000 RNG_CR
	__vo uint32_t SR;			//0x004 RNG_SR
	__vo uint32_t DR;			//0x008 RNG_DR
	__vo uint32_t NSCR;			//0x00C RNG_NSCR
	__vo uint32_t HTCR;			//0x010 RNG_HTCR
} RNG_RegDef_t;

/* -------------------------------------------------------------------------- */
/* RTC register layout                                                        */
/* -------------------------------------------------------------------------- */
/* RTC register map. */
typedef struct{
	__vo uint32_t TR;			//0x000 RTC_TR
	__vo uint32_t DR;			//0x004 RTC_DR
	__vo uint32_t SSR;			//0x008 RTC_SSR
	__vo uint32_t ICSR;			//0x00C RTC_ICSR
	__vo uint32_t PRER;			//0x010 RTC_PRER
	__vo uint32_t WUTR;			//0x014 RTC_WUTR
	__vo uint32_t CR;			//0x018 RTC_CR
	__vo uint32_t PRIVCFGR;			//0x01C RTC_PRIVCFGR
	uint32_t reserved_1;			//0x020 ---
	__vo uint32_t WPR;			//0x024 RTC_WPR
	__vo uint32_t CALR;			//0x028 RTC_CALR
	__vo uint32_t SHIFTR;			//0x02C RTC_SHIFTR
	__vo uint32_t TSTR;			//0x030 RTC_TSTR
	__vo uint32_t TSDR;			//0x034 RTC_TSDR
	__vo uint32_t TSSSR;			//0x038 RTC_TSSSR
	uint32_t reserved_2;			//0x03C ---
	__vo uint32_t ALRMAR;			//0x040 RTC_ALRMAR
	__vo uint32_t ALRMASSR;			//0x044 RTC_ALRMASSR
	__vo uint32_t ALRMBR;			//0x048 RTC_ALRMBR
	__vo uint32_t ALRMBSSR;			//0x04C RTC_ALRMBSSR
	__vo uint32_t SR;			//0x050 RTC_SR
	__vo uint32_t MISR;			//0x054 RTC_MISR
	uint32_t reserved_3;			//0x058 ---
	__vo uint32_t SCR;			//0x05C RTC_SCR
	uint32_t reserved_4[4];		//0x060-0x06C ---
	__vo uint32_t ALRABINR;			//0x070 RTC_ALRABINR
	__vo uint32_t ALRBBINR;			//0x074 RTC_ALRBBINR
} RTC_RegDef_t;

/* -------------------------------------------------------------------------- */
/* SAES register layout                                                       */
/* -------------------------------------------------------------------------- */
/* SAES register map. */
typedef struct{
	__vo uint32_t CR;			//0x000 SAES_CR
	__vo uint32_t SR;			//0x004 SAES_SR
	__vo uint32_t DINR;			//0x008 SAES_DINR
	__vo uint32_t DOUTR;			//0x00C SAES_DOUTR
	__vo uint32_t KEYR0;			//0x010 SAES_KEYR0
	__vo uint32_t KEYR1;			//0x014 SAES_KEYR1
	__vo uint32_t KEYR2;			//0x018 SAES_KEYR2
	__vo uint32_t KEYR3;			//0x01C SAES_KEYR3
	__vo uint32_t IVR0;			//0x020 SAES_IVR0
	__vo uint32_t IVR1;			//0x024 SAES_IVR1
	__vo uint32_t IVR2;			//0x028 SAES_IVR2
	__vo uint32_t IVR3;			//0x02C SAES_IVR3
	__vo uint32_t KEYR4;			//0x030 SAES_KEYR4
	__vo uint32_t KEYR5;			//0x034 SAES_KEYR5
	__vo uint32_t KEYR6;			//0x038 SAES_KEYR6
	__vo uint32_t KEYR7;			//0x03C SAES_KEYR7
	uint32_t reserved_1[176];		//0x040-0x2FC ---
	__vo uint32_t IER;			//0x300 SAES_IER
	__vo uint32_t ISR;			//0x304 SAES_ISR
	__vo uint32_t ICR;			//0x308 SAES_ICR
} SAES_RegDef_t;

/* -------------------------------------------------------------------------- */
/* SAI register layout                                                        */
/* -------------------------------------------------------------------------- */
/* SAI register map. */
typedef struct{
	__vo uint32_t GCR;			//0x000 SAI_GCR
	__vo uint32_t ACR1;			//0x004 SAI_ACR1
	__vo uint32_t ACR2;			//0x008 SAI_ACR2
	__vo uint32_t AFRCR;			//0x00C SAI_AFRCR
	__vo uint32_t ASLOTR;			//0x010 SAI_ASLOTR
	__vo uint32_t AIM;			//0x014 SAI_AIM
	__vo uint32_t ASR;			//0x018 SAI_ASR
	__vo uint32_t ACLRFR;			//0x01C SAI_ACLRFR
	__vo uint32_t ADR;			//0x020 SAI_ADR
	__vo uint32_t BCR1;			//0x024 SAI_BCR1
	__vo uint32_t BCR2;			//0x028 SAI_BCR2
	__vo uint32_t BFRCR;			//0x02C SAI_BFRCR
	__vo uint32_t BSLOTR;			//0x030 SAI_BSLOTR
	__vo uint32_t BIM;			//0x034 SAI_BIM
	__vo uint32_t BSR;			//0x038 SAI_BSR
	__vo uint32_t BCLRFR;			//0x03C SAI_BCLRFR
	__vo uint32_t BDR;			//0x040 SAI_BDR
	__vo uint32_t PDMCR;			//0x044 SAI_PDMCR
	__vo uint32_t PDMDLY;			//0x048 SAI_PDMDLY
} SAI_RegDef_t;

/* -------------------------------------------------------------------------- */
/* SBS register layout                                                        */
/* -------------------------------------------------------------------------- */
/* SBS register map. */
typedef struct{
	__vo uint32_t BOOTSR;			//0x000 SBS_BOOTSR
	uint32_t reserved_1[3];		//0x004-0x00C ---
	__vo uint32_t HDPLCR;			//0x010 SBS_HDPLCR
	__vo uint32_t HDPLSR;			//0x014 SBS_HDPLSR
	uint32_t reserved_2[2];		//0x018-0x01C ---
	__vo uint32_t DBGCR;			//0x020 SBS_DBGCR
	__vo uint32_t DBGLOCKR;			//0x024 SBS_DBGLOCKR
	uint32_t reserved_3[3];		//0x028-0x030 ---
	__vo uint32_t RSSCMDR;			//0x034 SBS_RSSCMDR
	uint32_t reserved_4[50];		//0x038-0x0FC ---
	__vo uint32_t PMCR;			//0x100 SBS_PMCR
	__vo uint32_t FPUIMR;			//0x104 SBS_FPUIMR
	__vo uint32_t MESR;			//0x108 SBS_MESR
	uint32_t reserved_5;			//0x10C ---
	__vo uint32_t CCCSR;			//0x110 SBS_CCCSR
	__vo uint32_t CCVALR;			//0x114 SBS_CCVALR
	__vo uint32_t CCSWVALR;			//0x118 SBS_CCSWVALR
	uint32_t reserved_6;			//0x11C ---
	__vo uint32_t BKLOCKR;			//0x120 SBS_BKLOCKR
	uint32_t reserved_7[3];		//0x124-0x12C ---
	__vo uint32_t EXTICR[4];			//0x130 SBS_EXTICR1 | 0x134 SBS_EXTICR2 | 0x138 SBS_EXTICR3 | 0x13C SBS_EXTICR4
} SBS_RegDef_t;

/* -------------------------------------------------------------------------- */
/* SDMMC register layout                                                      */
/* -------------------------------------------------------------------------- */
/* SDMMC register map. */
typedef struct{
	__vo uint32_t POWER;			//0x000 SDMMC_POWER
	__vo uint32_t CLKCR;			//0x004 SDMMC_CLKCR
	__vo uint32_t ARGR;			//0x008 SDMMC_ARGR
	__vo uint32_t CMDR;			//0x00C SDMMC_CMDR
	__vo uint32_t RESPCMDR;			//0x010 SDMMC_RESPCMDR
	uint32_t reserved_1[4];		//0x014-0x020 ---
	__vo uint32_t DTIMER;			//0x024 SDMMC_DTIMER
	__vo uint32_t DLENR;			//0x028 SDMMC_DLENR
	__vo uint32_t DCTRL;			//0x02C SDMMC_DCTRL
	__vo uint32_t DCNTR;			//0x030 SDMMC_DCNTR
	__vo uint32_t STAR;			//0x034 SDMMC_STAR
	__vo uint32_t ICR;			//0x038 SDMMC_ICR
	__vo uint32_t MASKR;			//0x03C SDMMC_MASKR
	__vo uint32_t ACKTIMER;			//0x040 SDMMC_ACKTIMER
	uint32_t reserved_2[3];		//0x044-0x04C ---
	__vo uint32_t IDMACTRLR;			//0x050 SDMMC_IDMACTRLR
	__vo uint32_t IDMABSIZER;			//0x054 SDMMC_IDMABSIZER
	__vo uint32_t IDMABASER;			//0x058 SDMMC_IDMABASER
	uint32_t reserved_3[2];		//0x05C-0x060 ---
	__vo uint32_t IDMALAR;			//0x064 SDMMC_IDMALAR
	__vo uint32_t IDMABAR;			//0x068 SDMMC_IDMABAR
} SDMMC_RegDef_t;

/* -------------------------------------------------------------------------- */
/* SPDIFRX register layout                                                    */
/* -------------------------------------------------------------------------- */
/* SPDIFRX register map. */
typedef struct{
	__vo uint32_t CR;			//0x000 SPDIFRX_CR
	__vo uint32_t IMR;			//0x004 SPDIFRX_IMR
	__vo uint32_t SR;			//0x008 SPDIFRX_SR
	__vo uint32_t IFCR;			//0x00C SPDIFRX_IFCR
	__vo uint32_t FMT0_DR;			//0x010 SPDIFRX_FMT0_DR
	__vo uint32_t CSR;			//0x014 SPDIFRX_CSR
	__vo uint32_t DIR;			//0x018 SPDIFRX_DIR
} SPDIFRX_RegDef_t;

/* -------------------------------------------------------------------------- */
/* SPI register layout                                                        */
/* -------------------------------------------------------------------------- */
/* SPI register map. */
typedef struct{
	__vo uint32_t CR1;			//0x000 SPI_CR1
	__vo uint32_t CR2;			//0x004 SPI_CR2
	__vo uint32_t CFG1;			//0x008 SPI_CFG1
	__vo uint32_t CFG2;			//0x00C SPI_CFG2
	__vo uint32_t IER;			//0x010 SPI_IER
	__vo uint32_t SR;			//0x014 SPI_SR
	__vo uint32_t IFCR;			//0x018 SPI_IFCR
	uint32_t reserved_1;			//0x01C ---
	__vo uint32_t TXDR;			//0x020 SPI_TXDR
	uint32_t reserved_2[3];		//0x024-0x02C ---
	__vo uint32_t RXDR;			//0x030 SPI_RXDR
	uint32_t reserved_3[3];		//0x034-0x03C ---
	__vo uint32_t CRCPOLY;			//0x040 SPI_CRCPOLY
	__vo uint32_t TXCRC;			//0x044 SPI_TXCRC
	__vo uint32_t RXCRC;			//0x048 SPI_RXCRC
	__vo uint32_t UDRDR;			//0x04C SPI_UDRDR
	__vo uint32_t I2SCFGR;			//0x050 SPI_I2SCFGR
} SPI_RegDef_t;

/* -------------------------------------------------------------------------- */
/* TAMP register layout                                                       */
/* -------------------------------------------------------------------------- */
/* TAMP register map. */
typedef struct{
	__vo uint32_t CR1;			//0x000 TAMP_CR1
	__vo uint32_t CR2;			//0x004 TAMP_CR2
	__vo uint32_t CR3;			//0x008 TAMP_CR3
	__vo uint32_t FLTCR;			//0x00C TAMP_FLTCR
	__vo uint32_t ATCR1;			//0x010 TAMP_ATCR1
	__vo uint32_t ATSEEDR;			//0x014 TAMP_ATSEEDR
	__vo uint32_t ATOR;			//0x018 TAMP_ATOR
	__vo uint32_t ATCR2;			//0x01C TAMP_ATCR2
	__vo uint32_t CFGR;			//0x020 TAMP_CFGR
	__vo uint32_t PRIVCFGR;			//0x024 TAMP_PRIVCFGR
	uint32_t reserved_1;			//0x028 ---
	__vo uint32_t IER;			//0x02C TAMP_IER
	__vo uint32_t SR;			//0x030 TAMP_SR
	__vo uint32_t MISR;			//0x034 TAMP_MISR
	uint32_t reserved_2;			//0x038 ---
	__vo uint32_t SCR;			//0x03C TAMP_SCR
	__vo uint32_t COUNT1R;			//0x040 TAMP_COUNT1R
	uint32_t reserved_3[4];		//0x044-0x050 ---
	__vo uint32_t RPCFGR;			//0x054 TAMP_RPCFGR
} TAMP_RegDef_t;

/* -------------------------------------------------------------------------- */
/* TIM register layout                                                        */
/* -------------------------------------------------------------------------- */
/* TIM register map. */
typedef struct{
	__vo uint32_t CR1;			//0x000 TIMx_CR1
	__vo uint32_t CR2;			//0x004 TIMx_CR2
	__vo uint32_t SMCR;			//0x008 TIMx_SMCR
	__vo uint32_t DIER;			//0x00C TIMx_DIER
	__vo uint32_t SR;			//0x010 TIMx_SR
	__vo uint32_t EGR;			//0x014 TIMx_EGR
	__vo uint32_t CCMR1;			//0x018 TIMx_CCMR1
	__vo uint32_t CCMR2;			//0x01C TIMx_CCMR2
	__vo uint32_t CCER;			//0x020 TIMx_CCER
	__vo uint32_t CNT;			//0x024 TIMx_CNT
	__vo uint32_t PSC;			//0x028 TIMx_PSC
	__vo uint32_t ARR;			//0x02C TIMx_ARR
	__vo uint32_t RCR;			//0x030 TIMx_RCR
	__vo uint32_t CCR1;			//0x034 TIMx_CCR1
	__vo uint32_t CCR2;			//0x038 TIMx_CCR2
	__vo uint32_t CCR3;			//0x03C TIMx_CCR3
	__vo uint32_t CCR4;			//0x040 TIMx_CCR4
	__vo uint32_t BDTR;			//0x044 TIMx_BDTR
	uint32_t reserved_1[3];		//0x048-0x050 ---
	__vo uint32_t DTR2;			//0x054 TIMx_DTR2
	__vo uint32_t ECR;			//0x058 TIMx_ECR
	__vo uint32_t TISEL;			//0x05C TIMx_TISEL
	__vo uint32_t AF1;			//0x060 TIMx_AF1
	__vo uint32_t AF2;			//0x064 TIMx_AF2
	uint32_t reserved_2[221];		//0x068-0x3D8 ---
	__vo uint32_t DCR;			//0x3DC TIMx_DCR
	__vo uint32_t DMAR;			//0x3E0 TIMx_DMAR
} TIM_RegDef_t;

/* -------------------------------------------------------------------------- */
/* TIM1 register layout                                                       */
/* -------------------------------------------------------------------------- */
/* TIM1 register map. */
typedef struct{
	__vo uint32_t CR1;			//0x000 TIM1_CR1
	__vo uint32_t CR2;			//0x004 TIM1_CR2
	__vo uint32_t SMCR;			//0x008 TIM1_SMCR
	__vo uint32_t DIER;			//0x00C TIM1_DIER
	__vo uint32_t SR;			//0x010 TIM1_SR
	__vo uint32_t EGR;			//0x014 TIM1_EGR
	__vo uint32_t CCMR1;			//0x018 TIM1_CCMR1
	__vo uint32_t CCMR2;			//0x01C TIM1_CCMR2
	__vo uint32_t CCER;			//0x020 TIM1_CCER
	__vo uint32_t CNT;			//0x024 TIM1_CNT
	__vo uint32_t PSC;			//0x028 TIM1_PSC
	__vo uint32_t ARR;			//0x02C TIM1_ARR
	__vo uint32_t RCR;			//0x030 TIM1_RCR
	__vo uint32_t CCR1;			//0x034 TIM1_CCR1
	__vo uint32_t CCR2;			//0x038 TIM1_CCR2
	__vo uint32_t CCR3;			//0x03C TIM1_CCR3
	__vo uint32_t CCR4;			//0x040 TIM1_CCR4
	__vo uint32_t BDTR;			//0x044 TIM1_BDTR
	__vo uint32_t CCR5;			//0x048 TIM1_CCR5
	__vo uint32_t CCR6;			//0x04C TIM1_CCR6
	__vo uint32_t CCMR3;			//0x050 TIM1_CCMR3
	__vo uint32_t DTR2;			//0x054 TIM1_DTR2
	__vo uint32_t ECR;			//0x058 TIM1_ECR
	__vo uint32_t TISEL;			//0x05C TIM1_TISEL
	__vo uint32_t AF1;			//0x060 TIM1_AF1
	__vo uint32_t AF2;			//0x064 TIM1_AF2
	uint32_t reserved_1[221];		//0x068-0x3D8 ---
	__vo uint32_t DCR;			//0x3DC TIM1_DCR
	__vo uint32_t DMAR;			//0x3E0 TIM1_DMAR
} TIM1_RegDef_t;

/* -------------------------------------------------------------------------- */
/* TIM15 register layout                                                      */
/* -------------------------------------------------------------------------- */
/* TIM15 register map. */
typedef struct{
	__vo uint32_t CR1;			//0x000 TIM15_CR1
	__vo uint32_t CR2;			//0x004 TIM15_CR2
	__vo uint32_t SMCR;			//0x008 TIM15_SMCR
	__vo uint32_t DIER;			//0x00C TIM15_DIER
	__vo uint32_t SR;			//0x010 TIM15_SR
	__vo uint32_t EGR;			//0x014 TIM15_EGR
	__vo uint32_t CCMR1;			//0x018 TIM15_CCMR1
	uint32_t reserved_1;			//0x01C ---
	__vo uint32_t CCER;			//0x020 TIM15_CCER
	__vo uint32_t CNT;			//0x024 TIM15_CNT
	__vo uint32_t PSC;			//0x028 TIM15_PSC
	__vo uint32_t ARR;			//0x02C TIM15_ARR
	__vo uint32_t RCR;			//0x030 TIM15_RCR
	__vo uint32_t CCR1;			//0x034 TIM15_CCR1
	__vo uint32_t CCR2;			//0x038 TIM15_CCR2
	uint32_t reserved_2[2];		//0x03C-0x040 ---
	__vo uint32_t BDTR;			//0x044 TIM15_BDTR
	uint32_t reserved_3[3];		//0x048-0x050 ---
	__vo uint32_t DTR2;			//0x054 TIM15_DTR2
	uint32_t reserved_4;			//0x058 ---
	__vo uint32_t TISEL;			//0x05C TIM15_TISEL
	__vo uint32_t AF1;			//0x060 TIM15_AF1
	__vo uint32_t AF2;			//0x064 TIM15_AF2
	uint32_t reserved_5[221];		//0x068-0x3D8 ---
	__vo uint32_t DCR;			//0x3DC TIM15_DCR
	__vo uint32_t DMAR;			//0x3E0 TIM15_DMAR
} TIM15_RegDef_t;

/* -------------------------------------------------------------------------- */
/* UCPD register layout                                                       */
/* -------------------------------------------------------------------------- */
/* UCPD register map. */
typedef struct{
	__vo uint32_t CFGR1;			//0x000 UCPD_CFGR1
	__vo uint32_t CFGR2;			//0x004 UCPD_CFGR2
	__vo uint32_t CFGR3;			//0x008 UCPD_CFGR3
	__vo uint32_t CR;			//0x00C UCPD_CR
	__vo uint32_t IMR;			//0x010 UCPD_IMR
	__vo uint32_t SR;			//0x014 UCPD_SR
	__vo uint32_t ICR;			//0x018 UCPD_ICR
	__vo uint32_t TX_ORDSETR;			//0x01C UCPD_TX_ORDSETR
	__vo uint32_t TX_PAYSZR;			//0x020 UCPD_TX_PAYSZR
	__vo uint32_t TXDR;			//0x024 UCPD_TXDR
	__vo uint32_t RX_ORDSETR;			//0x028 UCPD_RX_ORDSETR
	__vo uint32_t RX_PAYSZR;			//0x02C UCPD_RX_PAYSZR
	__vo uint32_t RXDR;			//0x030 UCPD_RXDR
	__vo uint32_t RX_ORDEXTR1;			//0x034 UCPD_RX_ORDEXTR1
	__vo uint32_t RX_ORDEXTR2;			//0x038 UCPD_RX_ORDEXTR2
} UCPD_RegDef_t;

/* -------------------------------------------------------------------------- */
/* USART register layout                                                      */
/* -------------------------------------------------------------------------- */
/* USART register map. */
typedef struct{
	__vo uint32_t CR1;			//0x000 USART_CR1
	__vo uint32_t CR2;			//0x004 USART_CR2
	__vo uint32_t CR3;			//0x008 USART_CR3
	__vo uint32_t BRR;			//0x00C USART_BRR
	__vo uint32_t GTPR;			//0x010 USART_GTPR
	__vo uint32_t RTOR;			//0x014 USART_RTOR
	__vo uint32_t RQR;			//0x018 USART_RQR
	__vo uint32_t ISR;			//0x01C USART_ISR
	__vo uint32_t ICR;			//0x020 USART_ICR
	__vo uint32_t RDR;			//0x024 USART_RDR
	__vo uint32_t TDR;			//0x028 USART_TDR
	__vo uint32_t PRESC;			//0x02C USART_PRESC
} USART_RegDef_t;

/* -------------------------------------------------------------------------- */
/* VREFBUF register layout                                                    */
/* -------------------------------------------------------------------------- */
/* VREFBUF register map. */
typedef struct{
	__vo uint32_t CSR;			//0x000 VREFBUF_CSR
	__vo uint32_t CCR;			//0x004 VREFBUF_CCR
} VREFBUF_RegDef_t;

/* -------------------------------------------------------------------------- */
/* WWDG register layout                                                       */
/* -------------------------------------------------------------------------- */
/* WWDG register map. */
typedef struct{
	__vo uint32_t CR;			//0x000 WWDG_CR
	__vo uint32_t CFR;			//0x004 WWDG_CFR
	__vo uint32_t SR;			//0x008 WWDG_SR
} WWDG_RegDef_t;

/* -------------------------------------------------------------------------- */
/* XSPI register layout                                                       */
/* -------------------------------------------------------------------------- */
/* XSPI register map. */
typedef struct{
	__vo uint32_t CR;			//0x000 XSPI_CR
	uint32_t reserved_1;			//0x004 ---
	__vo uint32_t DCR1;			//0x008 XSPI_DCR1
	__vo uint32_t DCR2;			//0x00C XSPI_DCR2
	__vo uint32_t DCR3;			//0x010 XSPI_DCR3
	__vo uint32_t DCR4;			//0x014 XSPI_DCR4
	uint32_t reserved_2[2];		//0x018-0x01C ---
	__vo uint32_t SR;			//0x020 XSPI_SR
	__vo uint32_t FCR;			//0x024 XSPI_FCR
	uint32_t reserved_3[6];		//0x028-0x03C ---
	__vo uint32_t DLR;			//0x040 XSPI_DLR
	uint32_t reserved_4;			//0x044 ---
	__vo uint32_t AR;			//0x048 XSPI_AR
	uint32_t reserved_5;			//0x04C ---
	__vo uint32_t DR;			//0x050 XSPI_DR
	uint32_t reserved_6[11];		//0x054-0x07C ---
	__vo uint32_t PSMKR;			//0x080 XSPI_PSMKR
	uint32_t reserved_7;			//0x084 ---
	__vo uint32_t PSMAR;			//0x088 XSPI_PSMAR
	uint32_t reserved_8;			//0x08C ---
	__vo uint32_t PIR;			//0x090 XSPI_PIR
	uint32_t reserved_9[27];		//0x094-0x0FC ---
	__vo uint32_t CCR;			//0x100 XSPI_CCR
	uint32_t reserved_10;			//0x104 ---
	__vo uint32_t TCR;			//0x108 XSPI_TCR
	uint32_t reserved_11;			//0x10C ---
	__vo uint32_t IR;			//0x110 XSPI_IR
	uint32_t reserved_12[3];		//0x114-0x11C ---
	__vo uint32_t ABR;			//0x120 XSPI_ABR
	uint32_t reserved_13[3];		//0x124-0x12C ---
	__vo uint32_t LPTR;			//0x130 XSPI_LPTR
	uint32_t reserved_14[3];		//0x134-0x13C ---
	__vo uint32_t WPCCR;			//0x140 XSPI_WPCCR
	uint32_t reserved_15;			//0x144 ---
	__vo uint32_t WPTCR;			//0x148 XSPI_WPTCR
	uint32_t reserved_16;			//0x14C ---
	__vo uint32_t WPIR;			//0x150 XSPI_WPIR
	uint32_t reserved_17[3];		//0x154-0x15C ---
	__vo uint32_t WPABR;			//0x160 XSPI_WPABR
	uint32_t reserved_18[7];		//0x164-0x17C ---
	__vo uint32_t WCCR;			//0x180 XSPI_WCCR
	uint32_t reserved_19;			//0x184 ---
	__vo uint32_t WTCR;			//0x188 XSPI_WTCR
	uint32_t reserved_20;			//0x18C ---
	__vo uint32_t WIR;			//0x190 XSPI_WIR
	uint32_t reserved_21[3];		//0x194-0x19C ---
	__vo uint32_t WABR;			//0x1A0 XSPI_WABR
	uint32_t reserved_22[23];		//0x1A4-0x1FC ---
	__vo uint32_t HLCR;			//0x200 XSPI_HLCR
	uint32_t reserved_23[3];		//0x204-0x20C ---
	__vo uint32_t CALFCR;			//0x210 XSPI_CALFCR
	uint32_t reserved_24;			//0x214 ---
	__vo uint32_t CALMR;			//0x218 XSPI_CALMR
	uint32_t reserved_25;			//0x21C ---
	__vo uint32_t CALSOR;			//0x220 XSPI_CALSOR
	uint32_t reserved_26;			//0x224 ---
	__vo uint32_t CALSIR;			//0x228 XSPI_CALSIR
} XSPI_RegDef_t;

/* -------------------------------------------------------------------------- */
/* XSPIM register layout                                                      */
/* -------------------------------------------------------------------------- */
/* XSPIM register map. */
typedef struct{
	__vo uint32_t CR;			//0x000 XSPIM_CR
} XSPIM_RegDef_t;


/* -------------------------------------------------------------------------- */
/* Peripheral instance definitions                                            */
/* -------------------------------------------------------------------------- */
#define GPIOA							((GPIO_RegDef_t*)GPIOA_BASEADDR)
#define GPIOB                           ((GPIO_RegDef_t*)GPIOB_BASEADDR)
#define GPIOC                           ((GPIO_RegDef_t*)GPIOC_BASEADDR)
#define GPIOE                           ((GPIO_RegDef_t*)GPIOE_BASEADDR)
#define GPIOD							((GPIO_RegDef_t*)GPIOD_BASEADDR)
#define GPIOF                           ((GPIO_RegDef_t*)GPIOF_BASEADDR)
#define GPIOG                           ((GPIO_RegDef_t*)GPIOG_BASEADDR)
#define GPIOH                           ((GPIO_RegDef_t*)GPIOH_BASEADDR)
#define GPIOM                           ((GPIO_RegDef_t*)GPIOM_BASEADDR)
#define GPION                           ((GPIO_RegDef_t*)GPION_BASEADDR)
#define GPIOO                           ((GPIO_RegDef_t*)GPIOO_BASEADDR)
#define GPIOP                           ((GPIO_RegDef_t*)GPIOP_BASEADDR)

#define TIM1                            ((TIM1_RegDef_t*)TIM1_BASEADDR)
#define TIM2                            ((TIM_RegDef_t*)TIM2_BASEADDR)
#define TIM3                            ((TIM_RegDef_t*)TIM3_BASEADDR)
#define TIM4                            ((TIM_RegDef_t*)TIM4_BASEADDR)
#define TIM5                            ((TIM_RegDef_t*)TIM5_BASEADDR)
#define TIM6                            ((TIM_RegDef_t*)TIM6_BASEADDR)
#define TIM7                            ((TIM_RegDef_t*)TIM7_BASEADDR)
#define TIM12                           ((TIM_RegDef_t*)TIM12_BASEADDR)
#define TIM13                           ((TIM_RegDef_t*)TIM13_BASEADDR)
#define TIM14                           ((TIM_RegDef_t*)TIM14_BASEADDR)
#define TIM15                           ((TIM15_RegDef_t*)TIM15_BASEADDR)
#define TIM16                           ((TIM15_RegDef_t*)TIM16_BASEADDR)
#define TIM17                           ((TIM15_RegDef_t*)TIM17_BASEADDR)
#define TIM9                            ((TIM_RegDef_t*)TIM9_BASEADDR)

#define LPTIM1                          ((LPTIM_RegDef_t*)LPTIM1_BASEADDR)
#define LPTIM2                          ((LPTIM_RegDef_t*)LPTIM2_BASEADDR)
#define LPTIM3                          ((LPTIM_RegDef_t*)LPTIM3_BASEADDR)
#define LPTIM4                          ((LPTIM_RegDef_t*)LPTIM4_BASEADDR)
#define LPTIM5                          ((LPTIM_RegDef_t*)LPTIM5_BASEADDR)
#define WWDG                            ((WWDG_RegDef_t*)WWDG_BASEADDR)
#define IWDG                            ((IWDG_RegDef_t*)IWDG_BASEADDR)
#define RTC                             ((RTC_RegDef_t*)RTC_BASEADDR)
#define TAMP                            ((TAMP_RegDef_t*)TAMP_BASEADDR)

#define I2C1                            ((I2C_RegDef_t*)I2C1_BASEADDR)
#define I2C2                            ((I2C_RegDef_t*)I2C2_BASEADDR)
#define I2C3                            ((I2C_RegDef_t*)I2C3_BASEADDR)
#define I3C1                            ((I3C_RegDef_t*)I3C1_BASEADDR)
#define USART1                          ((USART_RegDef_t*)USART1_BASEADDR)
#define USART2                          ((USART_RegDef_t*)USART2_BASEADDR)
#define USART3                          ((USART_RegDef_t*)USART3_BASEADDR)
#define UART4                           ((USART_RegDef_t*)UART4_BASEADDR)
#define UART5                           ((USART_RegDef_t*)UART5_BASEADDR)
#define UART7                           ((USART_RegDef_t*)UART7_BASEADDR)
#define UART8                           ((USART_RegDef_t*)UART8_BASEADDR)
#define LPUART1                         ((LPUART_RegDef_t*)LPUART1_BASEADDR)

#define SPI1                            ((SPI_RegDef_t*)SPI2S1_BASEADDR)
#define SPI2                            ((SPI_RegDef_t*)SPI2S2_BASEADDR)
#define SPI3                            ((SPI_RegDef_t*)SPI2S3_BASEADDR)
#define SPI4                            ((SPI_RegDef_t*)SPI4_BASEADDR)
#define SPI5                            ((SPI_RegDef_t*)SPI5_BASEADDR)
#define SPI6                            ((SPI_RegDef_t*)SPI2S6_BASEADDR)
#define SAI1                            ((SAI_RegDef_t*)SAI1_BASEADDR)
#define SAI2                            ((SAI_RegDef_t*)SAI2_BASEADDR)
#define SPDIFRX1                        ((SPDIFRX_RegDef_t*)SPDIFRX1_BASEADDR)
#define CEC                             ((CEC_RegDef_t*)CEC_BASEADDR)
#define CRS                             ((CRS_RegDef_t*)CRS_BASEADDR)
#define MDIOS                           ((MDIOS_RegDef_t*)MDIOS_BASEADDR)
#define FDCAN1                          ((FDCAN_RegDef_t*)FDCAN1_BASEADDR)
#define FDCAN2                          ((FDCAN_RegDef_t*)FDCAN2_BASEADDR)
#define UCPD1                           ((UCPD_RegDef_t*)UCPD1_BASEADDR)

#define GPDMA1                          ((GPDMA_RegDef_t*)GPDMA1_BASEADDR)
#define ADC1_ADC2                       ((ADC_RegDef_t*)ADC1_ADC2_BASEADDR)
#define ETH		                        ((ETH_RegDef_t*)ETH1_BASEADDR)
#define ADF1                            ((ADF_RegDef_t*)ADF1_BASEADDR)
#define OTG_HS                          ((OTG_RegDef_t*)OTG_HS_BASEADDR)
#define OTG_FS                          ((OTG_RegDef_t*)OTG_FS_BASEADDR)

#define PSSI                            ((PSSI_RegDef_t*)PSSI_BASEADDR)
#define SDMMC1                          ((SDMMC_RegDef_t*)SDMMC1_BASEADDR)
#define SDMMC2                          ((SDMMC_RegDef_t*)SDMMC2_BASEADDR)
#define DLYB_SDMMC1                     ((DLYB_RegDef_t*)DLYB_SDMMC1_BASEADDR)
#define DLYB_SDMMC2                     ((DLYB_RegDef_t*)DLYB_SDMMC2_BASEADDR)
#define CORDIC                          ((CORDIC_RegDef_t*)CORDIC_BASEADDR)

#define RNG                             ((RNG_RegDef_t*)RNG_BASEADDR)
#define HASH                            ((HASH_RegDef_t*)HASH_BASEADDR)
#define CRYP                            ((CRYP_RegDef_t*)CRYP_BASEADDR)
#define SAES                            ((SAES_RegDef_t*)SAES_BASEADDR)
#define PKA                             ((PKA_RegDef_t*)PKA_RAM_BASEADDR)

#define LTDC                            ((LTDC_RegDef_t*)LTDC_BASEADDR)
#define DCMIPP                          ((DCMIPP_RegDef_t*)DCMIPP_BASEADDR)
#define GFXTIM                          ((GFXTIM_RegDef_t*)GFXTIM_BASEADDR)
#define HPDMA1                          ((HPDMA_RegDef_t*)HPDMA1_BASEADDR)
#define DMA2D                           ((DMA2D_RegDef_t*)DMA2D_BASEADDR)
#define FLASH_REG                       ((FLASH_RegDef_t*)FLASH_REG_BASEADDR)
#define JPEG                            ((JPEG_RegDef_t*)JPEG_BASEADDR)
#define FMC                             ((FMC_RegDef_t*)FMC_BASEADDR)
#define XSPI1                           ((XSPI_RegDef_t*)XSPI1_BASEADDR)
#define XSPI2                           ((XSPI_RegDef_t*)XSPI2_BASEADDR)
#define XSPIM1                          ((XSPIM_RegDef_t*)XSPIM1_BASEADDR)
#define ECC_DIAG_D1                     ((RAMECC_RegDef_t*)ECC_DIAG_D1_BASEADDR)
#define ECC_DIAG_D2                     ((RAMECC_RegDef_t*)ECC_DIAG_D2_BASEADDR)
#define MCE1                            ((MCE_RegDef_t*)MCE1_BASEADDR)
#define MCE2                            ((MCE_RegDef_t*)MCE2_BASEADDR)
#define MCE3                            ((MCE_RegDef_t*)MCE3_BASEADDR)
#define GFXMMU                          ((GFXMMU_RegDef_t*)GFXMMU_BASEADDR)
#define GPU2D                           ((uint32_t*)GPU2D_BASEADDR)
#define ICACHE                          ((ICACHE_RegDef_t*)ICACHE_BASEADDR)

#define EXTI                            ((EXTI_RegDef_t*)EXTI_BASEADDR)
#define SBS                             ((SBS_RegDef_t*)SBS_BASEADDR)
#define VREFBUF                         ((VREFBUF_RegDef_t*)VREFBUF_BASEADDR)
#define DTS                             ((DTS_RegDef_t*)DTS_BASEADDR)
#define PWR                             ((PWR_RegDef_t*)PWR_BASEADDR)
#define RCC                             ((RCC_RegDef_t *)RCC_BASEADDR)
#define CRC                             ((CRC_RegDef_t*)CRC_BASEADDR)


/* -------------------------------------------------------------------------- */
/* CLOCK ENABLE DISABLE MACROS                                                */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* GPIOx clock enable macros                                                  */
/* -------------------------------------------------------------------------- */
#define GPIOA_PCLK_EN()                 (RCC->AHB4ENR |= (1UL << 0U))
#define GPIOB_PCLK_EN()                 (RCC->AHB4ENR |= (1UL << 1U))
#define GPIOC_PCLK_EN()                 (RCC->AHB4ENR |= (1UL << 2U))
#define GPIOD_PCLK_EN()                 (RCC->AHB4ENR |= (1UL << 3U))
#define GPIOE_PCLK_EN()                 (RCC->AHB4ENR |= (1UL << 4U))
#define GPIOF_PCLK_EN()                 (RCC->AHB4ENR |= (1UL << 5U))
#define GPIOG_PCLK_EN()                 (RCC->AHB4ENR |= (1UL << 6U))
#define GPIOH_PCLK_EN()                 (RCC->AHB4ENR |= (1UL << 7U))
#define GPIOM_PCLK_EN()                 (RCC->AHB4ENR |= (1UL << 12U))
#define GPION_PCLK_EN()                 (RCC->AHB4ENR |= (1UL << 13U))
#define GPIOO_PCLK_EN()                 (RCC->AHB4ENR |= (1UL << 14U))
#define GPIOP_PCLK_EN()                 (RCC->AHB4ENR |= (1UL << 15U))

#define GPIOA_PCLK_DI()                 (RCC->AHB4ENR &= ~(1UL << 0U))
#define GPIOB_PCLK_DI()                 (RCC->AHB4ENR &= ~(1UL << 1U))
#define GPIOC_PCLK_DI()                 (RCC->AHB4ENR &= ~(1UL << 2U))
#define GPIOD_PCLK_DI()                 (RCC->AHB4ENR &= ~(1UL << 3U))
#define GPIOE_PCLK_DI()                 (RCC->AHB4ENR &= ~(1UL << 4U))
#define GPIOF_PCLK_DI()                 (RCC->AHB4ENR &= ~(1UL << 5U))
#define GPIOG_PCLK_DI()                 (RCC->AHB4ENR &= ~(1UL << 6U))
#define GPIOH_PCLK_DI()                 (RCC->AHB4ENR &= ~(1UL << 7U))
#define GPIOM_PCLK_DI()                 (RCC->AHB4ENR &= ~(1UL << 12U))
#define GPION_PCLK_DI()                 (RCC->AHB4ENR &= ~(1UL << 13U))
#define GPIOO_PCLK_DI()                 (RCC->AHB4ENR &= ~(1UL << 14U))
#define GPIOP_PCLK_DI()                 (RCC->AHB4ENR &= ~(1UL << 15U))

/* -------------------------------------------------------------------------- */
/* TIMx clock enable macros                                                   */
/* -------------------------------------------------------------------------- */
#define TIM1_PCLK_EN()                  (RCC->APB2ENR |= (1UL << 0U))
#define TIM2_PCLK_EN()                  (RCC->APB1ENR1 |= (1UL << 0U))
#define TIM3_PCLK_EN()                  (RCC->APB1ENR1 |= (1UL << 1U))
#define TIM4_PCLK_EN()                  (RCC->APB1ENR1 |= (1UL << 2U))
#define TIM5_PCLK_EN()                  (RCC->APB1ENR1 |= (1UL << 3U))
#define TIM6_PCLK_EN()                  (RCC->APB1ENR1 |= (1UL << 4U))
#define TIM7_PCLK_EN()                  (RCC->APB1ENR1 |= (1UL << 5U))
#define TIM12_PCLK_EN()                 (RCC->APB1ENR1 |= (1UL << 6U))
#define TIM13_PCLK_EN()                 (RCC->APB1ENR1 |= (1UL << 7U))
#define TIM14_PCLK_EN()                 (RCC->APB1ENR1 |= (1UL << 8U))
#define TIM15_PCLK_EN()                 (RCC->APB2ENR |= (1UL << 16U))
#define TIM16_PCLK_EN()                 (RCC->APB2ENR |= (1UL << 17U))
#define TIM17_PCLK_EN()                 (RCC->APB2ENR |= (1UL << 18U))
#define TIM9_PCLK_EN()                  (RCC->APB2ENR |= (1UL << 19U))

#define TIM1_PCLK_DI()                  (RCC->APB2ENR &= ~(1UL << 0U))
#define TIM2_PCLK_DI()                  (RCC->APB1ENR1 &= ~(1UL << 0U))
#define TIM3_PCLK_DI()                  (RCC->APB1ENR1 &= ~(1UL << 1U))
#define TIM4_PCLK_DI()                  (RCC->APB1ENR1 &= ~(1UL << 2U))
#define TIM5_PCLK_DI()                  (RCC->APB1ENR1 &= ~(1UL << 3U))
#define TIM6_PCLK_DI()                  (RCC->APB1ENR1 &= ~(1UL << 4U))
#define TIM7_PCLK_DI()                  (RCC->APB1ENR1 &= ~(1UL << 5U))
#define TIM12_PCLK_DI()                 (RCC->APB1ENR1 &= ~(1UL << 6U))
#define TIM13_PCLK_DI()                 (RCC->APB1ENR1 &= ~(1UL << 7U))
#define TIM14_PCLK_DI()                 (RCC->APB1ENR1 &= ~(1UL << 8U))
#define TIM15_PCLK_DI()                 (RCC->APB2ENR &= ~(1UL << 16U))
#define TIM16_PCLK_DI()                 (RCC->APB2ENR &= ~(1UL << 17U))
#define TIM17_PCLK_DI()                 (RCC->APB2ENR &= ~(1UL << 18U))
#define TIM9_PCLK_DI()                  (RCC->APB2ENR &= ~(1UL << 19U))

/* -------------------------------------------------------------------------- */
/* LPTIMx clock enable macros                                                 */
/* -------------------------------------------------------------------------- */
#define LPTIM1_PCLK_EN()                (RCC->APB1ENR1 |= (1UL << 9U))
#define LPTIM2_PCLK_EN()                (RCC->APB4ENR |= (1UL << 9U))
#define LPTIM3_PCLK_EN()                (RCC->APB4ENR |= (1UL << 10U))
#define LPTIM4_PCLK_EN()                (RCC->APB4ENR |= (1UL << 11U))
#define LPTIM5_PCLK_EN()                (RCC->APB4ENR |= (1UL << 12U))

#define LPTIM1_PCLK_DI()                (RCC->APB1ENR1 &= ~(1UL << 9U))
#define LPTIM2_PCLK_DI()                (RCC->APB4ENR &= ~(1UL << 9U))
#define LPTIM3_PCLK_DI()                (RCC->APB4ENR &= ~(1UL << 10U))
#define LPTIM4_PCLK_DI()                (RCC->APB4ENR &= ~(1UL << 11U))
#define LPTIM5_PCLK_DI()                (RCC->APB4ENR &= ~(1UL << 12U))

/* -------------------------------------------------------------------------- */
/* I2Cx I3Cx clock enable macros                                              */
/* -------------------------------------------------------------------------- */
#define I2C1_PCLK_EN()                  (RCC->APB1ENR1 |= (1UL << 21U))
#define I3C1_PCLK_EN()                  (RCC->APB1ENR1 |= (1UL << 21U))
#define I2C2_PCLK_EN()                  (RCC->APB1ENR1 |= (1UL << 22U))
#define I2C3_PCLK_EN()                  (RCC->APB1ENR1 |= (1UL << 23U))

#define I2C1_PCLK_DI()                  (RCC->APB1ENR1 &= ~(1UL << 21U))
#define I3C1_PCLK_DI()                  (RCC->APB1ENR1 &= ~(1UL << 21U))
#define I2C2_PCLK_DI()                  (RCC->APB1ENR1 &= ~(1UL << 22U))
#define I2C3_PCLK_DI()                  (RCC->APB1ENR1 &= ~(1UL << 23U))

/* -------------------------------------------------------------------------- */
/* SPIx clock enable macros                                                   */
/* -------------------------------------------------------------------------- */
#define SPI1_PCLK_EN()                  (RCC->APB2ENR |= (1UL << 12U))
#define SPI2_PCLK_EN()                  (RCC->APB1ENR1 |= (1UL << 14U))
#define SPI3_PCLK_EN()                  (RCC->APB1ENR1 |= (1UL << 15U))
#define SPI4_PCLK_EN()                  (RCC->APB2ENR |= (1UL << 13U))
#define SPI5_PCLK_EN()                  (RCC->APB2ENR |= (1UL << 20U))
#define SPI6_PCLK_EN()                  (RCC->APB4ENR |= (1UL << 5U))

#define SPI1_PCLK_DI()                  (RCC->APB2ENR &= ~(1UL << 12U))
#define SPI2_PCLK_DI()                  (RCC->APB1ENR1 &= ~(1UL << 14U))
#define SPI3_PCLK_DI()                  (RCC->APB1ENR1 &= ~(1UL << 15U))
#define SPI4_PCLK_DI()                  (RCC->APB2ENR &= ~(1UL << 13U))
#define SPI5_PCLK_DI()                  (RCC->APB2ENR &= ~(1UL << 20U))
#define SPI6_PCLK_DI()                  (RCC->APB4ENR &= ~(1UL << 5U))

/* -------------------------------------------------------------------------- */
/* USARTx UARTx clock enable macros                                           */
/* -------------------------------------------------------------------------- */
#define USART1_PCLK_EN()                (RCC->APB2ENR |= (1UL << 4U))
#define USART2_PCLK_EN()                (RCC->APB1ENR1 |= (1UL << 17U))
#define USART3_PCLK_EN()                (RCC->APB1ENR1 |= (1UL << 18U))
#define UART4_PCLK_EN()                 (RCC->APB1ENR1 |= (1UL << 19U))
#define UART5_PCLK_EN()                 (RCC->APB1ENR1 |= (1UL << 20U))
#define UART7_PCLK_EN()                 (RCC->APB1ENR1 |= (1UL << 30U))
#define UART8_PCLK_EN()                 (RCC->APB1ENR1 |= (1UL << 31U))
#define LPUART1_PCLK_EN()               (RCC->APB4ENR |= (1UL << 3U))

#define USART1_PCLK_DI()                (RCC->APB2ENR &= ~(1UL << 4U))
#define USART2_PCLK_DI()                (RCC->APB1ENR1 &= ~(1UL << 17U))
#define USART3_PCLK_DI()                (RCC->APB1ENR1 &= ~(1UL << 18U))
#define UART4_PCLK_DI()                 (RCC->APB1ENR1 &= ~(1UL << 19U))
#define UART5_PCLK_DI()                 (RCC->APB1ENR1 &= ~(1UL << 20U))
#define UART7_PCLK_DI()                 (RCC->APB1ENR1 &= ~(1UL << 30U))
#define UART8_PCLK_DI()                 (RCC->APB1ENR1 &= ~(1UL << 31U))
#define LPUART1_PCLK_DI()               (RCC->APB4ENR &= ~(1UL << 3U))

/* -------------------------------------------------------------------------- */
/* AHB1 PERIPHERAL clock enable macros                                        */
/* -------------------------------------------------------------------------- */
#define GPDMA1_PCLK_EN()                (RCC->AHB1ENR |= (1UL << 4U))
#define ADC12_PCLK_EN()                 (RCC->AHB1ENR |= (1UL << 5U))
#define ETH1MAC_PCLK_EN()               (RCC->AHB1ENR |= (1UL << 15U))
#define ETH1TX_PCLK_EN()                (RCC->AHB1ENR |= (1UL << 16U))
#define ETH1RX_PCLK_EN()                (RCC->AHB1ENR |= (1UL << 17U))
#define ETH1_PCLK_EN()                  (RCC->AHB1ENR |= ((1UL << 15U) | (1UL << 16U) | (1UL << 17U)))
#define OTG_HS_PCLK_EN()                (RCC->AHB1ENR |= (1UL << 25U))
#define USBPHYC_PCLK_EN()               (RCC->AHB1ENR |= (1UL << 26U))
#define OTG_FS_PCLK_EN()                (RCC->AHB1ENR |= (1UL << 27U))
#define ADF1_PCLK_EN()                  (RCC->AHB1ENR |= (1UL << 31U))

#define GPDMA1_PCLK_DI()                (RCC->AHB1ENR &= ~(1UL << 4U))
#define ADC12_PCLK_DI()                 (RCC->AHB1ENR &= ~(1UL << 5U))
#define ETH1MAC_PCLK_DI()               (RCC->AHB1ENR &= ~(1UL << 15U))
#define ETH1TX_PCLK_DI()                (RCC->AHB1ENR &= ~(1UL << 16U))
#define ETH1RX_PCLK_DI()                (RCC->AHB1ENR &= ~(1UL << 17U))
#define ETH1_PCLK_DI()                  (RCC->AHB1ENR &= ~((1UL << 15U) | (1UL << 16U) | (1UL << 17U)))
#define OTG_HS_PCLK_DI()                (RCC->AHB1ENR &= ~(1UL << 25U))
#define USBPHYC_PCLK_DI()               (RCC->AHB1ENR &= ~(1UL << 26U))
#define OTG_FS_PCLK_DI()                (RCC->AHB1ENR &= ~(1UL << 27U))
#define ADF1_PCLK_DI()                  (RCC->AHB1ENR &= ~(1UL << 31U))

/* -------------------------------------------------------------------------- */
/* AHB2 PERIPHERAL clock enable macros                                        */
/* -------------------------------------------------------------------------- */
#define PSSI_PCLK_EN()                  (RCC->AHB2ENR |= (1UL << 1U))
#define SDMMC2_PCLK_EN()                (RCC->AHB2ENR |= (1UL << 9U))
#define CORDIC_PCLK_EN()                (RCC->AHB2ENR |= (1UL << 14U))
#define SRAM1_PCLK_EN()                 (RCC->AHB2ENR |= (1UL << 29U))
#define SRAM2_PCLK_EN()                 (RCC->AHB2ENR |= (1UL << 30U))

#define PSSI_PCLK_DI()                  (RCC->AHB2ENR &= ~(1UL << 1U))
#define SDMMC2_PCLK_DI()                (RCC->AHB2ENR &= ~(1UL << 9U))
#define CORDIC_PCLK_DI()                (RCC->AHB2ENR &= ~(1UL << 14U))
#define SRAM1_PCLK_DI()                 (RCC->AHB2ENR &= ~(1UL << 29U))
#define SRAM2_PCLK_DI()                 (RCC->AHB2ENR &= ~(1UL << 30U))

/* -------------------------------------------------------------------------- */
/* AHB3 PERIPHERAL clock enable macros                                        */
/* -------------------------------------------------------------------------- */
#define RNG_PCLK_EN()                   (RCC->AHB3ENR |= (1UL << 0U))
#define HASH_PCLK_EN()                  (RCC->AHB3ENR |= (1UL << 1U))
#define CRYP_PCLK_EN()                  (RCC->AHB3ENR |= (1UL << 2U))
#define SAES_PCLK_EN()                  (RCC->AHB3ENR |= (1UL << 4U))
#define PKA_PCLK_EN()                   (RCC->AHB3ENR |= (1UL << 6U))

#define RNG_PCLK_DI()                   (RCC->AHB3ENR &= ~(1UL << 0U))
#define HASH_PCLK_DI()                  (RCC->AHB3ENR &= ~(1UL << 1U))
#define CRYP_PCLK_DI()                  (RCC->AHB3ENR &= ~(1UL << 2U))
#define SAES_PCLK_DI()                  (RCC->AHB3ENR &= ~(1UL << 4U))
#define PKA_PCLK_DI()                   (RCC->AHB3ENR &= ~(1UL << 6U))

/* -------------------------------------------------------------------------- */
/* AHB4 PERIPHERAL clock enable macros                                        */
/* -------------------------------------------------------------------------- */
#define CRC_PCLK_EN()                   (RCC->AHB4ENR |= (1UL << 19U))
#define BKPRAM_PCLK_EN()                (RCC->AHB4ENR |= (1UL << 28U))

#define CRC_PCLK_DI()                   (RCC->AHB4ENR &= ~(1UL << 19U))
#define BKPRAM_PCLK_DI()                (RCC->AHB4ENR &= ~(1UL << 28U))

/* -------------------------------------------------------------------------- */
/* AHB5 PERIPHERAL clock enable macros                                        */
/* -------------------------------------------------------------------------- */
#define HPDMA1_PCLK_EN()                (RCC->AHB5ENR |= (1UL << 0U))
#define DMA2D_PCLK_EN()                 (RCC->AHB5ENR |= (1UL << 1U))
#define JPEG_PCLK_EN()                  (RCC->AHB5ENR |= (1UL << 3U))
#define FMC_PCLK_EN()                   (RCC->AHB5ENR |= (1UL << 4U))
#define XSPI1_PCLK_EN()                 (RCC->AHB5ENR |= (1UL << 5U))
#define MCE1_PCLK_EN()                  (RCC->AHB5ENR |= (1UL << 5U))
#define SDMMC1_PCLK_EN()                (RCC->AHB5ENR |= (1UL << 8U))
#define XSPI2_PCLK_EN()                 (RCC->AHB5ENR |= (1UL << 12U))
#define MCE2_PCLK_EN()                  (RCC->AHB5ENR |= (1UL << 12U))
#define XSPIM1_PCLK_EN()                (RCC->AHB5ENR |= (1UL << 14U))
#define GFXMMU_PCLK_EN()                (RCC->AHB5ENR |= (1UL << 19U))
#define GPU2D_PCLK_EN()                 (RCC->AHB5ENR |= (1UL << 20U))
#define MCE3_PCLK_EN()                  (RCC->AHB5ENR |= (1UL << 4U))

#define HPDMA1_PCLK_DI()                (RCC->AHB5ENR &= ~(1UL << 0U))
#define DMA2D_PCLK_DI()                 (RCC->AHB5ENR &= ~(1UL << 1U))
#define JPEG_PCLK_DI()                  (RCC->AHB5ENR &= ~(1UL << 3U))
#define FMC_PCLK_DI()                   (RCC->AHB5ENR &= ~(1UL << 4U))
#define XSPI1_PCLK_DI()                 (RCC->AHB5ENR &= ~(1UL << 5U))
#define MCE1_PCLK_DI()                  (RCC->AHB5ENR &= ~(1UL << 5U))
#define SDMMC1_PCLK_DI()                (RCC->AHB5ENR &= ~(1UL << 8U))
#define XSPI2_PCLK_DI()                 (RCC->AHB5ENR &= ~(1UL << 12U))
#define MCE2_PCLK_DI()                  (RCC->AHB5ENR &= ~(1UL << 12U))
#define XSPIM1_PCLK_DI()                (RCC->AHB5ENR &= ~(1UL << 14U))
#define GFXMMU_PCLK_DI()                (RCC->AHB5ENR &= ~(1UL << 19U))
#define GPU2D_PCLK_DI()                 (RCC->AHB5ENR &= ~(1UL << 20U))
#define MCE3_PCLK_DI()                  (RCC->AHB5ENR &= ~(1UL << 4U))

/* -------------------------------------------------------------------------- */
/* APB1 PERIPHERAL clock enable macros                                        */
/* -------------------------------------------------------------------------- */
#define WWDG_PCLK_EN()                  (RCC->APB1ENR1 |= (1UL << 11U))
#define SPDIFRX1_PCLK_EN()              (RCC->APB1ENR1 |= (1UL << 16U))
#define CEC_PCLK_EN()                   (RCC->APB1ENR1 |= (1UL << 27U))
#define CRS_PCLK_EN()                   (RCC->APB1ENR2 |= (1UL << 1U))
#define MDIOS_PCLK_EN()                 (RCC->APB1ENR2 |= (1UL << 5U))
#define FDCAN_PCLK_EN()                 (RCC->APB1ENR2 |= (1UL << 8U))
#define FDCAN1_PCLK_EN()                FDCAN_PCLK_EN()
#define FDCAN2_PCLK_EN()                FDCAN_PCLK_EN()
#define UCPD1_PCLK_EN()                 (RCC->APB1ENR2 |= (1UL << 27U))

#define WWDG_PCLK_DI()                  (RCC->APB1ENR1 &= ~(1UL << 11U))
#define SPDIFRX1_PCLK_DI()              (RCC->APB1ENR1 &= ~(1UL << 16U))
#define CEC_PCLK_DI()                   (RCC->APB1ENR1 &= ~(1UL << 27U))
#define CRS_PCLK_DI()                   (RCC->APB1ENR2 &= ~(1UL << 1U))
#define MDIOS_PCLK_DI()                 (RCC->APB1ENR2 &= ~(1UL << 5U))
#define FDCAN_PCLK_DI()                 (RCC->APB1ENR2 &= ~(1UL << 8U))
#define FDCAN1_PCLK_DI()                FDCAN_PCLK_DI()
#define FDCAN2_PCLK_DI()                FDCAN_PCLK_DI()
#define UCPD1_PCLK_DI()                 (RCC->APB1ENR2 &= ~(1UL << 27U))

/* -------------------------------------------------------------------------- */
/* APB2 PERIPHERAL clock enable macros                                        */
/* -------------------------------------------------------------------------- */
#define SAI1_PCLK_EN()                  (RCC->APB2ENR |= (1UL << 22U))
#define SAI2_PCLK_EN()                  (RCC->APB2ENR |= (1UL << 23U))

#define SAI1_PCLK_DI()                  (RCC->APB2ENR &= ~(1UL << 22U))
#define SAI2_PCLK_DI()                  (RCC->APB2ENR &= ~(1UL << 23U))

/* -------------------------------------------------------------------------- */
/* APB4 PERIPHERAL clock enable macros                                        */
/* -------------------------------------------------------------------------- */
#define SBS_PCLK_EN()                   (RCC->APB4ENR |= (1UL << 1U))
#define LPUART1_PCLK_EN()               (RCC->APB4ENR |= (1UL << 3U))
#define VREFBUF_PCLK_EN()               (RCC->APB4ENR |= (1UL << 15U))
#define RTCAPB_PCLK_EN()                (RCC->APB4ENR |= (1UL << 16U))
#define DTS_PCLK_EN()                   (RCC->APB4ENR |= (1UL << 26U))

#define SBS_PCLK_DI()                   (RCC->APB4ENR &= ~(1UL << 1U))
#define LPUART1_PCLK_DI()               (RCC->APB4ENR &= ~(1UL << 3U))
#define VREFBUF_PCLK_DI()               (RCC->APB4ENR &= ~(1UL << 15U))
#define RTCAPB_PCLK_DI()                (RCC->APB4ENR &= ~(1UL << 16U))
#define DTS_PCLK_DI()                   (RCC->APB4ENR &= ~(1UL << 26U))

/* -------------------------------------------------------------------------- */
/* APB5 PERIPHERAL clock enable macros                                        */
/* -------------------------------------------------------------------------- */
#define LTDC_PCLK_EN()                  (RCC->APB5ENR |= (1UL << 1U))
#define DCMIPP_PCLK_EN()                (RCC->APB5ENR |= (1UL << 2U))
#define GFXTIM_PCLK_EN()                (RCC->APB5ENR |= (1UL << 4U))

#define LTDC_PCLK_DI()                  (RCC->APB5ENR &= ~(1UL << 1U))
#define DCMIPP_PCLK_DI()                (RCC->APB5ENR &= ~(1UL << 2U))
#define GFXTIM_PCLK_DI()                (RCC->APB5ENR &= ~(1UL << 4U))

/* -------------------------------------------------------------------------- */
/* Cortex-M7 FPU definitions                                                  */
/* -------------------------------------------------------------------------- */
#define SCB_CPACR_ADDR                 (0xE000ED88UL)
#define SCB_CPACR                      (*(__vo uint32_t *)SCB_CPACR_ADDR)

#define SCB_DCCMVAC_ADDR                (0xE000EF68UL)
#define SCB_DCCMVAC                     (*((__vo uint32_t *)SCB_DCCMVAC_ADDR))

/*
 * FPU system registers
 */
#define FPU_BASEADDR                   (0xE000EF34UL)

typedef struct{
	__vo uint32_t FPCCR;      /* Floating-point context control register */
	__vo uint32_t FPCAR;      /* Floating-point context address register */
	__vo uint32_t FPDSCR;     /* Floating-point default status control register */
} FPU_RegDef_t;

#define FPU                            ((FPU_RegDef_t *)FPU_BASEADDR)

/*
 * Enable CP10 and CP11 full access.
 */
#define FPU_ENABLE()                   do {                           \
	                                      SCB_CPACR |= (0xFU << 20U); \
	                                      __asm volatile ("dsb");     \
	                                      __asm volatile ("isb");     \
                                      } while(0)

/* -------------------------------------------------------------------------- */
/* Cortex-M7 MPU definitions                                                  */
/* -------------------------------------------------------------------------- */
#define MPU_BASEADDR					(0xE000ED90UL)

typedef struct{
	__vo uint32_t TYPE;					//0x00 MPU Type Register
	__vo uint32_t CTRL;					//0x04 MPU Control Register
	__vo uint32_t RNR;					//0x08 MPU regon number register
	__vo uint32_t RBAR;					//0x0C MPU region base address register
	__vo uint32_t RASR;					//0x10 MPU region attribute and size register
}MPU_RegDef_t;

#define MPU								((MPU_RegDef_t*)MPU_BASEADDR)

/*
 * MPU_CTRL Bits
 */
#define MPU_CTRL_ENABLE 				0U		//Enables the MPU
#define MPU_CTRL_HFNMIENA				1U		//Enables the operation of MPU during hard fault, NMI, and FAULTMASK handlers
#define MPU_CTRL_PRIVDEFENA				2U		//Enables privileged software access to the default memory map

/*
 * MPU_RASR Bits
 */
#define MPU_RASR_ENABLE					0U		//Region enable bit.
#define MPU_RASR_SIZE					1U		//Specifies the size of the MPU protection region. The minimum permitted value is 4 (0b00100) for 32 Bytes, and the maximum is 31 (0b11111) for 4Gbytes.
#define MPU_RASR_SRD					8U		//Subregion disable bits
#define MPU_RASR_B						16U		//Memory access attributes
#define MPU_RASR_C						17U		//
#define MPU_RASR_S						18U
#define MPU_RASR_TEX					19U
#define MPU_RASR_AP						24U
#define MPU_RASR_XN						28U


/* -------------------------------------------------------------------------- */
/* OTHER MACROS                                                               */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* SBS bit position macros                                                    */
/* -------------------------------------------------------------------------- */
/*
 * Bit position definitions for SBS_PMCR
 */
#define SBS_PMCR_ETH_PHYSEL			21U		// Ethernet PHY interface selection, 3 bits: [23:21]

/*
 * SBS_PMCR ETH_PHYSEL values
 */
#define SBS_PMCR_ETH_PHYSEL_MII		0U		// 000: GMII or MII
#define SBS_PMCR_ETH_PHYSEL_RMII	4U		// 100: RMII

/* -------------------------------------------------------------------------- */
/* Ethernet bit position macros                                               */
/* -------------------------------------------------------------------------- */
/*
 * ETH_DMAMR bits
 */
#define ETH_DMAMR_SWR					0U		//Software reset

/*
 * ETH_MACMDIOAR bits
 *
 * MACMDIOAR:
 * - MB			: MDIO busy
 * - C45E   	: Clause 45 enable (if not enable Clause 22)
 * - GOC[1:0]	: MII Operation Command
 * - SKAP		: Skip address packet
 * - CR[3:0]	: CSR/MDC Clock Range
 * - NTC[2:0]	: Number of trailing clocks
 * - RDA[4:0]	: Register/device address
 * - PA[4:0]	: PHY address
 * - BTB		: Back-to-back transactions
 * - PSE		: Preamble suppression enable
 */
#define ETH_MACMDIOAR_MB				0U
#define ETH_MACMDIOAR_C45E				1U
#define ETH_MACMDIOAR_GOC				2U
#define ETH_MACDMIOAR_SKAP				4U
#define ETH_MACMDIOAR_CR				8U
#define ETH_MACMDIOAR_NTC				12U
#define ETH_MACMDIOAR_RDA				16U
#define ETH_MACMDIOAR_PA				21U
#define ETH_MACMDIOAR_BTB				26U
#define ETH_MACMDIOAR_PSE				27U

/*
 * ETH_MACMDIODR bits
 */
#define ETH_MACMDIODR_MD				0U		//MDIO Data, 16 bit

/* -------------------------------------------------------------------------- */
/* ETH DMA Descriptor Macros                                                  */
/* -------------------------------------------------------------------------- */
/*
 * TX descriptor read-format TDES2 bits
 */
#define ETH_TDES2_IOC                   (1UL << 31)
#define ETH_TDES2_B1L_MASK              (0x3FFFUL)

/*
 * Ethernet TX Descriptor TDES3 bit selection macros
 */
#define ETH_TDES3_FL_MASK               0x00007FFFUL
#define ETH_TDES3_CIC                   16U
#define ETH_TDES3_CIC_MASK              (0x3UL << ETH_TDES3_CIC)
#define ETH_TDES3_LD                    (1UL << 28)
#define ETH_TDES3_FD                    (1UL << 29)
#define ETH_TDES3_CTXT                  (1UL << 30)
#define ETH_TDES3_OWN                   (1UL << 31)

/*
 * RX descriptor read-format RDES3 bits.
 *
 * These are written by software before giving the descriptor to DMA.
 */
#define ETH_RDES3_OWN					(1UL << 31)		//When this bit is set, it indicates that the DMA owns the descriptor.
#define ETH_RDES3_IOC					(1UL << 30)		//When this bit is set, an interrupt is issued to the application when the DMA closes this descriptor.
#define ETH_RDES3_BUF2V					(1UL << 25)		//When this bit is set, it indicates to the DMA that the buffer 2 address specified in RDES2 is valid.
#define ETH_RDES3_BUF1V					(1UL << 24)		//When set, this indicates to the DMA that the buffer 1 address specified in RDES0 is valid.

/*
 * RX Descriptor write-back RDES3 bits
 *
 * These are written by DMA after packet reception
 */
#define ETH_RDES3_WB_OWN				(1UL << 31)		//The DMA owns the descriptor
#define ETH_RDES3_WB_CTXT				(1UL << 30)		//When this bit is set, it indicates that the current descriptor is a context type descriptor. The DMA writes 0 to this bit for normal receive descriptor.
#define ETH_RDES3_WB_FD					(1UL << 29)		//When this bit is set, it indicates that this descriptor contains the first buffer of the packet.
#define ETH_RDES3_WB_LD					(1UL << 28)		//When this bit is set, it indicates that the buffers to which this descriptor is pointing are the last buffers of the packet.
#define ETH_RDES3_WB_ES					(1UL << 15)
#define ETH_RDES3_WB_PL_MASK			(0x7FFFUL)		//Bits between [0:14]

/*
 * Notes on: ETH_RDES3_WB_ES:
 *
 * When this bit is set, it indicates the logical OR of the following bits:
 * RDES3[19]: Dribble Error
 * RDES3[20]: Receive Error
 * RDES3[21]: Overflow Error
 * RDES3[22]: Watchdog timeout
 * RDES3[23]: Giant Packet
 * RDES3[24]: CRC Error
 * This field is valid only when the LD bit of RDES3 is set
 */

/* -------------------------------------------------------------------------- */
/* ETH MAC bit position macros                                                */
/* -------------------------------------------------------------------------- */
/*
 * @ETH_MACCR_MODES
 * Bit position definitions for ETH_MACCR: Operating Mode Configuration Register
 */
#define ETH_MACCR_RE					0U		// Receiver enable
#define ETH_MACCR_TE					1U		// Transmitter enable
#define ETH_MACCR_PRELEN				2U		// Preamble length for transmit packets
#define ETH_MACCR_DC					4U		// Deferral check
#define ETH_MACCR_BL					5U		// Back-off limit
#define ETH_MACCR_DR					8U		// Disable retry
#define ETH_MACCR_DCRS					9U		// Disable Carrier Sense During Transmission
#define ETH_MACCR_DO					10U		// Disable Receive Own
#define ETH_MACCR_ECRSFD				11U		// Enable Carrier Sense Before Transmission in Full-duplex mode
#define ETH_MACCR_LM					12U		// Loopback Mode
#define ETH_MACCR_DM					13U		// Duplex mode: (When Set full-duplex)
#define ETH_MACCR_FES					14U		// Fast Ethernet speed (0: 10 MBit/s, 1: 100 MBit/s
#define ETH_MACCR_JE					16U		// Jumbo Packet Enable (When set, MAC allows jumbo packets of 9,018 bytes without reporting a giant packet error
#define ETH_MACCR_JD					17U		// When this bit is set, the MAC disables the jabber timer on the transmitter.
#define ETH_MACCR_WD					19U		// When this bit is set, the MAC disables the watchdog timer on the transmitter.
#define ETH_MACCR_ACS					20U		// Automatic Pad or CRC Stripping
#define ETH_MACCR_CST					21U		// CRC Stripping for Type Packets
#define ETH_MACCR_S2KP					22U		// IEEE 802.3as Support for 2K Packets
#define ETH_MACCR_GPSLCE				23U		// Giant Packet Size Limit Control Enable
#define ETH_MACCR_IPG					24U		// Inter-Packet Gap
#define ETH_MACCR_IPC					27U		// Checksum Offload
#define ETH_MACCR_SARC					28U		// Source Address Insertation or Replacement Control
#define ETH_MACCR_ARPEN					31U		// ARP Offload Enable

/*
 * Bit position masks for ETH_MACCR: Operating Mode Configuration Register
 */
#define ETH_MACCR_RE_MSK				(1U << ETH_MACCR_RE)		//
#define ETH_MACCR_TE_MSK				(1U << ETH_MACCR_TE)		//
#define ETH_MACCR_PRELEN_MSK			(3U << ETH_MACCR_PRELEN)	//
#define ETH_MACCR_DC_MSK				(1U << ETH_MACCR_DC)		//
#define ETH_MACCR_BL_MSK				(3U << ETH_MACCR_BL)		//
#define ETH_MACCR_DR_MSK				(1U << ETH_MACCR_DR)		//
#define ETH_MACCR_DCRS_MSK				(1U << ETH_MACCR_DCRS)		// Disable Carrier Sense During Transmission
#define ETH_MACCR_DO_MSK				(1U << ETH_MACCR_DO)		// Disable Receive Own
#define ETH_MACCR_ECRSFD_MSK			(1U << ETH_MACCR_ECRSFD)	// Enable Carrier Sense Before Transmission in Full-duplex mode
#define ETH_MACCR_LM_MSK				(1U << ETH_MACCR_LM)		// Loopback Mode
#define ETH_MACCR_DM_MSK				(1U << ETH_MACCR_DM)		// Duplex mode: (When Set full-duplex)
#define ETH_MACCR_FES_MSK				(1U << ETH_MACCR_FES)		// Fast Ethernet speed (0: 10 MBit/s, 1: 100 MBit/s
#define ETH_MACCR_JE_MSK				(1U << ETH_MACCR_JE)		// Jumbo Packet Enable (When set, MAC allows jumbo packets of 9,018 bytes without reporting a giant packet error
#define ETH_MACCR_JD_MSK				(1U << ETH_MACCR_JD)		// When this bit is set, the MAC disables the jabber timer on the transmitter.
#define ETH_MACCR_WD_MSK				(1U << ETH_MACCR_WD)		// When this bit is set, the MAC disables the watchdog timer on the transmitter.
#define ETH_MACCR_ACS_MSK				(1U << ETH_MACCR_ACS)		// Automatic Pad or CRC Stripping
#define ETH_MACCR_CST_MSK				(1U << ETH_MACCR_CST)		// CRC Stripping for Type Packets
#define ETH_MACCR_S2KP_MSK				(1U << ETH_MACCR_S2KP)		// IEEE 802.3as Support for 2K Packets
#define ETH_MACCR_GPSLCE_MSK			(1U << ETH_MACCR_GPSLCE)	// Giant Packet Size Limit Control Enable
#define ETH_MACCR_IPG_MSK				(7U << ETH_MACCR_IPG)		// Inter-Packet Gap
#define ETH_MACCR_IPC_MSK				(1U << ETH_MACCR_IPC)		// Checksum Offload
#define ETH_MACCR_SARC_MSK				(7U << ETH_MACCR_SARC)		// Source Address Insertation or Replacement Control
#define ETH_MACCR_ARPEN_MSK				(1U << ETH_MACCR_ARPEN)		// ARP Offload Enable

/*
 * @ETH_MACPFR_FILTERS
 * Bit position definitions for ETH_MACPFR: Packet Filtering Control Register
 */
#define ETH_MACPFR_PR					0U		// Promiscuous mode
#define ETH_MACPFR_HUC					1U		// Hash Unicast
#define ETH_MACPFR_HMC					2U		// Hash Multicast
#define ETH_MACPFR_DAIF					3U		// DA Inverse Filtering
#define ETH_MACPFR_PM					4U		// Pass All Multicast
#define ETH_MACPFR_DBF					5U		// Disable Broadcast Packets
#define ETH_MACPFR_PCF					6U		// Pass Control Packets
#define ETH_MACPFR_SAIF					8U		// SA Inverse Filtering
#define ETH_MACPFR_SAF					9U		// Source Address Filter Enable
#define ETH_MACPFR_HPF					10U		// Hash or Perfect Filter
#define ETH_MACPFR_VTFE					16U		// VLAN Tag Filter Enable
#define ETH_MACPFR_IPFE					20U		// Layer 3 and Layer 4 Filter Enable
#define ETH_MACPFR_DNTU					21U		// Drop Non-TCP/UDP over IP Packets
#define ETH_MACPFR_RA					31U		// Receive All

#define ETH_MACPFR_PR_MSK				(1UL << ETH_MACPFR_PR)
#define ETH_MACPFR_HUC_MSK				(1UL << ETH_MACPFR_HUC)
#define ETH_MACPFR_HMC_MSK				(1UL << ETH_MACPFR_HMC)
#define ETH_MACPFR_DAIF_MSK				(1UL << ETH_MACPFR_DAIF)
#define ETH_MACPFR_PM_MSK				(1UL << ETH_MACPFR_PM)
#define ETH_MACPFR_DBF_MSK				(1UL << ETH_MACPFR_DBF)
#define ETH_MACPFR_PCF_MSK				(3UL << ETH_MACPFR_PCF)
#define ETH_MACPFR_SAIF_MSK				(1UL << ETH_MACPFR_SAIF)
#define ETH_MACPFR_SAF_MSK				(1UL << ETH_MACPFR_SAF)
#define ETH_MACPFR_HPF_MSK				(1UL << ETH_MACPFR_HPF)
#define ETH_MACPFR_VTFE_MSK				(1UL << ETH_MACPFR_VTFE)
#define ETH_MACPFR_IPFE_MSK				(1UL << ETH_MACPFR_IPFE)
#define ETH_MACPFR_DNTU_MSK				(1UL << ETH_MACPFR_DNTU)
#define ETH_MACPFR_RA_MSK				(1UL << ETH_MACPFR_RA)


/* -------------------------------------------------------------------------- */
/* ETH MTL bit position macros                                                */
/* -------------------------------------------------------------------------- */
/*
 * @ETH_MTLRXQOMR_CONFIGS
 * Bit position definitions for ETH_MTLRXQOMR: Rx queue operating mode register
 */
#define ETH_MTLRXQOMR_RTC				0U		// Receive threshold control, bits [1:0]
#define ETH_MTLRXQOMR_FUP				3U		// Forward undersized good packets
#define ETH_MTLRXQOMR_FEP				4U		// Forward error packets
#define ETH_MTLRXQOMR_RSF				5U		// Receive store and forward
#define ETH_MTLRXQOMR_DIS_TCP_EF		6U		// Disable dropping TCP/IP checksum error packets
#define ETH_MTLRXQOMR_EHFC				7U		// Enable hardware flow control

#define ETH_MTLRXQOMR_RTC_MSK			(3UL << ETH_MTLRXQOMR_RTC)
#define ETH_MTLRXQOMR_FUP_MSK			(1UL << ETH_MTLRXQOMR_FUP)
#define ETH_MTLRXQOMR_FEP_MSK			(1UL << ETH_MTLRXQOMR_FEP)
#define ETH_MTLRXQOMR_RSF_MSK			(1UL << ETH_MTLRXQOMR_RSF)
#define ETH_MTLRXQOMR_DIS_TCP_EF_MSK	(1UL << ETH_MTLRXQOMR_DIS_TCP_EF)
#define ETH_MTLRXQOMR_EHFC_MSK			(1UL << ETH_MTLRXQOMR_EHFC)

/*
 * @ETH_MTLTXQOMR_CONFIGS
 * Bit position definitions for ETH_MTLTXQOMR: Tx queue operating mode register
 */
#define ETH_MTLTXQOMR_FTQ				0U		// Flust trnsmit queue
#define ETH_MTLTXQOMR_TSF				1U		// Transmit store and forward
#define ETH_MTLTXQOMR_TXQEN				3U		// Transmit queue enable
#define ETH_MTLTXQOMR_TTC				4U		// Transmit threshold control
#define ETH_MTLTXQOMR_TQS				16U		// Transmit queue size

#define ETH_MTLTXQOMR_FTQ_MSK			(1UL << ETH_MTLTXQOMR_FTQ)
#define ETH_MTLTXQOMR_TSF_MSK			(1UL << ETH_MTLTXQOMR_TSF)
#define ETH_MTLTXQOMR_TXQEN_MSK			(3UL << ETH_MTLTXQOMR_TXQEN)	// Use TXQEN Field Values Macros
#define ETH_MTLTXQOMR_TTC_MSK			(7UL << ETH_MTLTXQOMR_TTC)		// Threshold level from 32 = 0b000 to 512 = 0b111 <!These bits are only used when TSF is reset>
#define ETH_MTLTXQOMR_TQS_MSK			(7UL << ETH_MTLTXQOMR_TQS)		// Queue size range from 256 bytes TQS = 0b000 to 2024 bytes TQS = 0b111

/*
 * TXQEN field values
 */
#define ETH_MTLTXQOMR_TXQEN_DISABLE		(0UL << ETH_MTLTXQOMR_TXQEN)
#define ETH_MTLTXQOMR_TXQEN_ENABLE		(2UL << ETH_MTLTXQOMR_TXQEN)

/* -------------------------------------------------------------------------- */
/* ETH DMA Channel bit position macros                                        */
/* -------------------------------------------------------------------------- */
/*
 * Bit Position definitions for ETH_DMACTXCR: Channel transmit control register
 */
#define ETH_DMACTXCR_ST					0U		// Start or stop receive

/*
 * Bit position definitions/masks for ETH_DMACRXCR: Channel receive control register
 */
#define ETH_DMACRXCR_SR					0U		// Start or stop receive
#define ETH_DMACRXCR_RBSZ				1U		// Receive buffer size, bits [14:1]
#define ETH_DMACRXCR_RBSZ_MASK			(0x3FFFUL << ETH_DMACRXCR_RBSZ)


/* -------------------------------------------------------------------------- */
/* I2C bit position macros                                                    */
/* -------------------------------------------------------------------------- */
/*
 * Bit position definitions for I2C_CR1
 */
#define I2C_CR1_PE                  	0U      // Peripheral enable
#define I2C_CR1_TXIE                	1U      // TX interrupt enable
#define I2C_CR1_RXIE               		2U      // RX interrupt enable
#define I2C_CR1_ADDRIE              	3U      // Address match interrupt enable
#define I2C_CR1_NACKIE              	4U      // Not acknowledge interrupt enable
#define I2C_CR1_STOPIE              	5U      // STOP detection interrupt enable
#define I2C_CR1_TCIE                	6U      // Transfer complete interrupt enable
#define I2C_CR1_ERRIE               	7U      // Error interrupt enable

#define I2C_CR1_DNF                 	8U      // Digital noise filter, 4 bits: [11:8]
#define I2C_CR1_ANFOFF              	12U     // Analog noise filter OFF
#define I2C_CR1_TXDMAEN             	14U     // TX DMA enable
#define I2C_CR1_RXDMAEN             	15U     // RX DMA enable

#define I2C_CR1_SBC                 	16U     // Slave byte control
#define I2C_CR1_NOSTRETCH           	17U     // Clock stretching disable
#define I2C_CR1_WUPEN               	18U     // Wakeup from Stop mode enable
#define I2C_CR1_GCEN                	19U     // General call enable
#define I2C_CR1_SMBHEN              	20U     // SMBus host address enable
#define I2C_CR1_SMBDEN             		21U     // SMBus device default address enable
#define I2C_CR1_ALERTEN             	22U     // SMBus alert enable
#define I2C_CR1_PECEN               	23U     // Packet error checking enable
#define I2C_CR1_FMP                 	24U     // Fast-mode Plus drive enable

#define I2C_CR1_ADDRACLR            	30U     // ADDR flag automatic clear
#define I2C_CR1_STOPFACLR           	31U     // STOPF flag automatic clear

/*
 * Bit position definitions for I2C_CR2
 */
#define I2C_CR2_SADD                	0U      // Slave address, 10 bits: [9:0]
#define I2C_CR2_RD_WRN              	10U     // Transfer direction
#define I2C_CR2_ADD10               	11U     // 10-bit addressing mode
#define I2C_CR2_HEAD10R             	12U     // 10-bit address header only read direction
#define I2C_CR2_START               	13U     // START generation
#define I2C_CR2_STOP                	14U     // STOP generation
#define I2C_CR2_NACK                	15U     // NACK generation

#define I2C_CR2_NBYTES             		16U     // Number of bytes, 8 bits: [23:16]
#define I2C_CR2_RELOAD              	24U     // NBYTES reload mode
#define I2C_CR2_AUTOEND             	25U     // Automatic end mode
#define I2C_CR2_PECBYTE            		26U     // Packet error checking byte

/*
 * Bit position definitions for I2C_OAR1
 */
#define I2C_OAR1_OA1                	0U      // Own address 1, bits depend on 7/10-bit mode
#define I2C_OAR1_OA1MODE            	10U     // Own address 1 mode
#define I2C_OAR1_OA1EN              	15U     // Own address 1 enable

/*
 * Bit position definitions for I2C_OAR2
 */
#define I2C_OAR2_OA2                	1U      // Own address 2, 7 bits: [7:1]
#define I2C_OAR2_OA2MSK             	8U      // Own address 2 mask, 3 bits: [10:8]
#define I2C_OAR2_OA2EN              	15U     // Own address 2 enable

/*
 * Bit position definitions for I2C_TIMINGR
 */
#define I2C_TIMINGR_SCLL            	0U      // SCL low period, 8 bits: [7:0]
#define I2C_TIMINGR_SCLH            	8U      // SCL high period, 8 bits: [15:8]
#define I2C_TIMINGR_SDADEL          	16U     // SDA delay, 4 bits: [19:16]
#define I2C_TIMINGR_SCLDEL          	20U     // SCL delay, 4 bits: [23:20]
#define I2C_TIMINGR_PRESC           	28U     // Timing prescaler, 4 bits: [31:28]

/*
 * Bit position definitions for I2C_TIMEOUTR
 */
#define I2C_TIMEOUTR_TIMEOUTA       	0U      // Bus timeout A, 12 bits: [11:0]
#define I2C_TIMEOUTR_TIDLE          	12U     // Idle clock timeout detection
#define I2C_TIMEOUTR_TIMOUTEN       	15U     // Clock timeout enable

#define I2C_TIMEOUTR_TIMEOUTB       	16U     // Bus timeout B, 12 bits: [27:16]
#define I2C_TIMEOUTR_TEXTEN         	31U     // Extended clock timeout enable

/*
 * Bit position definitions for I2C_ISR
 */
#define I2C_ISR_TXE                 	0U      // TX data register empty
#define I2C_ISR_TXIS                	1U      // TX interrupt status
#define I2C_ISR_RXNE                	2U      // RX data register not empty
#define I2C_ISR_ADDR                	3U      // Address matched
#define I2C_ISR_NACKF               	4U      // NACK received flag
#define I2C_ISR_STOPF               	5U      // STOP detection flag
#define I2C_ISR_TC                  	6U      // Transfer complete
#define I2C_ISR_TCR                 	7U      // Transfer complete reload

#define I2C_ISR_BERR                	8U      // Bus error
#define I2C_ISR_ARLO                	9U      // Arbitration lost
#define I2C_ISR_OVR                 	10U     // Overrun/underrun
#define I2C_ISR_PECERR              	11U     // PEC error
#define I2C_ISR_TIMEOUT             	12U     // Timeout or tLOW detection
#define I2C_ISR_ALERT               	13U     // SMBus alert

#define I2C_ISR_BUSY                	15U     // Bus busy
#define I2C_ISR_DIR                 	16U     // Transfer direction, target mode
#define I2C_ISR_ADDCODE             	17U     // Address match code, 7 bits: [23:17]

/*
 * Bit position definitions for I2C_ICR
 */
#define I2C_ICR_ADDRCF              	3U      // Address matched flag clear
#define I2C_ICR_NACKCF              	4U      // NACK flag clear
#define I2C_ICR_STOPCF              	5U      // STOP detection flag clear

#define I2C_ICR_BERRCF              	8U      // Bus error flag clear
#define I2C_ICR_ARLOCF              	9U      // Arbitration lost flag clear
#define I2C_ICR_OVRCF               	10U     // Overrun/underrun flag clear
#define I2C_ICR_PECCF               	11U     // PEC error flag clear
#define I2C_ICR_TIMOUTCF            	12U     // Timeout flag clear
#define I2C_ICR_ALERTCF             	13U     // Alert flag clear

/*
 * Bit position definitions for I2C_PECR
 */
#define I2C_PECR_PEC                	0U      // Packet error checking register, 8 bits: [7:0]

/*
 * Bit position definitions for I2C_RXDR
 */
#define I2C_RXDR_RXDATA             	0U      // 8-bit receive data

/*
 * Bit position definitions for I2C_TXDR
 */
#define I2C_TXDR_TXDATA             	0U      // 8-bit transmit data

/* -------------------------------------------------------------------------- */
/* Interrupt IRQ number macros                                                */
/* -------------------------------------------------------------------------- */
/*
 * STM32H7S3 IRQ numbers.
 *
 * Source: RM0477 Rev 9, Section 19.1.2, Table 142.
 * EXTI0..EXTI15 are mapped to individual IRQ lines on STM32H7Rx/7Sx.
 */

#define IRQ_NO_EXTI0              		16
#define IRQ_NO_EXTI1              		17
#define IRQ_NO_EXTI2              		18
#define IRQ_NO_EXTI3              		19
#define IRQ_NO_EXTI4              		20
#define IRQ_NO_EXTI5              		21
#define IRQ_NO_EXTI6              		22
#define IRQ_NO_EXTI7              		23
#define IRQ_NO_EXTI8              		24
#define IRQ_NO_EXTI9              		25
#define IRQ_NO_EXTI10            		26
#define IRQ_NO_EXTI11             		27
#define IRQ_NO_EXTI12             		28
#define IRQ_NO_EXTI13             		29
#define IRQ_NO_EXTI14             		30
#define IRQ_NO_EXTI15             		31

/* NVIC logical priority levels. */
#define NVIC_IRQ_PRIO0					0U
#define NVIC_IRQ_PRIO1					1U
#define NVIC_IRQ_PRIO2					2U
#define NVIC_IRQ_PRIO3					3U
#define NVIC_IRQ_PRIO4					4U
#define NVIC_IRQ_PRIO5					5U
#define NVIC_IRQ_PRIO6					6U
#define NVIC_IRQ_PRIO7					7U
#define NVIC_IRQ_PRIO8					8U
#define NVIC_IRQ_PRIO9					9U
#define NVIC_IRQ_PRIO10					10U
#define NVIC_IRQ_PRIO11					11U
#define NVIC_IRQ_PRIO12					12U
#define NVIC_IRQ_PRIO13					13U
#define NVIC_IRQ_PRIO14					14U
#define NVIC_IRQ_PRIO15					15U

/* -------------------------------------------------------------------------- */
/* Common status macros                                                       */
/* -------------------------------------------------------------------------- */
#define ENABLE 							1U
#define DISABLE 						0U

#define SET 							ENABLE
#define RESET 							DISABLE

#define GPIO_PIN_SET					SET
#define GPIO_PIN_RESET					RESET

#define FLAG_SET            			1U
#define FLAG_RESET          			0U


#endif /* INC_STM32H7SXX_H_ */
