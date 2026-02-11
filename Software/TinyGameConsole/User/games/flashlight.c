#include <string.h>
#include <stdlib.h>

#include "debug.h"
#include "ch32v00x.h"
#include "termGFX.h"
#include "st7735.h"
#include "buttons.h"
#include "engine.h"
#include "menu.h"


#include "flashlight.h"

void FLASHLIGHT_Init(void)
{
    tGFX_Clear(15);

    ENG_UpdateInFrameCallback(&FLASHLIGHT_Main);
}

void FLASHLIGHT_Main(void)
{
    if(BTN_IsPressed(BTN_UP)) 
	{
		ENG_SetInFrameCallback(&MENU_MainMenuProcess); 
		MENU_ForceBTNNotReleased(); 
		tGFX_Clear(0);
	}
}