/*
 *	ST7735 driver for CH32V003
 *	V 0.0.1
 */


#include <stdio.h>
#include <string.h>

#include "debug.h"
#include "ch32v00x.h"
#include "tick.h"
#include "st7735.h"


const uint8_t arg_set0[]  = {0x01, 0x2C, 0x2D};
const uint8_t arg_set1[]  = {0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D};
const uint8_t arg_set2[]  = {0x07};
const uint8_t arg_set3[]  = {0x62, 0x02, 0x04};
const uint8_t arg_set4[]  = {0xC0};
const uint8_t arg_set5[]  = {0x0D, 0x00};
const uint8_t arg_set6[]  = {0x8D, 0x6A};
const uint8_t arg_set7[]  = {0x8D, 0xFE};
const uint8_t arg_set8[]  = {0x0E};
const uint8_t arg_set9[]  = {0x10, 0x0E, 0x02, 0x03, 0x0E, 0x07, 0x02, 0x07,
                             0x0A, 0x12, 0x27, 0x37, 0x00, 0x0D, 0x0E, 0x10};
const uint8_t arg_set10[] = {0x10, 0x0E, 0x03, 0x03, 0x0F, 0x06, 0x02, 0x08,
                             0x0A, 0x13, 0x26, 0x36, 0x00, 0x0D, 0x0E, 0x10};
const uint8_t arg_set11[] = {0x05};

// Command table
const lcd_command_t lcd_init_commands[] = {
    #ifdef INVOFF
    {ST7735_INVOFF,     NULL,          0},
    #endif
    #ifdef INVON
    {ST7735_INVON,     NULL,          0},
    #endif
    {ST7735_FRMCTR1,   arg_set0,      3},
    {ST7735_FRMCTR2,   arg_set0,      3},
    {ST7735_FRMCTR3,   arg_set1,      6},
    {0x4B,             arg_set2,      1},
    {ST7735_PWCTR1,    arg_set3,      3},
    {ST7735_PWCTR2,    arg_set4,      1},
    {ST7735_PWCTR3,    arg_set5,      2},
    {ST7735_PWCTR4,    arg_set6,      2},
    {ST7735_PWCTR5,    arg_set7,      2},
    {ST7735_VMCTR1,    arg_set8,      1},
    {ST7735_GMCTRP1,   arg_set9,      16},
    {ST7735_GMCTRN1,   arg_set10,     16},
    {ST7735_COLMOD,    arg_set11,     1}
};

#define LCD_INIT_COMMANDS_COUNT (sizeof(lcd_init_commands) / sizeof(lcd_command_t))


static uint8_t lcd_rotation = LCD_DEFAULT_ROTATION;

static volatile uint8_t dma_tx_cplt = 1;

void DMA1_Channel3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));


static void _LCD_SPI_Send(uint8_t byte)
{
	while((SPI1->STATR & SPI_STATR_TXE) != SPI_STATR_TXE){};
	SPI1->DATAR = byte;
	while((SPI1->STATR & SPI_STATR_BSY) == SPI_STATR_BSY){};
}

static void _LCD_SPI_Send16(uint16_t data)
{
	while((SPI1->STATR & SPI_STATR_TXE) != SPI_STATR_TXE){};
	SPI1->DATAR = data;
	while((SPI1->STATR & SPI_STATR_BSY) == SPI_STATR_BSY){};
}


static void _LCD_SendCommand(uint8_t cmd, const uint8_t *arg, uint8_t arg_len)
{
    LCD_CS_LOW;
    LCD_DC_LOW;

    _LCD_SPI_Send(cmd);

    LCD_DC_HIGH;

    if(arg_len > 0)
    {
        for (uint8_t i = 0; i < arg_len; i++)
        {
            _LCD_SPI_Send(*arg++);
        }
    }
    
    LCD_CS_HIGH;
}

static void _LCD_HardwareInit(void)
{
	// Enable clocks
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOC | RCC_APB2Periph_SPI1;
	RCC->AHBPCENR |= RCC_AHBPeriph_DMA1;

	// GPIOs configuration: PC6(MOSI) PC5(SCK) PC4(DC) PC3(RS) PC2(CS)
	GPIOC->CFGLR = GPIO_CFGLR_CNF6_1 | GPIO_CFGLR_MODE6 | GPIO_CFGLR_CNF5_1 | GPIO_CFGLR_MODE5 | GPIO_CFGLR_MODE4 | GPIO_CFGLR_MODE3 | GPIO_CFGLR_MODE2;

	// SPI configuration: Mode master, Only TX, Software CS
	SPI1->CTLR1 = SPI_CTLR1_SSI | SPI_CTLR1_SSM | SPI_CTLR1_MSTR | SPI_CTLR1_SPE | SPI_CTLR1_BIDIOE;

	// Enable DMA for SPI TX
	SPI1->CTLR2 |= SPI_CTLR2_TXDMAEN;

	// DMA configuration
	DMA1_Channel3->CFGR |= DMA_Priority_VeryHigh | DMA_MemoryDataSize_HalfWord | DMA_PeripheralDataSize_HalfWord | DMA_MemoryInc_Enable | DMA_DIR_PeripheralDST | DMA_IT_TC;
	DMA1_Channel3->PADDR = (uint32_t)&SPI1->DATAR;

	NVIC_EnableIRQ(DMA1_Channel3_IRQn);
}

static void _LCD_SPI_SwitchTo8bit(void)
{
	SPI1->CTLR1 &= ~SPI_CTLR1_DFF;
}

static void _LCD_SPI_SwitchTo16bit(void)
{
	SPI1->CTLR1 |= SPI_CTLR1_DFF;
}

void LCD_Init(void)
{
	_LCD_HardwareInit();

    LCD_RS_LOW;
    Delay(200);
    LCD_RS_HIGH;
    Delay(20);

    _LCD_SendCommand(ST7735_SLPOUT, 0, 0);
    Delay(100);

    for(uint8_t i = 0; i < LCD_INIT_COMMANDS_COUNT; i++)
    {
        _LCD_SendCommand(lcd_init_commands[i].command, lcd_init_commands[i].args, lcd_init_commands[i].arg_count);
    }

    LCD_SetRotation(LCD_DEFAULT_ROTATION);

    _LCD_SendCommand(ST7735_DISPON, 0, 0);
    _LCD_SPI_SwitchTo16bit();

}

void LCD_SetRotation(uint8_t rotation)
{
    uint8_t arg[] = {0x00};

    #ifdef  COLOR_ORDER_BGR
    switch (rotation)
    {
    case 0:
        arg[0] = (0x00 | (1 << 3));
        break;

    case 1:
        arg[0] = (0xC0 | (1 << 3));
        break;

    case 2:
        arg[0] = (0x70 | (1 << 3));
        break;

    case 3:
        arg[0] = (0xA0 | (1 << 3));
        break;

    default:
        return;
    }
    #endif

    #ifdef COLOR_ORDER_RGB
    switch (rotation)
    {
    case 0:
        arg[0] = (0x00);
        break;

    case 1:
        arg[0] = (0xC0);
        break;

    case 2:
        arg[0] = (0x70);
        break;

    case 3:
        arg[0] = (0xA0);
        break;

    default:
        return;
    }
    #endif

    _LCD_SPI_SwitchTo8bit();
    _LCD_SendCommand(ST7735_MADCTL, arg, 1);
    _LCD_SPI_SwitchTo16bit();
    lcd_rotation = rotation;
}

void LCD_SetWindow(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h)
{
    uint16_t x2 = (x1 + w - 1), y2 = (y1 + h - 1);
    uint8_t arg0[4] = {0x00, 0x00, 0x00, 0x00};
    uint8_t arg1[4]= {0x00, 0x00, 0x00, 0x00};

    #ifdef OFFSET_1_26

    switch (lcd_rotation)
    {
    case 0:
        arg0[1] = x1 + 26;
        arg0[3] = x2 + 26;
        arg1[1] = y1 + 1;
        arg1[3] = y2 + 1;
        break;

    case 1:
        arg0[1] = x1 + 26;
        arg0[3] = x2 + 26;
        arg1[1] = y1 + 1;
        arg1[3] = y2 + 1;
        break;

    case 2:
        arg0[1] = x1 + 1;
        arg0[3] = x2 + 1;
        arg1[1] = y1 + 26;
        arg1[3] = y2 + 26;
        break;

    case 3:
        arg0[1] = x1 + 1;
        arg0[3] = x2 + 1;
        arg1[1] = y1 + 26;
        arg1[3] = y2 + 26;
        break;
    
    default:
        return;
    }

    #endif

    #ifdef OFFSET_0_24

    switch (lcd_rotation)
    {
    case 0:
        arg0[1] = x1 + 24;
        arg0[3] = x2 + 24;
        arg1[1] = y1 + 0;
        arg1[3] = y2 + 0;
        break;

    case 1:
        arg0[1] = x1 + 24;
        arg0[3] = x2 + 24;
        arg1[1] = y1 + 0;
        arg1[3] = y2 + 0;
        break;

    case 2:
        arg0[1] = x1 + 0;
        arg0[3] = x2 + 0;
        arg1[1] = y1 + 24;
        arg1[3] = y2 + 24;
        break;

    case 3:
        arg0[1] = x1 + 0;
        arg0[3] = x2 + 0;
        arg1[1] = y1 + 24;
        arg1[3] = y2 + 24;
        break;
    
    default:
        return;
    }

    #endif

    _LCD_SPI_SwitchTo8bit();
    _LCD_SendCommand(ST7735_CASET, arg0, 4);
    _LCD_SendCommand(ST7735_RASET, arg1, 4);
    _LCD_SendCommand(ST7735_RAMWR, 0, 0);
    _LCD_SPI_SwitchTo16bit();

}

void LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
	LCD_SetWindow(x,y,1,1);
	LCD_CS_LOW;
	_LCD_SPI_Send16(color);
	LCD_CS_HIGH;
}

void LCD_OptimizedFill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
	LCD_SetWindow(x, y, w, h);

	LCD_CS_LOW;
	for(uint32_t i = 0; i < w*h; i++)
	{
		_LCD_SPI_Send16(color);
	}
	LCD_CS_HIGH;
}

void LCD_WriteRawData(uint16_t *data, uint16_t size)
{
	LCD_CS_LOW;
	for(uint32_t i = 0; i < size; i++)
	{
		_LCD_SPI_Send16(data[i]);
	}
	LCD_CS_HIGH;
}

void LCD_WriteRawDataDMA(uint16_t *data, uint16_t size)
{
	while(!dma_tx_cplt);

	LCD_CS_LOW;

	dma_tx_cplt = 0;

	DMA1_Channel3->CFGR &= ~DMA_CFGR3_EN;
	//Buffer address
	DMA1_Channel3->MADDR = (uint32_t)data;
	//Number of data transfer
	DMA1_Channel3->CNTR = (uint16_t)size;

	//Enable DMA Channel
	DMA1_Channel3->CFGR |= DMA_CFGR3_EN;
}

void LCD_DMA_Wait(void)
{
	while(!dma_tx_cplt);
}

void DMA1_Channel3_IRQHandler(void)
{
	// Clear flag
	DMA1->INTFCR |= DMA_CGIF3;
	while(SPI1->STATR & SPI_STATR_BSY);
	LCD_CS_HIGH;
	dma_tx_cplt = 1;
}





