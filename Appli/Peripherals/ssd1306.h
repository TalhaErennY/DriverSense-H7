/*
 * ssd1306.h
 *
 *  Created on: 5 Haz 2026
 *      Author: talha
 */

#ifndef INC_SSD1306_H_
#define INC_SSD1306_H_

#include <stdint.h>
#include "stm32h7sxx_i2c_driver.h"

#define SSD1306_X_OFFSET                1U

/*
 * @SSD1306_DISPLAY_CONFIGURATION
 * SSD1306 display size and buffer configuration macros
 */
#define SSD1306_WIDTH              128U
#define SSD1306_HEIGHT             64U
#define SSD1306_PAGE_COUNT         8U
#define SSD1306_BUFFER_SIZE        (SSD1306_WIDTH * SSD1306_HEIGHT / 8U)

/*
 * @SSD1306_I2C_ADDRESS
 * SSD1306 I2C address selection macro
 *
 * SA0 = 0 -> 0x3C
 * SA0 = 1 -> 0x3D
 */
#define SSD1306_I2C_ADDR           0x3CU

/*
 * @SSD1306_PIXEL_STATES
 * OLED pixel state selection macros
 */
#define SSD1306_PIXEL_OFF          0U
#define SSD1306_PIXEL_ON           1U

/*
 * Dashboard data structure
 */
typedef struct
{
    uint8_t throttle;      /* 0-100 */
    uint8_t brakeRL;       /* 0-100 */
    uint8_t brakeRR;       /* 0-100 */
    uint8_t brakeFL;       /* 0-100 */
    uint8_t brakeFR;       /* 0-100 */
    uint16_t speed;        /* km/h */
    uint16_t timer_ms;     /* example */
} Dashboard_Data_t;

/******************************************************************************************************
 * 								APIs Supported by This Driver
 *       For more information about the APIs, check the function definitions in ssd1306.c
 ******************************************************************************************************/

/*
 * Basic OLED APIs
 */
uint8_t SSD1306_Init(I2C_Handle_t *pI2CHandle);
void SSD1306_Clear(void);
void SSD1306_Fill(void);
uint8_t SSD1306_UpdateScreen(void);

/*
 * Drawing APIs
 */
void SSD1306_DrawPixel(uint8_t x, uint8_t y, uint8_t state);
void SSD1306_DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t state);
void SSD1306_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t state);
void SSD1306_DrawHBar(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t percent);
void SSD1306_DrawVBar(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t percent);

/*
 * Simple Numeric Display APIs
 */
void SSD1306_DrawDigitLarge(uint8_t x, uint8_t y, uint8_t digit);
void SSD1306_DrawNumberLarge(uint8_t x, uint8_t y, uint16_t number);

/*
 * Dashboard API
 */
void SSD1306_DrawDashboard(const Dashboard_Data_t *data);

/*
 * Text APIs
 */
void SSD1306_DrawChar5x7(uint8_t x, uint8_t y, char ch);
void SSD1306_DrawString5x7(uint8_t x, uint8_t y, const char *str);

#endif /* INC_SSD1306_H_ */
