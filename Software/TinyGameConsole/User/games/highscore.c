/*
 * highscore.c
 *
 *  Created on: Sep 27, 2024
 *      Author: Max
 */


#include <stdlib.h>

#include "debug.h"
#include "ch32v00x.h"
#include "termGFX.h"
#include "buttons.h"
#include "tick.h"
#include "engine.h"
#include "menu.h"

#include "highscore.h"

static uint32_t highscore_data[HIGHSCORE_DATA_ID_COUNT];

static const highscore_display_data_t highscore_display_data[HIGHSCORE_DISPLAY_LINES] =
{
        {"HIGHEST SCORES", 0xFF},
        {"SNAKE: ", HIGHSCORE_SNAKE},
        {"JUMP BIRD: ", HIGHSCORE_JUMPBIRD},
        {"DINO RUN: ", HIGHSCORE_DINORUN},
        {"PONG WINS", 0xFF},
        {"Player: ", HIGHSCORE_PONG_P_WINS},
        {"AI: ", HIGHSCORE_PONG_E_WINS},
};

static void HIGHSCORE_LoadData(void)
{
    uint32_t *ptr = (uint32_t *)HIGHSCORE_PAGE_ADR;

    for (uint8_t i = 0; i < HIGHSCORE_DATA_ID_COUNT; i++)
    {
        highscore_data[i] = *ptr++;
    }
}

static void HIGHSCORE_WriteData(void)
{
    FLASH_Unlock_Fast();

    FLASH_ErasePage_Fast(HIGHSCORE_PAGE_ADR);

    FLASH_BufReset();

    uint32_t data[16];

    for (uint8_t i = 0; i < 16; i++)
    {
        if(i < HIGHSCORE_DATA_ID_COUNT)
        {
            data[i] = highscore_data[i];
        }
        else
        {
            data[i] = 0;
        }
    }

    for (uint8_t i = 0; i < 16; i++)
    {
        FLASH_BufLoad(HIGHSCORE_PAGE_ADR + (i * 4), data[i]);
    }

    FLASH_ProgramPage_Fast(HIGHSCORE_PAGE_ADR);
    FLASH_Lock_Fast();
}


static void HIGHSCORE_WipeAllData(void)
{
    FLASH_Unlock_Fast();

    FLASH_ErasePage_Fast(HIGHSCORE_PAGE_ADR);

    FLASH_BufReset();

    // Write the magic numbers and fill the rest with zeros
    FLASH_BufLoad(HIGHSCORE_PAGE_ADR, HIGHSCORE_INIT_MAGIC_NUMBER);

    for (uint8_t i = 1; i < 16; i++)
    {
        FLASH_BufLoad(HIGHSCORE_PAGE_ADR + (i*4), 0);
    }

    FLASH_ProgramPage_Fast(HIGHSCORE_PAGE_ADR);
    FLASH_Lock_Fast();
}


uint32_t HIGHSCORE_Read(highscore_data_id_t id)
{
    HIGHSCORE_LoadData();

    return highscore_data[id];
}


void HIGHSCORE_Write(highscore_data_id_t id, uint32_t val)
{
    HIGHSCORE_LoadData();

    highscore_data[id] = val;

    HIGHSCORE_WriteData();
}


void HIGHSCORE_WipeConfScreen(void)
{
    tGFX_Clear(0);
    tGFX_SetCursor(0, 0);
    tGFX_Print("PRESS DOWN TO WIPE", MENU_COLOR_FOREGROUND, MENU_COLOR_BACKGROUND);
    tGFX_SetCursor(0, 1);
    tGFX_Print("ALL DATA", MENU_COLOR_FOREGROUND, MENU_COLOR_BACKGROUND);

    tGFX_SetCursor(0, 3);
    tGFX_Print("PRESS UP TO", MENU_COLOR_FOREGROUND, MENU_COLOR_BACKGROUND);
    tGFX_SetCursor(0, 4);
    tGFX_Print("CANCEL", MENU_COLOR_FOREGROUND, MENU_COLOR_BACKGROUND);

    if(BTN_IsPressed(BTN_UP)) ENG_UpdateInFrameCallback(&HIGHSCORE_Main);
    if(BTN_IsPressed(BTN_DOWN)) {HIGHSCORE_WipeAllData(); ENG_SetInFrameCallback(&MENU_MainMenuProcess); MENU_ForceBTNNotReleased(); tGFX_Clear(0);}
}


void HIGHSCORE_Init(void)
{
    // Clear background
    tGFX_Clear(0);

    ENG_UpdateInFrameCallback(&HIGHSCORE_Main);
}


void HIGHSCORE_Main(void)
{
    char buf[16];

    // Clear background
    tGFX_Clear(0);

    for (uint8_t i = 0; i < HIGHSCORE_DISPLAY_LINES; i++)
    {
        if(highscore_display_data[i].data_id == 0xFF)
        {
            tGFX_SetCursor(0, i);
            tGFX_Print((char *)highscore_display_data[i].name, MENU_COLOR_FOREGROUND, MENU_COLOR_BACKGROUND);
        }
        else
        {
            tGFX_SetCursor(0, i);
            tGFX_Print((char *)highscore_display_data[i].name, MENU_COLOR_FOREGROUND, MENU_COLOR_BACKGROUND);
            itoa(HIGHSCORE_Read(highscore_display_data[i].data_id), buf, 10);
            tGFX_Print(buf, 15, 0);
        }
    }


    if(BTN_IsPressed(BTN_LEFT) && BTN_IsPressed(BTN_RIGHT) && BTN_IsPressed(BTN_ACTION)) ENG_SetInFrameCallback(&HIGHSCORE_WipeConfScreen);

    if(BTN_IsPressed(BTN_UP)) {ENG_SetInFrameCallback(&MENU_MainMenuProcess); MENU_ForceBTNNotReleased(); tGFX_Clear(0);}

}

uint32_t HIGHSCORE_CheckInitialization(void)
{
    if(HIGHSCORE_Read(HIGHSCORE_INIT_FLAG) != HIGHSCORE_INIT_MAGIC_NUMBER)
    {
        // In case of first run after programming "format" the storage area
        HIGHSCORE_WipeAllData();
        return 1;

    }

    return 0;
}