/*
 * pong.c
 *
 *  Created on: Jul 14, 2023
 *      Author: max
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

#include "pong.h"

static const uint8_t paddle_bitmap[] =
{
		0b01010101,
		0b01010101,
		0b01010101,
		0b01010101,
		0b01010101,
		0b01010101,
		0b01010101,
		0b01010101,
		0b01010101,
		0b01010101,
		0b01010101,
		0b01010101,
		0b01010101,
		0b01010101,
		0b01010101,
		0b01010101,
		0b01010101,
		0b01010101,
		0b01010101,
		0b01010101,
};

static const uint8_t ball_bitmap[] =
{
		0b01010101, 0b01000000,
		0b01010101, 0b01000000,
		0b01010101, 0b01000000,
		0b01010101, 0b01000000,
		0b01010101, 0b01000000,
};

static const int8_t pong_move_lut[4][4][2] =
{
	{// DIR RIGHT UP

		{// ANGLE 0
				4, 0
		},
		{// ANGLE 30
				5, -3
		},
		{// ANGLE 45
				4, -4
		},
		{// ANGLE 60
				3, -5
		}

	},
	{// DIR RIGHT DOWN

		{// ANGLE 0
				4, 0
		},
		{// ANGLE 30
				5, 3
		},
		{// ANGLE 45
				4, 4
		},
		{// ANGLE 60
				3, 5
		}

	},
	{// DIR LEFT UP

		{// ANGLE 0
				-4, 0
		},
		{// ANGLE 30
				-5, -3
		},
		{// ANGLE 45
				-4, -4
		},
		{// ANGLE 60
				-3, -5
		}

	},
	{// DIR LEFT DOWN

		{// ANGLE 0
				-4, 0
		},
		{// ANGLE 30
				-5, 3
		},
		{// ANGLE 45
				-4, 4
		},
		{// ANGLE 60
				-3, 5
		}

	}
};

static pong_game_state_t *gs;

static const char* pong_gameover_msg[] = {"PLAYER WON", "AI WON"};

void PONG_PrintScore(void)
{
	tGFX_SetChar(PONG_U_SEG_0 + gs->p_score, 4, 1, 15, 0);
	tGFX_SetChar(PONG_D_SEG_0 + gs->p_score, 4, 2, 15, 0);

	tGFX_SetChar(PONG_U_SEG_0 + gs->e_score, 15, 1, 15, 0);
	tGFX_SetChar(PONG_D_SEG_0 + gs->e_score, 15, 2, 15, 0);

}

void PONG_BallBounce(term_sprite_t *paddle)
{
    // Ball is for sure on enemy side
    if(gs->ball.x > 80)
    {
        // Align with paddle
        gs->ball.x = 149;

        if(gs->ball_dir == PONG_BALL_DIR_RIGHT_DOWN) gs->ball_dir = PONG_BALL_DIR_LEFT_DOWN;
        else gs->ball_dir = PONG_BALL_DIR_LEFT_UP;
    }
    // Ball is for sure on player side
    else
    {
        // Align with paddle
        gs->ball.x = 4;

        if(gs->ball_dir == PONG_BALL_DIR_LEFT_DOWN) gs->ball_dir = PONG_BALL_DIR_RIGHT_DOWN;
        else gs->ball_dir = PONG_BALL_DIR_RIGHT_UP;
    }

    // Corner
    if((gs->ball.y >= paddle->y - 4 &&  gs->ball.y <= paddle->y) || (gs->ball.y >= paddle->y + 20 &&  gs->ball.y <= paddle->y + 24))
    {
        gs->ball_angle = PONG_BALL_ANGLE_60;
    }// Edge
    else if((gs->ball.y >= paddle->y &&  gs->ball.y <= paddle->y + 6) || (gs->ball.y >= paddle->y + 14 && gs->ball.y <= paddle->y + 20))
    {
        gs->ball_angle = PONG_BALL_ANGLE_45;
    }
    else
    {
        gs->ball_angle = PONG_BALL_ANGLE_30;
    }

    SND_PlayNow(PONG_COLL_SOUND_PADDLE_FREQ, 1);
}


uint8_t PONG_BallCollision(void)
{
    if(gs->ball.x > 158)
    {
        gs->p_score++;

        // Move ball to the middle
        gs->ball.y = RNG_GetRange(20, 60);
        gs->ball.x = 78;
        gs->ball_dir = RNG_GetRange(PONG_BALL_DIR_LEFT_UP, PONG_BALL_DIR_LEFT_DOWN);
        gs->ball_angle = RNG_GetRange(0, 3);

        SND_PlayNow(PONG_SOUND_LOSS_POINT_FREQ, 2);
        return 1;
    }
    else if(gs->ball.x < 2)
    {
        gs->e_score++;

        // Move ball to the middle
        gs->ball.y = RNG_GetRange(20, 60);
        gs->ball.x = 78;
        gs->ball_dir = RNG_GetRange(PONG_BALL_DIR_RIGHT_UP, PONG_BALL_DIR_RIGHT_DOWN);
        gs->ball_angle = RNG_GetRange(0, 3);

        SND_PlayNow(PONG_SOUND_LOSS_POINT_FREQ, 2);
        return 1;
    }

    // Paddle Collision
    if(gs->ball.y >= gs->paddle_p.y - 4 && gs->ball.y <= gs->paddle_p.y + 20 && gs->ball.x <= 4)
    {
        PONG_BallBounce(&gs->paddle_p);
        return 1;
    }

    if(gs->ball.y >= gs->paddle_e.y - 4 && gs->ball.y <= gs->paddle_e.y + 20 && gs->ball.x >= 151)
    {
        PONG_BallBounce(&gs->paddle_e);
        return 1;
    }


    // World Collision
    if(gs->ball.y < 0)
    {
        gs->ball.y = 0;
        SND_PlayNow(PONG_COLL_SOUND_ENV_FREQ, 1);

        if(gs->ball_dir == PONG_BALL_DIR_RIGHT_UP) gs->ball_dir = PONG_BALL_DIR_RIGHT_DOWN;
        else if(gs->ball_dir == PONG_BALL_DIR_LEFT_UP) gs->ball_dir = PONG_BALL_DIR_LEFT_DOWN;
        return 1;
    }
    else if(gs->ball.y > 74)
    {

        gs->ball.y = 74;
        SND_PlayNow(PONG_COLL_SOUND_ENV_FREQ, 1);

        if(gs->ball_dir == PONG_BALL_DIR_RIGHT_DOWN) gs->ball_dir = PONG_BALL_DIR_RIGHT_UP;
        else if(gs->ball_dir == PONG_BALL_DIR_LEFT_DOWN) gs->ball_dir = PONG_BALL_DIR_LEFT_UP;
        return 1;
    }

    return 0;

}


uint8_t _PONG_EnemyAI_UpdateTarget(uint8_t rng, uint8_t target_pos)
{
    int8_t error;
    int8_t new_target_pos;

    if(rng == 0)
    {
        error = 0;
    }
    else if(rng > 0 && rng < 7)
    {
        error = -4 + (RNG_Get() % 8);
    }
    else if(rng >= 7 && rng < 9)
    {
        error = -6 + (RNG_Get() % 12);
    }
    else
    {
        error = -12 + (RNG_Get() % 24);
    }

    new_target_pos = (gs->ball.y - 7) + error;
    if(new_target_pos < 0) new_target_pos = 0;

    // Prevent very small movements to avoid paddle shake
    if(abs(target_pos - new_target_pos) > 3) return new_target_pos;

    return target_pos;
}

void PONG_EnemyAI(void)
{

    uint8_t active_treshold = RNG_GetRange(60, 80);
    uint8_t rng = RNG_GetRange(0, 10);
    static uint8_t target_pos;

    if(gs->ball.x > active_treshold)
    {
        // Always update target
        target_pos = _PONG_EnemyAI_UpdateTarget(rng, target_pos);
    }
    else
    {
        // Low chance to update target
        if(rng < 2)
        {
            target_pos = _PONG_EnemyAI_UpdateTarget(rng, target_pos);
        }
    }

    if(abs(gs->paddle_e.y - target_pos) > 1 && gs->paddle_e.y > target_pos)
    {
        gs->paddle_e.y -= 2;
    }
    else if(abs(gs->paddle_e.y - target_pos) > 1 && gs->paddle_e.y < target_pos)
    {
        gs->paddle_e.y += 2;
    }

    if(gs->paddle_e.y < 0) gs->paddle_e.y = 0;
    if(gs->paddle_e.y > 60) gs->paddle_e.y = 60;

}

void PONG_BallPhysics(void)
{
    int16_t x_target = gs->ball.x + pong_move_lut[gs->ball_dir][gs->ball_angle][0];
    int16_t y_target = gs->ball.y + pong_move_lut[gs->ball_dir][gs->ball_angle][1];


    // Move ball one by one pixel to avoid it slipping through paddle or bouncing off empty wall
    do
    {
        if(x_target != gs->ball.x)
        {
            if (pong_move_lut[gs->ball_dir][gs->ball_angle][0] > 0) gs->ball.x += 1;
            else if (pong_move_lut[gs->ball_dir][gs->ball_angle][0] < 0) gs->ball.x -= 1;
        }

        if(y_target != gs->ball.y)
        {
            if (pong_move_lut[gs->ball_dir][gs->ball_angle][1] > 0) gs->ball.y += 1;
            else if (pong_move_lut[gs->ball_dir][gs->ball_angle][1] < 0) gs->ball.y -= 1;
        }

        if(PONG_BallCollision()) break;

    }while(x_target != gs->ball.x || y_target != gs->ball.y);
}


void PONG_Init(void)
{
    // Setup RAM
    ENG_ClearCommonBuf();
    gs = ENG_CommonBufAlloc(sizeof(pong_game_state_t));

    // Clear background
    tGFX_Clear(0);

    // Load and setup sprites
    tGFX_SetSpritesCount(3);

    gs->paddle_p.size_x = 2;
    gs->paddle_p.size_y = 20;
    gs->paddle_p.x = 2;
    gs->paddle_p.y = 32;
    gs->paddle_p.data = (uint8_t*)paddle_bitmap;

    gs->paddle_e.size_x = 2;
    gs->paddle_e.size_y = 20;
    gs->paddle_e.x = 156;
    gs->paddle_e.y = 32;
    gs->paddle_e.data = (uint8_t*)paddle_bitmap;

    gs->ball.size_x = 5;
    gs->ball.size_y = 5;
    gs->ball.x = 78;
    gs->ball.data = (uint8_t*)ball_bitmap;

    // Set up game
    gs->p_score = 0;
    gs->e_score = 0;

    uint32_t tick = TICK_Get();
    RNG_SetSeed(tick);

    gs->ball_dir = RNG_Get() % 4;
    gs->ball_angle = RNG_Get() % 4;
    gs->ball.y = 38;

 	for(uint32_t j = 0; j < 10; j++)
 	{
 		tGFX_SetChar(PONG_LINE_A, 9, j, 15, 0);
 		tGFX_SetChar(PONG_LINE_B, 10, j, 15, 0);
 	}

    tGFX_SetSprite(&(gs->ball), 0);
    tGFX_SetSprite(&(gs->paddle_p), 1);
    tGFX_SetSprite(&(gs->paddle_e), 2);

    ENG_UpdateInFrameCallback(&PONG_Main);
}


void PONG_Main(void)
{

    // Process input
	if(BTN_IsPressed(BTN_UP) && gs->paddle_p.y > 1) gs->paddle_p.y -= 2;
	else if(BTN_IsPressed(BTN_DOWN) && gs->paddle_p.y < 59)gs->paddle_p.y += 2;

    // Force game over
    if(BTN_IsPressed(BTN_LEFT) && BTN_IsPressed(BTN_RIGHT))
    {
         gs->e_score = 10;
    }

    PONG_EnemyAI();

    PONG_BallPhysics();

	PONG_PrintScore();

    tGFX_SetSprite(&(gs->ball), 0);
    tGFX_SetSprite(&(gs->paddle_p), 1);
    tGFX_SetSprite(&(gs->paddle_e), 2);

    // Game Over
    if(gs->e_score > 9 || gs->p_score > 9)
    {
        uint8_t str_idx;
        uint32_t wins;

        tGFX_ClearSprite(0);
        tGFX_ClearSprite(1);
        tGFX_ClearSprite(2);

        if(gs->e_score > 9)
        {
            // AI wins
            str_idx = 1;
            wins = HIGHSCORE_Read(HIGHSCORE_PONG_E_WINS) + 1;
            HIGHSCORE_Write(HIGHSCORE_PONG_E_WINS, wins);
        }
        else
        {
            // Player wins
            str_idx = 0;
            wins = HIGHSCORE_Read(HIGHSCORE_PONG_P_WINS) + 1;
            HIGHSCORE_Write(HIGHSCORE_PONG_P_WINS, wins);
        }

        GAMEOVER_Setup(GAMEOVER_TYPE_MESSAGE, 0, 0, BTN_ACTION, pong_gameover_msg[str_idx]);
        GAMEOVER_Show();
    }
}

