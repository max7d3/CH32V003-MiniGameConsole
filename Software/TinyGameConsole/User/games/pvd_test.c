
#include <stdlib.h>

#include "debug.h"
#include "ch32v00x.h"
#include "termGFX.h"
#include "buttons.h"
#include "tick.h"
#include "engine.h"
#include "menu.h"

#include "pvd_test.h"

uint8_t pvd_val;
static uint8_t btn_relesed = 1;

void set_pvd_value(uint8_t val)
{
    PWR->CTLR &= ~(0b111 << 5);
    PWR->CTLR |= (val << 5);
}

void PVD_TEST_Init(void)
{
    // Clear background
    tGFX_Clear(0);

    RCC->APB1PCENR |= RCC_APB1Periph_PWR;
    PWR->CTLR |= (1 << 4);

    ENG_UpdateInFrameCallback(&PVD_TEST_Main);
}


void PVD_TEST_Main(void)
{
    // Clear background
    tGFX_Clear(0);
    tGFX_SetCursor(0, 0);

    tGFX_Print("PVD: 0b", 10, 0);

    if((pvd_val >> 2) & 1) tGFX_Print("1", 10, 0);
    else tGFX_Print("0", 10, 0);


    if((pvd_val >> 1) & 1) tGFX_Print("1", 10, 0);
    else tGFX_Print("0", 10, 0);


    if((pvd_val >> 0) & 1) tGFX_Print("1", 10, 0);
    else tGFX_Print("0", 10, 0);

    tGFX_SetCursor(0, 2);
    tGFX_Print("Triggered?: ", 10, 0);

    if(PWR->CSR & (1 << 2)) tGFX_Print("1", 10, 0);
    else tGFX_Print("0", 10, 0);

    tGFX_SetCursor(0, 3);
    tGFX_Print("Enabled?: ", 10, 0);

    if(PWR->CTLR & (1 << 4)) tGFX_Print("1", 10, 0);
    else tGFX_Print("0", 10, 0);

	if(btn_relesed)
	{
		if(BTN_IsPressed(BTN_UP))
		{
			btn_relesed = 0;
			if(pvd_val < 7) pvd_val++;
		}

		if(BTN_IsPressed(BTN_DOWN))
		{
			btn_relesed = 0;
			if(pvd_val != 0) pvd_val--;
		}

        set_pvd_value(pvd_val);

	}
	else if(!BTN_IsPressed(BTN_DOWN) && !BTN_IsPressed(BTN_UP))
	{
		btn_relesed = 1;
	}

    if(BTN_IsPressed(BTN_RIGHT)) {ENG_SetInFrameCallback(&MENU_MainMenuProcess); MENU_ForceBTNNotReleased(); tGFX_Clear(0);}
}

