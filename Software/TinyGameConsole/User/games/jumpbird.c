/*
 * jumpbird.c
 *
 *  Created on: Aug 26, 2023
 *      Author: Maksym
 */

#include <string.h>
#include <stdlib.h>

#include "debug.h"
#include "ch32v00x.h"
#include "termGFX.h"
#include "st7735.h"
#include "buttons.h"
#include "tick.h"
#include "sound.h"
#include "engine.h"
#include "menu.h"
#include "gameover.h"
#include "rng.h"
#include "highscore.h"

#include "jumpbird.h"

static const uint16_t jumpbird_game_over_sound_seq[] = {200, 10, 400, 10, 200, 10, 400, 10, 150, 40};

static const uint8_t pipe_bitmap[] =
{
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,
        0b11101010, 0b10100101, 0b01110000,

};

static const uint8_t pipe_cap_bitmap[]=
{
        0b11111111, 0b11111111, 0b11111111, 0b11110000,
        0b11101010, 0b10101010, 0b10010101, 0b01110000,
        0b11101010, 0b10101010, 0b10010101, 0b01110000,
        0b11101010, 0b10101010, 0b10010101, 0b01110000,
        0b11111111, 0b11111111, 0b11111111, 0b11110000,
};

static const uint8_t bird_bitmap[] =
{

        0b00000000, 0b00000000, 0b00000000,
        0b00001010, 0b10101000, 0b00000000,
        0b00100110, 0b11111110, 0b00000000,
        0b10010110, 0b11101011, 0b10000000,
        0b10010110, 0b11101011, 0b10100000,
        0b10010101, 0b10111111, 0b10101000,
        0b10010101, 0b01101010, 0b01101010,
        0b00100101, 0b01010101, 0b01100000,
        0b00001001, 0b01010101, 0b10000000,
        0b00000010, 0b10101010, 0b00000000,
        0b00000000, 0b00000000, 0b00000000,
        0b00000000, 0b00000000, 0b00000000,
};



static jump_bird_game_state_t *gs;


void JUMPBIRD_SetupSprites(void)
{
    for(size_t i = 0; i < 3; i++)
    {
        gs->pipes_upper[i].size_x = 10;
        gs->pipes_upper[i].y = 0;
        gs->pipes_upper[i].pallet = 1;
        gs->pipes_upper[i].data = (uint8_t*)pipe_bitmap;

        gs->pipes_upper_cap[i].size_x = 14;
        gs->pipes_upper_cap[i].size_y = 5;
        gs->pipes_upper_cap[i].pallet = 1;
        gs->pipes_upper_cap[i].data = (uint8_t*)pipe_cap_bitmap;

        gs->pipes_lower[i].size_x = 10;
        gs->pipes_lower[i].pallet = 1;
        gs->pipes_lower[i].data = (uint8_t*)pipe_bitmap;

        gs->pipes_lower_cap[i].size_x = 14;
        gs->pipes_lower_cap[i].size_y = 5;
        gs->pipes_lower_cap[i].pallet = 1;
        gs->pipes_lower_cap[i].data = (uint8_t*)pipe_cap_bitmap;
    }

    gs->bird.data = (uint8_t*)bird_bitmap;
    gs->bird.pallet = 2;
    gs->bird.size_x = 12;
    gs->bird.size_y = 12;
    gs->bird.x = 79;
    gs->bird.y = 39;

}

void JUMPBIRD_DrawPipe(int16_t x, uint8_t gap, int8_t gap_pos, uint8_t idx)
{
    uint8_t upper_pipe_len, lower_pipe_len;

    upper_pipe_len = ((70 - gap) / 2) - gap_pos;
    lower_pipe_len = ((70 - gap) / 2) + gap_pos;


    gs->pipes_upper[idx].size_y = upper_pipe_len;
    gs->pipes_upper[idx].x = x;

    gs->pipes_upper_cap[idx].x = x - 2;
    gs->pipes_upper_cap[idx].y = upper_pipe_len;

    gs->pipes_lower[idx].size_y = lower_pipe_len;
    gs->pipes_lower[idx].x = x;
    gs->pipes_lower[idx].y = upper_pipe_len + gap + 10;

    gs->pipes_lower_cap[idx].x = x - 2;
    gs->pipes_lower_cap[idx].y = upper_pipe_len + gap + 5;

    switch (idx)
    {
        case 0:
            tGFX_SetSprite(&(gs->pipes_upper[idx]), 0);
            tGFX_SetSprite(&(gs->pipes_upper_cap[idx]), 1);
            tGFX_SetSprite(&(gs->pipes_lower[idx]), 2);
            tGFX_SetSprite(&(gs->pipes_lower_cap[idx]), 3);
            break;

        case 1:
            tGFX_SetSprite(&(gs->pipes_upper[idx]), 4);
            tGFX_SetSprite(&(gs->pipes_upper_cap[idx]), 5);
            tGFX_SetSprite(&(gs->pipes_lower[idx]), 6);
            tGFX_SetSprite(&(gs->pipes_lower_cap[idx]), 7);
            break;

        case 2:
            tGFX_SetSprite(&(gs->pipes_upper[idx]), 8);
            tGFX_SetSprite(&(gs->pipes_upper_cap[idx]), 9);
            tGFX_SetSprite(&(gs->pipes_lower[idx]), 10);
            tGFX_SetSprite(&(gs->pipes_lower_cap[idx]), 11);
            break;
        default:
            break;
    }
}



void JUMPBIRD_ProcessGravity(void)
{
    gs->bird_acc += JUMPBIRD_GRAVITY;
    gs->bird.y += gs->bird_acc / 4;
}

void JUMPBIRD_ProcessCollision(void)
{

    uint16_t distance_1 = gs->pipe_pos[1] - 79 + 11; // Bird pos + offset
    uint16_t distance_2 = gs->pipe_pos[2] - 79 + 11;

    uint8_t nearest_pipe = (distance_1 < distance_2) ? 1 : 2;

    if ((gs->bird.x + 11) > gs->pipes_upper[nearest_pipe].x && gs->bird.y < (gs->pipes_upper[nearest_pipe].y + gs->pipes_upper[nearest_pipe].size_y))
    {
        gs->collision = 1;
    }
    else if((gs->bird.x + 11) > gs->pipes_upper_cap[nearest_pipe].x && gs->bird.y < (gs->pipes_upper_cap[nearest_pipe].y + gs->pipes_upper_cap[nearest_pipe].size_y))
    {
        gs->collision = 1;
    }
    else if((gs->bird.x + 11) > gs->pipes_lower[nearest_pipe].x && (gs->bird.y + 11) > gs->pipes_lower[nearest_pipe].y)
    {
        gs->collision = 1;
    }
    else if((gs->bird.x + 11) > gs->pipes_lower_cap[nearest_pipe].x && (gs->bird.y + 11)  > gs->pipes_lower_cap[nearest_pipe].y)
    {
        gs->collision = 1;
    }
    else if((gs->bird.y + 11) > 80 || gs->bird.y < 0)
    {
        gs->collision = 1;
    }

}


void JUMPBIRD_Init(void)
{


    ENG_ClearCommonBuf();

    gs = ENG_CommonBufAlloc(sizeof(jump_bird_game_state_t));

    tGFX_SetSpritesCount(16);

    JUMPBIRD_SetupSprites();
    tGFX_SetSprite(&(gs->bird), 12);

    // Clear background
    tGFX_Clear(14);


    gs->pipe_pos[0] = -19;
    gs->pipe_pos[1] = 61;
    gs->pipe_pos[2] = 141;

    gs->pipe_gap_pos[0] = -17 + (RNG_Get() % 34);
    gs->pipe_gap_pos[1] = 0;
    gs->pipe_gap_pos[2] = -17 + (RNG_Get() % 34);


    JUMPBIRD_DrawPipe(gs->pipe_pos[0], 36, gs->pipe_gap_pos[0], 0);
    JUMPBIRD_DrawPipe(gs->pipe_pos[1], 36, gs->pipe_gap_pos[1], 1);
    JUMPBIRD_DrawPipe(gs->pipe_pos[2] , 36, gs->pipe_gap_pos[2], 2);

    gs->collision = 0;
    gs->score = 0;

    ENG_UpdateInFrameCallback(&JUMPBIRD_Main);

}

void JUMPBIRD_PrintScore(void)
{
    char int_buf[4];

    tGFX_SetCursor(9, 1);
    tGFX_Print("000", 0, 14);
    if(gs->score < 10)
    {
        tGFX_SetCursor(11, 1);
    }
    else if(gs->score < 100)
    {
        tGFX_SetCursor(10, 1);
    }
    else
    {
        tGFX_SetCursor(9, 1);
    }
    itoa(gs->score, int_buf, 10);
    tGFX_Print(int_buf, 0, 14);

}



void JUMPBIRD_Main(void)
{

    // Process input
    if(!BTN_IsPressed(BTN_ACTION))
    {
        gs->released = 1;
    }

    if(BTN_IsPressed(BTN_ACTION) && gs->released)
    {
        gs->bird_acc = -18;
        gs->released = 0;
        SND_PlayNow(JUMPBIRD_JUMP_FREQ, 1);
    }

    // Process physics
    JUMPBIRD_ProcessGravity();
    JUMPBIRD_ProcessCollision();


    // Process graphics
    for(uint8_t i = 0; i < 3; i++)
    {
        gs->pipe_pos[i]--;
    }


    if(gs->pipe_pos[0] <= -79)
    {
        gs->pipe_pos[0] = 1;
        gs->pipe_pos[1] = 81;
        gs->pipe_pos[2] = 161;

        gs->pipe_gap_pos[0] = gs->pipe_gap_pos[1];
        gs->pipe_gap_pos[1] = gs->pipe_gap_pos[2];

        int random = -17 + (RNG_Get() % 34);
        gs->pipe_gap_pos[2] = random;

        SND_PlayNow(JUMPBIRD_POINT_FREQ, 3);
        gs->score++;
    }

    tGFX_SetSprite(&(gs->bird), 12);

    JUMPBIRD_DrawPipe(gs->pipe_pos[0], 36, gs->pipe_gap_pos[0], 0);
    JUMPBIRD_DrawPipe(gs->pipe_pos[1], 36, gs->pipe_gap_pos[1], 1);
    JUMPBIRD_DrawPipe(gs->pipe_pos[2] , 36, gs->pipe_gap_pos[2], 2);

    JUMPBIRD_PrintScore();

    // Game Over
    if(gs->collision)
    {
        uint32_t highscore = HIGHSCORE_Read(HIGHSCORE_JUMPBIRD);

        SND_PlaySequence((uint16_t*)jumpbird_game_over_sound_seq, 4);

        for(uint8_t i = 0; i < 16; i++)
        {
            tGFX_ClearSprite(i);
        }

        // Update highscore
        if(highscore < gs->score)
        {
            HIGHSCORE_Write(HIGHSCORE_JUMPBIRD, gs->score);
            highscore = gs->score;
        }

        GAMEOVER_Setup(GAMEOVER_TYPE_SCORE, gs->score, highscore, BTN_UP, NULL);
        GAMEOVER_Show();
    }

}
