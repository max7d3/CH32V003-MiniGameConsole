/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/08/08
 * Description        : Main program body.
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#include <stdlib.h>

#include "debug.h"
#include "ch32v00x.h"
#include "termGFX.h"
#include "st7735.h"
#include "buttons.h"
#include "tick.h"
#include "sound.h"
#include "engine.h"
#include "pwr_monitor.h"
#include "games/menu.h"
#include "games/highscore.h"

static const uint16_t init_sound_seq[] = {200, 10, 300, 10, 400, 10, 500, 10, 600, 10, 700, 10, 800, 10, 900, 10, 1000, 10};
static const uint16_t first_init_sound_seq[] = {200, 10, 300, 10, 200, 10, 300, 10};


void Init(void);

int main(void)
{

	Init();

    if(HIGHSCORE_CheckInitialization() == 1)
    {
        SND_PlaySequence((uint16_t*)first_init_sound_seq, 3);
    }
    else
    {
        SND_PlaySequence((uint16_t*)init_sound_seq, 7);
    }

    tGFX_SetSpritesCount(15);

	ENG_SetFPS(30);

    ENG_SetInFrameCallback(&MENU_MainMenuProcess);

	ENG_MainProcess();
}


void Init(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    TICK_Init();

    LCD_Init();

    SND_Init();

    BTN_Init();

    PWR_MONITOR_Init();

    LCD_OptimizedFill(0, 0, 160, 80, ST7735_BLACK);

}
