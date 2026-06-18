/*
 * ssd1306.c
 *
 *  Created on: 5 Haz 2026
 *      Author: talha
 */

#include "ssd1306.h"

static I2C_Handle_t *SSD1306_I2CHandle = 0;

static uint8_t SSD1306_Buffer[SSD1306_BUFFER_SIZE];
static uint8_t SSD1306_Dirty = 0U;

/*
 * If same data comes again, do not update dashboard area again.
 */
static Dashboard_Data_t LastDashboardData;
static uint8_t LastDashboardValid = 0U;

/*
 * Private Helper Functions
 */
static uint8_t SSD1306_SendCommand(uint8_t command);
static uint8_t SSD1306_SendCommandList(const uint8_t *pCommands, uint32_t Len);
static uint8_t SSD1306_SendData(uint8_t *pData, uint32_t Len);
static uint8_t SSD1306_DashboardChanged(const Dashboard_Data_t *data);

/*
 * Basic OLED APIs
 */
uint8_t SSD1306_Init(I2C_Handle_t *pI2CHandle)
{
    uint8_t status;

    static const uint8_t initCommands[] =
    {
        0xAE,       // Display OFF
        0x20, 0x00, // Memory addressing mode = horizontal
        0xB0,       // Page start address
        0xC8,       // COM scan direction remapped
        0x00,       // Lower column address
        0x10,       // Higher column address
        0x40,       // Start line = 0
        0x81, 0x7F, // Contrast
        0xA1,       // Segment remap
        0xA6,       // Normal display
        0xA8, 0x3F, // Multiplex ratio = 64
        0xA4,       // Display follows RAM
        0xD3, 0x00, // Display offset = 0
        0xD5, 0x80, // Clock divide / oscillator
        0xD9, 0xF1, // Pre-charge
        0xDA, 0x12, // COM pins config for 128x64
        0xDB, 0x40, // VCOMH
        0x8D, 0x14, // Charge pump enable
        0x2E,       // Deactivate scroll
        0xAF        // Display ON
    };

    if (pI2CHandle == 0)
    {
        return I2C_ERROR_INVALID_ARG;
    }

    SSD1306_I2CHandle = pI2CHandle;

    status = SSD1306_SendCommandList(initCommands, sizeof(initCommands));

    if (status != I2C_OK)
    {
        return status;
    }

    SSD1306_Clear();

    status = SSD1306_UpdateScreen();

    return status;
}

void SSD1306_Clear(void)
{
    uint32_t i;

    for (i = 0U; i < SSD1306_BUFFER_SIZE; i++)
    {
        SSD1306_Buffer[i] = 0x00U;
    }

    SSD1306_Dirty = 1U;
}

void SSD1306_Fill(void)
{
    uint32_t i;

    for (i = 0U; i < SSD1306_BUFFER_SIZE; i++)
    {
        SSD1306_Buffer[i] = 0xFFU;
    }

    SSD1306_Dirty = 1U;
}

/*
 * Draw one pixel
 *
 * For state selection, see @SSD1306_PIXEL_STATES macros.
 */
void SSD1306_DrawPixel(uint8_t x, uint8_t y, uint8_t state)
{
    uint16_t index;
    uint8_t bit_mask;
    uint8_t physical_x;

    physical_x = (uint8_t)(x + SSD1306_X_OFFSET);

    if ((physical_x >= SSD1306_WIDTH) || (y >= SSD1306_HEIGHT))
    {
        return;
    }

    /*
     * SSD1306:
     * 1 byte = vertical 8 pixel
     *
     * y / 8 -> page
     * y % 8 -> bit in byte
     */
    index = (uint16_t)physical_x + ((uint16_t)(y / 8U) * SSD1306_WIDTH);
    bit_mask = (uint8_t)(1U << (y % 8U));

    if (state == SSD1306_PIXEL_ON)
    {
        SSD1306_Buffer[index] |= bit_mask;
    }
    else
    {
        SSD1306_Buffer[index] &= (uint8_t)(~bit_mask);
    }

    SSD1306_Dirty = 1U;
}

/*
 * Draw rectangle border
 *
 * For state selection, see @SSD1306_PIXEL_STATES macros.
 */
void SSD1306_DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t state)
{
    uint8_t i;

    if ((w == 0U) || (h == 0U))
    {
        return;
    }

    for (i = 0U; i < w; i++)
    {
        SSD1306_DrawPixel(x + i, y, state);
        SSD1306_DrawPixel(x + i, y + h - 1U, state);
    }

    for (i = 0U; i < h; i++)
    {
        SSD1306_DrawPixel(x, y + i, state);
        SSD1306_DrawPixel(x + w - 1U, y + i, state);
    }
}

/*
 * Fill rectangle area
 *
 * For state selection, see @SSD1306_PIXEL_STATES macros.
 */
void SSD1306_FillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t state)
{
    uint8_t i;
    uint8_t j;

    for (j = 0U; j < h; j++)
    {
        for (i = 0U; i < w; i++)
        {
            SSD1306_DrawPixel(x + i, y + j, state);
        }
    }
}

/*
 * Draw Horizontal Bar
 *
 * For state selections used inside this function, see @SSD1306_PIXEL_STATES macros.
 */
void SSD1306_DrawHBar(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t percent)
{
    uint8_t fill_w;

    if (percent > 100U)
    {
        percent = 100U;
    }

    SSD1306_DrawRect(x, y, w, h, SSD1306_PIXEL_ON);

    /*
     * Clear inside the shape
     */
    if ((w > 2U) && (h > 2U))
    {
        SSD1306_FillRect(x + 1U, y + 1U, w - 2U, h - 2U, SSD1306_PIXEL_OFF);
    }

    fill_w = (uint8_t)(((uint16_t)(w - 2U) * percent) / 100U);

    if (fill_w > 0U)
    {
        SSD1306_FillRect(x + 1U, y + 1U, fill_w, h - 2U, SSD1306_PIXEL_ON);
    }
}

/*
 * Draw Vertical Bar
 *
 * For state selections used inside this function, see @SSD1306_PIXEL_STATES macros.
 */
void SSD1306_DrawVBar(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t percent)
{
    uint8_t fill_h;

    if (percent > 100U)
    {
        percent = 100U;
    }

    SSD1306_DrawRect(x, y, w, h, SSD1306_PIXEL_ON);

    if ((w > 2U) && (h > 2U))
    {
        SSD1306_FillRect(x + 1U, y + 1U, w - 2U, h - 2U, SSD1306_PIXEL_OFF);
    }

    fill_h = (uint8_t)(((uint16_t)(h - 2U) * percent) / 100U);

    if (fill_h > 0U)
    {
        /*
         * Bar fills from bottom to top
         */
        SSD1306_FillRect(x + 1U,
                         y + h - 1U - fill_h,
                         w - 2U,
                         fill_h,
                         SSD1306_PIXEL_ON);
    }
}

/*
 * 5x7 Text Helper
 *
 * Each byte represents one vertical column.
 * Bit0 = top pixel, Bit6 = bottom pixel.
 */
static void SSD1306_GetChar5x7(char ch, uint8_t out[5])
{
    uint8_t i;

    for (i = 0U; i < 5U; i++)
    {
        out[i] = 0x00U;
    }

    switch (ch)
    {
        case '0': out[0]=0x3E; out[1]=0x51; out[2]=0x49; out[3]=0x45; out[4]=0x3E; break;
        case '1': out[0]=0x00; out[1]=0x42; out[2]=0x7F; out[3]=0x40; out[4]=0x00; break;
        case '2': out[0]=0x42; out[1]=0x61; out[2]=0x51; out[3]=0x49; out[4]=0x46; break;
        case '3': out[0]=0x21; out[1]=0x41; out[2]=0x45; out[3]=0x4B; out[4]=0x31; break;
        case '4': out[0]=0x18; out[1]=0x14; out[2]=0x12; out[3]=0x7F; out[4]=0x10; break;
        case '5': out[0]=0x27; out[1]=0x45; out[2]=0x45; out[3]=0x45; out[4]=0x39; break;
        case '6': out[0]=0x3C; out[1]=0x4A; out[2]=0x49; out[3]=0x49; out[4]=0x30; break;
        case '7': out[0]=0x01; out[1]=0x71; out[2]=0x09; out[3]=0x05; out[4]=0x03; break;
        case '8': out[0]=0x36; out[1]=0x49; out[2]=0x49; out[3]=0x49; out[4]=0x36; break;
        case '9': out[0]=0x06; out[1]=0x49; out[2]=0x49; out[3]=0x29; out[4]=0x1E; break;

        case 'A': out[0]=0x7E; out[1]=0x11; out[2]=0x11; out[3]=0x11; out[4]=0x7E; break;
        case 'B': out[0]=0x7F; out[1]=0x49; out[2]=0x49; out[3]=0x49; out[4]=0x36; break;
        case 'C': out[0]=0x3E; out[1]=0x41; out[2]=0x41; out[3]=0x41; out[4]=0x22; break;
        case 'D': out[0]=0x7F; out[1]=0x41; out[2]=0x41; out[3]=0x22; out[4]=0x1C; break;
        case 'E': out[0]=0x7F; out[1]=0x49; out[2]=0x49; out[3]=0x49; out[4]=0x41; break;
        case 'F': out[0]=0x7F; out[1]=0x09; out[2]=0x09; out[3]=0x09; out[4]=0x01; break;
        case 'G': out[0]=0x3E; out[1]=0x41; out[2]=0x49; out[3]=0x49; out[4]=0x7A; break;
        case 'H': out[0]=0x7F; out[1]=0x08; out[2]=0x08; out[3]=0x08; out[4]=0x7F; break;
        case 'I': out[0]=0x00; out[1]=0x41; out[2]=0x7F; out[3]=0x41; out[4]=0x00; break;
        case 'J': out[0]=0x20; out[1]=0x40; out[2]=0x41; out[3]=0x3F; out[4]=0x01; break;
        case 'K': out[0]=0x7F; out[1]=0x08; out[2]=0x14; out[3]=0x22; out[4]=0x41; break;
        case 'L': out[0]=0x7F; out[1]=0x40; out[2]=0x40; out[3]=0x40; out[4]=0x40; break;
        case 'M': out[0]=0x7F; out[1]=0x02; out[2]=0x0C; out[3]=0x02; out[4]=0x7F; break;
        case 'N': out[0]=0x7F; out[1]=0x04; out[2]=0x08; out[3]=0x10; out[4]=0x7F; break;
        case 'O': out[0]=0x3E; out[1]=0x41; out[2]=0x41; out[3]=0x41; out[4]=0x3E; break;
        case 'P': out[0]=0x7F; out[1]=0x09; out[2]=0x09; out[3]=0x09; out[4]=0x06; break;
        case 'Q': out[0]=0x3E; out[1]=0x41; out[2]=0x51; out[3]=0x21; out[4]=0x5E; break;
        case 'R': out[0]=0x7F; out[1]=0x09; out[2]=0x19; out[3]=0x29; out[4]=0x46; break;
        case 'S': out[0]=0x46; out[1]=0x49; out[2]=0x49; out[3]=0x49; out[4]=0x31; break;
        case 'T': out[0]=0x01; out[1]=0x01; out[2]=0x7F; out[3]=0x01; out[4]=0x01; break;
        case 'U': out[0]=0x3F; out[1]=0x40; out[2]=0x40; out[3]=0x40; out[4]=0x3F; break;
        case 'V': out[0]=0x1F; out[1]=0x20; out[2]=0x40; out[3]=0x20; out[4]=0x1F; break;
        case 'W': out[0]=0x7F; out[1]=0x20; out[2]=0x18; out[3]=0x20; out[4]=0x7F; break;
        case 'X': out[0]=0x63; out[1]=0x14; out[2]=0x08; out[3]=0x14; out[4]=0x63; break;
        case 'Y': out[0]=0x07; out[1]=0x08; out[2]=0x70; out[3]=0x08; out[4]=0x07; break;
        case 'Z': out[0]=0x61; out[1]=0x51; out[2]=0x49; out[3]=0x45; out[4]=0x43; break;

        case ':': out[0]=0x00; out[1]=0x36; out[2]=0x36; out[3]=0x00; out[4]=0x00; break;
        case '.': out[0]=0x00; out[1]=0x60; out[2]=0x60; out[3]=0x00; out[4]=0x00; break;
        case '-': out[0]=0x08; out[1]=0x08; out[2]=0x08; out[3]=0x08; out[4]=0x08; break;
        case '/': out[0]=0x20; out[1]=0x10; out[2]=0x08; out[3]=0x04; out[4]=0x02; break;
        case '%': out[0]=0x63; out[1]=0x13; out[2]=0x08; out[3]=0x64; out[4]=0x63; break;
        case '=': out[0]=0x14; out[1]=0x14; out[2]=0x14; out[3]=0x14; out[4]=0x14; break;
        case ' ': default: break;
    }
}

void SSD1306_DrawChar5x7(uint8_t x, uint8_t y, char ch)
{
    uint8_t col;
    uint8_t row;
    uint8_t bitmap[5];

    SSD1306_GetChar5x7(ch, bitmap);

    for (col = 0U; col < 5U; col++)
    {
        for (row = 0U; row < 7U; row++)
        {
            if ((bitmap[col] & (1U << row)) != 0U)
            {
                SSD1306_DrawPixel((uint8_t)(x + col),
                                  (uint8_t)(y + row),
                                  SSD1306_PIXEL_ON);
            }
        }
    }
}

void SSD1306_DrawString5x7(uint8_t x, uint8_t y, const char *str)
{
    uint8_t cursor_x = x;

    if (str == 0)
    {
        return;
    }

    while (*str != '\0')
    {
        if (cursor_x > 122U)
        {
            break;
        }

        SSD1306_DrawChar5x7(cursor_x, y, *str);
        cursor_x = (uint8_t)(cursor_x + 6U);
        str++;
    }
}

/*
 * Büyük 7-segment tarzı rakam.
 *
 * Segmentler:
 *
 *   A
 * F   B
 *   G
 * E   C
 *   D
 */
void SSD1306_DrawDigitLarge(uint8_t x, uint8_t y, uint8_t digit)
{
    uint8_t seg;

    static const uint8_t digitSegments[10] =
    {
        0x3F, // 0: A B C D E F
        0x06, // 1: B C
        0x5B, // 2: A B D E G
        0x4F, // 3: A B C D G
        0x66, // 4: B C F G
        0x6D, // 5: A C D F G
        0x7D, // 6: A C D E F G
        0x07, // 7: A B C
        0x7F, // 8: A B C D E F G
        0x6F  // 9: A B C D F G
    };

    if (digit > 9U)
    {
        return;
    }

    seg = digitSegments[digit];

    /*
     * A
     */
    if (seg & 0x01U)
    {
        SSD1306_FillRect(x + 2U, y, 10U, 2U, SSD1306_PIXEL_ON);
    }

    /*
     * B
     */
    if (seg & 0x02U)
    {
        SSD1306_FillRect(x + 12U, y + 2U, 2U, 10U, SSD1306_PIXEL_ON);
    }

    /*
     * C
     */
    if (seg & 0x04U)
    {
        SSD1306_FillRect(x + 12U, y + 14U, 2U, 10U, SSD1306_PIXEL_ON);
    }

    /*
     * D
     */
    if (seg & 0x08U)
    {
        SSD1306_FillRect(x + 2U, y + 24U, 10U, 2U, SSD1306_PIXEL_ON);
    }

    /*
     * E
     */
    if (seg & 0x10U)
    {
        SSD1306_FillRect(x, y + 14U, 2U, 10U, SSD1306_PIXEL_ON);
    }

    /*
     * F
     */
    if (seg & 0x20U)
    {
        SSD1306_FillRect(x, y + 2U, 2U, 10U, SSD1306_PIXEL_ON);
    }

    /*
     * G
     */
    if (seg & 0x40U)
    {
        SSD1306_FillRect(x + 2U, y + 12U, 10U, 2U, SSD1306_PIXEL_ON);
    }
}

/*
 * Draw large number using 7-segment style digits
 */
void SSD1306_DrawNumberLarge(uint8_t x, uint8_t y, uint16_t number)
{
    uint8_t hundreds;
    uint8_t tens;
    uint8_t ones;
    uint8_t pos = x;

    if (number > 999U)
    {
        number = 999U;
    }

    hundreds = number / 100U;
    tens = (number / 10U) % 10U;
    ones = number % 10U;

    if (hundreds > 0U)
    {
        SSD1306_DrawDigitLarge(pos, y, hundreds);
        pos += 17U;
    }

    if ((hundreds > 0U) || (tens > 0U))
    {
        SSD1306_DrawDigitLarge(pos, y, tens);
        pos += 17U;
    }

    SSD1306_DrawDigitLarge(pos, y, ones);
}

/*
 * If the same dashboard error occured dont update the screen
 */
static uint8_t SSD1306_DashboardChanged(const Dashboard_Data_t *data)
{
    if (data == 0)
    {
        return 0U;
    }

    if (LastDashboardValid == 0U)
    {
        LastDashboardData = *data;
        LastDashboardValid = 1U;
        return 1U;
    }

    if ((LastDashboardData.throttle != data->throttle) ||
        (LastDashboardData.brakeRL  != data->brakeRL)  ||
        (LastDashboardData.brakeRR  != data->brakeRR)  ||
        (LastDashboardData.brakeFL  != data->brakeFL)  ||
        (LastDashboardData.brakeFR  != data->brakeFR)  ||
        (LastDashboardData.speed    != data->speed)    ||
        (LastDashboardData.timer_ms != data->timer_ms))
    {
        LastDashboardData = *data;
        return 1U;
    }

    return 0U;
}

/*
 * Dashboard Layout
 *
 * 128x64:
 *
 * top: throttle bar
 * bottom sol: RL RR FL FR bars
 * right side: speed
 */
/*
 * Draw dashboard screen
 */
void SSD1306_DrawDashboard(const Dashboard_Data_t *data)
{
    uint8_t i;

    if (data == 0)
    {
        return;
    }

    /*
     * If no change in the data dont update the screen
     */
    if (SSD1306_DashboardChanged(data) == 0U)
    {
        return;
    }

    SSD1306_Clear();

    /*
     * Outlines
     */
    SSD1306_DrawRect(0U, 0U, 128U, 64U, SSD1306_PIXEL_ON);

    /*
     * Throttle bar
     */
    SSD1306_DrawHBar(6U, 6U, 70U, 10U, data->throttle);

    /*
     * Throttle effect
     */
    for (i = 0U; i < 8U; i++)
    {
        uint8_t h = (uint8_t)((data->throttle * (i + 1U)) / 100U);
        if (h > 12U)
        {
            h = 12U;
        }

        SSD1306_FillRect(10U + i * 7U,
                         29U - h,
                         3U,
                         h,
                         SSD1306_PIXEL_ON);
    }

    /*
     * Bottom bars
     */
    SSD1306_DrawVBar(7U, 33U, 14U, 25U, data->brakeRL);
    SSD1306_DrawVBar(28U, 33U, 14U, 25U, data->brakeRR);
    SSD1306_DrawVBar(49U, 33U, 14U, 25U, data->brakeFL);
    SSD1306_DrawVBar(70U, 33U, 14U, 25U, data->brakeFR);

    /*
     * Speedometer
     */
    SSD1306_DrawNumberLarge(88U, 16U, data->speed);

    /*
     * Timer bar 0-1000ms
     */
    SSD1306_DrawHBar(88U, 51U, 34U, 7U, (data->timer_ms > 1000U) ? 100U : (uint8_t)(data->timer_ms / 10U));
}

/*
 * Send framebuffer to SSD1306 display RAM
 */
uint8_t SSD1306_UpdateScreen(void)
{
    uint8_t page;
    uint8_t status;

    if (SSD1306_Dirty == 0U)
    {
        return I2C_OK;
    }

    for (page = 0U; page < SSD1306_PAGE_COUNT; page++)
    {
        status = SSD1306_SendCommand((uint8_t)(0xB0U + page));

        if (status != I2C_OK)
        {
            return status;
        }

        status = SSD1306_SendCommand(0x00U);

        if (status != I2C_OK)
        {
            return status;
        }

        status = SSD1306_SendCommand(0x10U);

        if (status != I2C_OK)
        {
            return status;
        }

        status = SSD1306_SendData(&SSD1306_Buffer[page * SSD1306_WIDTH],
                                  SSD1306_WIDTH);

        if (status != I2C_OK)
        {
            return status;
        }
    }

    SSD1306_Dirty = 0U;

    return I2C_OK;
}

//===============================PRIVATE HELPERS=======================================
/*
 * 0x00 = command control byte
 */
static uint8_t SSD1306_SendCommand(uint8_t command)
{
    uint8_t txData[2];

    if (SSD1306_I2CHandle == 0)
    {
        return I2C_ERROR_INVALID_ARG;
    }

    txData[0] = 0x00U;
    txData[1] = command;

    return I2C_MasterSendData(SSD1306_I2CHandle,
                              txData,
                              2U,
                              SSD1306_I2C_ADDR);
}

static uint8_t SSD1306_SendCommandList(const uint8_t *pCommands, uint32_t Len)
{
    uint32_t i;
    uint8_t status;

    if ((pCommands == 0) || (Len == 0U))
    {
        return I2C_ERROR_INVALID_ARG;
    }

    for (i = 0U; i < Len; i++)
    {
        status = SSD1306_SendCommand(pCommands[i]);

        if (status != I2C_OK)
        {
            return status;
        }
    }

    return I2C_OK;
}

/*
 * 0x40 = data control byte
 *
 * 1 control byte + 128 data byte = 129 byte
 */
static uint8_t SSD1306_SendData(uint8_t *pData, uint32_t Len)
{
    uint8_t txData[129];
    uint32_t i;

    if ((SSD1306_I2CHandle == 0) || (pData == 0) || (Len == 0U) || (Len > 128U))
    {
        return I2C_ERROR_INVALID_ARG;
    }

    txData[0] = 0x40U;

    for (i = 0U; i < Len; i++)
    {
        txData[i + 1U] = pData[i];
    }

    return I2C_MasterSendData(SSD1306_I2CHandle,
                              txData,
                              Len + 1U,
                              SSD1306_I2C_ADDR);
}
