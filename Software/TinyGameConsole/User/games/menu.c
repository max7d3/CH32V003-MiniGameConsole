/*
 * menu.c
 *
 *  Created on: Jul 11, 2023
 *      Author: max
 */

#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "ch32v00x.h"
#include "termGFX.h"
#include "st7735.h"
#include "buttons.h"
#include "tick.h"
#include "sound.h"
#include "engine.h"
#include "pwr_monitor.h"
#include "menu.h"

#include "snake.h"
#include "pong.h"
#include "jumpbird.h"
#include "dinorun.h"
#include "highscore.h"
#include "flashlight.h"

static const menu_option_t menu[MENU_ITEMS_COUNT] =
{
		{"SNAKE", &MENU_SetGameSnake},
		{"PONG", &MENU_SetGamePong},
		{"JUMP BIRD", &MENU_SetGameJumpBird},
		{"DINO RUN", &MENU_SetGameDinoRun},
		{"HIGH SCORE", &MENU_SetGameHighscore},
		{"SOUND: ", &MENU_ChangeSound},
		{"FLASHLIGHT", &MENU_SetGameFlashlight},
};

static uint8_t btn_relesed = 1;
static uint8_t selected_option;

void MENU_SetGameSnake(void)
{
	    ENG_UpdateInFrameCallback(&SNAKE_Init);
}

void MENU_SetGamePong(void)
{
	    ENG_UpdateInFrameCallback(&PONG_Init);
}

void MENU_SetGameJumpBird(void)
{
        ENG_UpdateInFrameCallback(&JUMPBIRD_Init);
}

void MENU_SetGameDinoRun(void)
{
        ENG_UpdateInFrameCallback(&DINORUN_Init);
}

void MENU_SetGameHighscore(void)
{
        ENG_UpdateInFrameCallback(&HIGHSCORE_Init);
}

void MENU_SetGameFlashlight(void)
{
		ENG_UpdateInFrameCallback(&FLASHLIGHT_Init);
}


void MENU_ChangeSound(void)
{
    if(SND_GetMuteState() == 0)
	{
		SND_Mute();
	}
	else
	{
		SND_Unmute();
		SND_PlayNow(300, 10);
	}
}

void MENU_MainMenuProcess(void)
{
	MENU_SelectionProcess();
	tGFX_Clear(0);

	tGFX_SetCursor(0, 0);
	tGFX_Print(">MAIN MENU", MENU_COLOR_FOREGROUND, 0);

	for(size_t i = 0; i < MENU_ITEMS_COUNT; i++)
	{

		tGFX_SetCursor(0, i + 1);

	    if(selected_option == i)
	    {
	        tGFX_Print((char*)menu[i].name, MENU_COLOR_BACKGROUND, MENU_COLOR_FOREGROUND);
	    }
	    else
	    {
	    	tGFX_Print((char*)menu[i].name, MENU_COLOR_FOREGROUND, MENU_COLOR_BACKGROUND);
	    }
	}

	if(SND_GetMuteState() == 0)
	{
		tGFX_SetCursor(strlen(menu[MENU_SOUND_POS].name), MENU_SOUND_POS + 1);
		tGFX_Print("   ", MENU_COLOR_BACKGROUND, MENU_COLOR_BACKGROUND); // Clear
		tGFX_SetCursor(strlen(menu[MENU_SOUND_POS].name), MENU_SOUND_POS + 1);

		if(selected_option == MENU_SOUND_POS)
		{
			tGFX_Print("ON", MENU_COLOR_BACKGROUND, MENU_COLOR_FOREGROUND);
		}
		else
		{
			tGFX_Print("ON", MENU_COLOR_FOREGROUND, MENU_COLOR_BACKGROUND);
		}
	}
	else
	{
		tGFX_SetCursor(strlen(menu[MENU_SOUND_POS].name), MENU_SOUND_POS + 1);

		if(selected_option == MENU_SOUND_POS)
		{
			tGFX_Print("OFF", MENU_COLOR_BACKGROUND, MENU_COLOR_FOREGROUND);
		}
		else
		{
			tGFX_Print("OFF", MENU_COLOR_FOREGROUND, MENU_COLOR_BACKGROUND);
		}		
	}

	if(PWR_Monitor_IsPowerLow() == 1)
	{
		tGFX_SetCursor(1, 9);
		tGFX_Print("!! LOW BATTERY !!", 9, 0);
	}
}

void MENU_ForceBTNNotReleased(void)
{
	btn_relesed = 0;
}

void MENU_SelectionProcess(void)
{
	if(btn_relesed)
	{
		if(BTN_IsPressed(BTN_DOWN))
		{
			btn_relesed = 0;
			if(selected_option < MENU_ITEMS_COUNT - 1) selected_option++;
			SND_PlayNow(300, 10);
		}

		if(BTN_IsPressed(BTN_UP))
		{
			btn_relesed = 0;
			if(selected_option != 0) selected_option--;
			SND_PlayNow(300, 10);
		}

		if(BTN_IsPressed(BTN_ACTION))
		{
			btn_relesed = 0;
			menu[selected_option].Action();

		}
	}
	else if(!BTN_IsPressed(BTN_DOWN) && !BTN_IsPressed(BTN_UP) && !BTN_IsPressed(BTN_ACTION))
	{
		btn_relesed = 1;
	}
}

