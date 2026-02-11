/*
 *  Terminal Graphics
 *  DMA Sprite Version for CH32V003 V1.1
 */

#include <string.h>
#include <stdlib.h>

#include "debug.h"
#include "ch32v00x.h"
#include "st7735.h"
#include "termGFX.h"

static term_char_t term_buf[TERM_SIZE_X][TERM_SIZE_Y];

static uint32_t term_update_map[TERM_SIZE_Y];

static uint32_t term_sprite_map[TERM_SIZE_Y];

static term_sprite_t *term_sprites[TERM_SPRITES_COUNT];

static uint8_t sprites_count = TERM_SPRITES_COUNT;

static uint16_t term_character_buf[2][TERM_BLOCK_SIZE*TERM_BLOCK_SIZE];
static uint8_t term_current_buf;

static uint8_t term_x_pos;
static uint8_t term_y_pos;

static const uint16_t colors_lut[16] =
{
		0x0000,
		0x7861,
		0x1be4,
		0x7be4,
		0x000f,
		0x786f,
		0x1bef,
		0xbdf7,
		0x7bef,
		0xf903,
		0x3fe9,
		0xffe9,
		0x003e,
		0xf91f,
		0x37ff,
		0xffff
};

static const uint16_t color_pallets[][4] =
{
		{0, ST7735_WHITE, ST7735_WHITE, ST7735_WHITE},
		{0, 0x4488, 0x3e87, 0x0821/*0x6fe0*/},
		{0, 0xf785, ST7735_BLACK, ST7735_WHITE/*0x6fe0*/},
		{0, 0x6b4d, 0x6b4d, 0x6b4d/*0x6fe0*/},
};


//// For initing LCD by calling tGFX_Init. Not really needed.
//static void tGFX_LCD_Init(void)
//{
//
//}

// Set LCD drawing window (required)
static void tGFX_LCD_SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    LCD_SetWindow(x, y, w, h);
}

static void tGFX_LCD_SendData(uint16_t *data, uint16_t data_size)
{
	LCD_WriteRawData(data, data_size);
}


static void tGFX_LCD_DMA_SendData(uint16_t *data, uint16_t data_size)
{
	LCD_WriteRawDataDMA(data, data_size);
}

// Block program untill transfer is complete
static void tGFX_LCD_DMA_Wait(void)
{
	LCD_DMA_Wait();
}

static void tGFX_CharToBuf(term_char_t *character)
{
    for (size_t i = 0; i < TERM_BLOCK_SIZE; i++)
    {
        for (size_t j = 0; j < TERM_BLOCK_SIZE; j++)
        {

            if ((term_char_map[character->character][i] >> j) & 1)
            {

                term_character_buf[term_current_buf][ (7 - j) + i * TERM_BLOCK_SIZE] = colors_lut[character->c_fg];
            }
            else
            {
                term_character_buf[term_current_buf][(7 - j) + i * TERM_BLOCK_SIZE] = colors_lut[character->c_bg];
            }
        }
    }
}


static term_range_t tGFX_CheckCommonPartOfRange(term_range_t range1, term_range_t range2)
{
	term_range_t common_part;

	if(range1.start <= range2.end && range2.start <= range1.end)
	{
		common_part.start = (range1.start > range2.start) ? range1.start : range2.start;
		common_part.end = (range1.end < range2.end) ? range1.end : range2.end;
	}
	else
	{
		common_part.start = 0xFF;
		common_part.end = 0xFF;
	}

	return common_part;

}

/**
 * @brief Draws a portion of a sprite onto the specified block within the terminal graphics buffer.
 *
 * This function handles the extraction and rendering of the sprite's pixel data within a given
 * range. Each pixel is encoded using 2 bits, allowing for a 4-color pallet. Only non-transparent
 * pixels (non-zero) are rendered onto the block.
 *
 * @param x_range Horizontal pixel range where the sprite overlaps with the current block.
 *                This range is given in global screen coordinates.
 * @param y_range Vertical pixel range where the sprite overlaps with the current block.
 *                This range is given in global screen coordinates.
 * @param idx Index of the sprite in the `term_sprites` array to be drawn.
 *
 * @note This function assumes that the sprite's pixel data is stored in a 2-bit per pixel format,
 *       and that each byte contains 4 pixels. It also assumes that the sprite data is tightly packed
 *       row-wise, and that the sprite's pallet contains 4 colors.
 *
 * @details
 * The function calculates the relevant portion of the sprite to be drawn by iterating over the
 * overlapping area between the sprite and the block. For each pixel, it extracts the 2-bit value
 * from the sprite's data array, applies the appropriate color from the sprite's pallet, and writes
 * it to the character buffer. Pixels are mapped to the 8x8 character block using modulo arithmetic
 * to ensure correct placement. This description was written using GPT4o. I hope it helps :)
 */
static void tGFX_DrawSprite(term_range_t x_range, term_range_t y_range, uint8_t idx)
{
    volatile uint8_t bytes_len = term_sprites[idx]->size_x / 4;
    uint8_t *data = term_sprites[idx]->data;

    // Adjust byte length if size_x is not divisible by 4
    if (term_sprites[idx]->size_x % 4) bytes_len++;

    // Iterate over the sprite's pixel data within the specified range
    for (size_t i = y_range.start - term_sprites[idx]->y; i < y_range.end - term_sprites[idx]->y; i++)
    {
        for (size_t j = x_range.start - term_sprites[idx]->x; j < x_range.end - term_sprites[idx]->x; j++)
        {
            /* Extract the 2-bit pixel value from the sprite's data array
            *
            * - The expression `j / 4` computes the byte index within the current row, since each byte contains 4 pixels.
            * - `i * bytes_len` computes the row offset within the data array, which is multiplied by `bytes_len` to access
            *   the correct row.
            * - `6 - ((j % 4) * 2)` calculates the correct shift value to align the pixel's 2-bit value to the least
            *   significant bits. This is because the most significant bits in the byte correspond to the first pixel.
            * - The bitmask `& 0b11` ensures that only the least significant 2 bits (the desired pixel value) are returned.
            *
            * Example:
            * - For `j = 5` (the 6th pixel in the row):
            *   - `j / 4 = 1` (second byte in the row),
            *   - `j % 4 = 1` (second pixel in the byte),
            *   - The shift is `6 - (1 * 2) = 4`, meaning the pixel's value is in bits 5 and 4 of the byte.
            */
            uint8_t pixel = (data[j / 4 + i * bytes_len]) >> (6 - ((j % 4) * 2)) & 0b11;

            // If the pixel is non-transparent, map it to the 8x8 character block
            if (pixel)
            {
                term_character_buf[term_current_buf][((j + term_sprites[idx]->x) % 8) + ((i + term_sprites[idx]->y) % 8) * TERM_BLOCK_SIZE] = color_pallets[term_sprites[idx]->pallet][pixel];
            }
        }
    }
}


static uint8_t tGFX_DrawSpriteOnBlock(uint8_t block_x, uint8_t block_y)
{
	term_range_t block_x_range = {block_x * 8, (block_x * 8) + 8};
	term_range_t block_y_range = {block_y * 8, (block_y * 8) + 8};

	term_range_t sprite_x_range, sprite_y_range, common_x_range, common_y_range;

	uint8_t ret = 1;


	for(size_t i = 0; i < sprites_count; i++)
	{
		if(term_sprites[i] != NULL)
		{
			sprite_x_range.start = term_sprites[i]->x;
			sprite_x_range.end = term_sprites[i]->x + term_sprites[i]->size_x;

			sprite_y_range.start = term_sprites[i]->y;
			sprite_y_range.end = term_sprites[i]->y + term_sprites[i]->size_y;

			common_x_range = tGFX_CheckCommonPartOfRange(block_x_range, sprite_x_range);

			if(common_x_range.start != 0xFF)
			{
				common_y_range = tGFX_CheckCommonPartOfRange(block_y_range, sprite_y_range);

				if(common_y_range.start != 0xFF)
				{

					tGFX_DrawSprite(common_x_range, common_y_range, i);
					ret = 0;

				}

			}
		}
	}

	return ret;
}


void tGFX_SetChar(uint8_t character, uint8_t x, uint8_t y, uint8_t c_fg, uint8_t c_bg)
{
    term_buf[x][y].character = character;
    term_buf[x][y].c_fg = c_fg;
    term_buf[x][y].c_bg = c_bg;

    term_update_map[y] = term_update_map[y] | (1 << x);
}


void tGFX_SetCursor(uint8_t x, uint8_t y)
{
    term_x_pos = x;
    term_y_pos = y;
}


void tGFX_Print(char *txt, uint8_t color_foreground, uint8_t color_background)
{

    uint16_t print_len = strlen(txt);

    for (size_t i = 0; i < print_len; i++)
    {
        if (term_x_pos > TERM_SIZE_X)
        {
            term_x_pos = 0;
            term_y_pos++;
        }
        if (term_y_pos > TERM_SIZE_Y)
        {
            term_y_pos = 0;
        }

        tGFX_SetChar(txt[i], term_x_pos, term_y_pos, color_foreground, color_background);

        term_x_pos++;
    }
}


void tGFX_Clear(uint8_t color)
{
    for(uint32_t i = 0; i < TERM_PX_SIZE_X/TERM_BLOCK_SIZE; i++)
     {
        for(uint32_t j = 0; j < TERM_PX_SIZE_Y/TERM_BLOCK_SIZE; j++)
        {
            tGFX_SetChar(color, i, j, 0, color);
        }

     }
}

void tGFX_Update(void)
{
    for (size_t i = 0; i < TERM_SIZE_X; i++)
    {
        for (size_t j = 0; j < TERM_SIZE_Y; j++)
        {
        	if((term_update_map[j] >> i) & 1)
        	{
        		term_update_map[j] &= ~(1 << i);

                tGFX_CharToBuf(&term_buf[i][j]);

                if((term_sprite_map[j] >> i) & 1)
                {
                    // Set this block for update again. This will result in automatic clear up if sprite is deleted/move out of screen bounds.
                	term_update_map[j] = term_update_map[j] | (1 << i);

                	if(tGFX_DrawSpriteOnBlock(i, j))
                	{
                		// Sprite is NULL/NOT PRESENT SO DELETE IT FROM MAP AND UPDATE THAT BLOCK
                		term_sprite_map[j] &= ~(1 << i);
                	}
                }

                tGFX_LCD_DMA_Wait();
                tGFX_LCD_SetWindow((i * TERM_BLOCK_SIZE) + TERM_PX_OFFSET_X, (j * TERM_BLOCK_SIZE) + TERM_PX_OFFSET_Y, 8, 8);
                tGFX_LCD_DMA_SendData(&term_character_buf[term_current_buf][0], TERM_BLOCK_SIZE*TERM_BLOCK_SIZE);
                term_current_buf = !term_current_buf;
        	}
        }
    }
}

void tGFX_SingleUpdate(uint8_t x, uint8_t y)
{
    tGFX_CharToBuf(&term_buf[x][y]);
    tGFX_LCD_SetWindow((x * TERM_BLOCK_SIZE) + TERM_PX_OFFSET_X, (y * TERM_BLOCK_SIZE) + TERM_PX_OFFSET_Y, 8, 8);
    tGFX_LCD_SendData(&term_character_buf[term_current_buf][0], TERM_BLOCK_SIZE*TERM_BLOCK_SIZE);
}

void tGFX_SetSprite(term_sprite_t *sprite, uint8_t idx)
{
	int16_t sprite_start_x = (sprite->x/8);
	int16_t sprite_stop_x = (sprite->x + sprite->size_x)/8;

	int16_t sprite_start_y = (sprite->y/8);
	int16_t sprite_stop_y = (sprite->y + sprite->size_y)/8;

	term_sprites[idx] = sprite;

	if(sprite->x % 8) sprite_stop_x++;
	if(sprite->y % 8) sprite_stop_y++;

	if(sprite_stop_y > TERM_SIZE_Y - 1) sprite_stop_y = TERM_SIZE_Y - 1;
	if(sprite_stop_x > TERM_SIZE_X - 1) sprite_stop_x = TERM_SIZE_X - 1;

	if(sprite_start_x < 0) sprite_start_x = 0;
	if(sprite_stop_x < 0) sprite_stop_x = -1;

    if(sprite_start_y < 0) sprite_start_y = 0;
    if(sprite_stop_y < 0) sprite_stop_y = -1;


	for(size_t i = sprite_start_y; i < sprite_stop_y + 1; i++)
	{
		for(size_t j = sprite_start_x; j < sprite_stop_x + 1; j++)
		{
			term_sprite_map[i] = term_sprite_map[i] | (1 << j);
			term_update_map[i] = term_update_map[i] | (1 << j);
		}

	}

}

void tGFX_ClearSprite(uint8_t idx)
{
    term_sprites[idx] = NULL;
}

void tGFX_SetSpritesCount(uint8_t count)
{
    if(count > TERM_SPRITES_COUNT) sprites_count = TERM_SPRITES_COUNT;
    sprites_count = count;
}
