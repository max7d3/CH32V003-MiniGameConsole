/*
 * snake.c
 *
 *  Created on: Jul 11, 2023
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

#include "snake.h"


static snake_game_state_t *game_state;


static const uint16_t game_oversound_seq[] = {880, 20, 660, 20, 440, 20, 330, 20, 220, 20};

void SNAKE_SpawnFood(void)
{
	uint8_t fail = 0;

	do
	{
		fail = 0;
		game_state->food_x = RNG_Get() % 19;
		game_state->food_y = (RNG_Get() % 8) + 1;

		for(size_t i = 0; i < game_state->tail_len*2; i += 2)
		{

			if(game_state->snake_segments[i] == game_state->food_x && game_state->snake_segments[i+1] == game_state->food_y)
			{
				fail = 1;
				break;
			}

		}

	}while(fail);

	tGFX_SetChar(0, game_state->food_x, game_state->food_y, 9, 9);

}



void SNAKE_GameOver(void)
{
    uint32_t highscore = HIGHSCORE_Read(HIGHSCORE_SNAKE);

	SND_PlaySequence((uint16_t*)game_oversound_seq, 4);
	game_state->game_over = 1;

	// Update highscore
	if(highscore < game_state->score)
	{
	    HIGHSCORE_Write(HIGHSCORE_SNAKE, game_state->score);
	    highscore = game_state->score;
	}


    GAMEOVER_Setup(GAMEOVER_TYPE_SCORE, game_state->score, highscore, BTN_ACTION, NULL);
    GAMEOVER_Show();
}




uint8_t SNAKE_CheckCollision(void)
{
	for(size_t i = 2; i < game_state->tail_len*2; i += 2)
	{

		if(game_state->snake_segments[i] == *(game_state->snake_pos_x) && game_state->snake_segments[i+1] == *(game_state->snake_pos_y))
		{
			if(game_state->snake_segments[game_state->tail_len*2 - 2] == *(game_state->snake_pos_x) &&  game_state->snake_segments[game_state->tail_len*2 - 1] == *(game_state->snake_pos_y))
			{
				return 0;
			}

			SNAKE_GameOver();


			return 1;
		}

		if(*(game_state->snake_pos_x) > 19 || *(game_state->snake_pos_y) > 9 || *(game_state->snake_pos_y) == 0 )
		{
			SNAKE_GameOver();

			return 1;
		}

	}

	return 0;
}

void SNAKE_PrintScore(void)
{
	char score_buf[16];

	for(size_t i = 0; i < 20; i++)
	{
		tGFX_SetChar(132, i, 0, 7, 0);
	}

	tGFX_SetCursor(0, 0);
	tGFX_Print("SCORE:", 15, 0);

	itoa(game_state->score, score_buf, 10);

	tGFX_Print(score_buf, 15, 0);

}





void SNAKE_Init(void)
{
    tGFX_Clear(0);

    ENG_ClearCommonBuf();

    game_state = ENG_CommonBufAlloc(sizeof(snake_game_state_t));

    game_state->snake_pos_x = &(game_state->snake_segments[0]);
    game_state->snake_pos_y = &(game_state->snake_segments[1]);

    uint32_t tick = TICK_Get();
    RNG_SetSeed(tick);
    *(game_state->snake_pos_x) = 5 + (RNG_Get() % 10);
    *(game_state->snake_pos_y) = 3 + (RNG_Get() % 4);

    game_state->game_over = 0;
    game_state->score = 0;
    game_state->frames_per_move = 15;
    game_state->frames_since_move = 0;
    game_state->tail_len = 2;
    game_state->snake_dir = (RNG_Get() % 3) + 129;
    game_state->snake_dir_lock = game_state->snake_dir;

    SNAKE_PrintScore();
    SNAKE_SpawnFood();
    ENG_UpdateInFrameCallback(&SNAKE_Main);
}



void SNAKE_Main(void)
{
    game_state->frames_since_move++;

	if(BTN_IsPressed(BTN_UP) && game_state->snake_dir_lock != 131) game_state->snake_dir = 129;
	else if(BTN_IsPressed(BTN_RIGHT) && game_state->snake_dir_lock != 128) game_state->snake_dir = 130;
	else if(BTN_IsPressed(BTN_DOWN) && game_state->snake_dir_lock != 129) game_state->snake_dir = 131;
	else if(BTN_IsPressed(BTN_LEFT) && game_state->snake_dir_lock != 130) game_state->snake_dir = 128;

	if(*(game_state->snake_pos_x) == game_state->food_x &&  *(game_state->snake_pos_y) == game_state->food_y)
	{
	    game_state->snake_segments[game_state->tail_len*2] = *(game_state->snake_pos_x);
	    game_state->snake_segments[game_state->tail_len*2 + 1] = *(game_state->snake_pos_y);
	    game_state->tail_len++;

		SND_PlayNow(200, 5);

		game_state->score += 5;
		SNAKE_PrintScore();

		if(!(game_state->score % 20) && game_state->frames_per_move > 1) game_state->frames_per_move--;

		SNAKE_SpawnFood();
	}

	if(game_state->frames_since_move > game_state->frames_per_move)
	{
	    game_state->frames_since_move = 0;

		for(size_t i = ((game_state->tail_len*2) - 1); i > 1; i -= 2)
		{
		    game_state->snake_segments[i] = game_state->snake_segments[i - 2];
		    game_state->snake_segments[i - 1] = game_state->snake_segments[i - 3];
		}

		game_state->snake_dir_lock = game_state->snake_dir;

		switch(game_state->snake_dir)
		{
			case 129:
			    *(game_state->snake_pos_y) -= 1;

				break;
			case 130:
			    *(game_state->snake_pos_x) += 1;

				break;
			case 131:
			    *(game_state->snake_pos_y) += 1;

				break;
			case 128:
			    *(game_state->snake_pos_x) -= 1;
				break;
		}


		SNAKE_CheckCollision();

		if(!game_state->game_over)
		{
			for(size_t i = 2; i < game_state->tail_len*2; i += 2)
			{

				tGFX_SetChar(132, game_state->snake_segments[i], game_state->snake_segments[i + 1], 2, 10);

			}

			tGFX_SetChar(0, game_state->snake_segments[game_state->tail_len*2 - 2], game_state->snake_segments[game_state->tail_len*2 - 1], 0, 0);

			tGFX_SetChar(game_state->snake_dir, game_state->snake_segments[0], game_state->snake_segments[1], 2, 0);
		}

	}

}
