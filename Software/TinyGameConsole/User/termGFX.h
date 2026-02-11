/*
 *	Terminal Graphics
 *  DMA Sprite Version for CH32V003 V1.1
 */

#ifndef _TERMGFX_H_
#define _TERMGFX_H_

/* ######## CONFIG ######## */

// All X/Y values must were divisible by 8
#define TERM_PX_SIZE_X 160
#define TERM_PX_SIZE_Y 80


#define TERM_PX_OFFSET_X 0
#define TERM_PX_OFFSET_Y 0

#define TERM_SPRITES_COUNT 16

/* ######################## */

#define TERM_BLOCK_SIZE 8

#define TERM_SIZE_X TERM_PX_SIZE_X/TERM_BLOCK_SIZE
#define TERM_SIZE_Y TERM_PX_SIZE_Y/TERM_BLOCK_SIZE


typedef struct
{
	// Char bitmap LUTs index
	uint8_t character;
	// Bitmap background color
	uint8_t c_bg:4;
	// Bitmap foreground color
	uint8_t c_fg:4;

}term_char_t;


typedef struct
{
	int16_t x;
	int16_t y;

	uint8_t size_x;
	uint8_t size_y;

	uint8_t *data;
	uint8_t pallet;

}term_sprite_t;

typedef struct
{
    uint8_t x;
    uint8_t y;

    uint8_t size_x;
    uint8_t size_y;

    uint8_t *data;
    uint8_t pallet;

}term_sprite_old_t;

typedef struct
{
	int16_t start;
	int16_t end;

}term_range_t;

extern const uint8_t term_char_map[][8];

void tGFX_SetCursor(uint8_t x, uint8_t y);
void tGFX_Print(char *txt, uint8_t color_foreground, uint8_t color_background);
void tGFX_SetChar(uint8_t character, uint8_t x, uint8_t y, uint8_t c_fg, uint8_t c_bg);
void tGFX_Clear(uint8_t color);
void tGFX_Update(void);
void tGFX_SingleUpdate(uint8_t x, uint8_t y);
void tGFX_ClearBuf(void);
void tGFX_SetSprite(term_sprite_t *sprite, uint8_t idx);
void tGFX_ClearSprite(uint8_t idx);
void tGFX_SetSpritesCount(uint8_t count);

#endif /* _TERMGFX_H_ */
