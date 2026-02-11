/*
 * buttons.c
 *
 *  Created on: Jun 25, 2023
 *      Author: max
 */

#include <stdlib.h>

#include "debug.h"
#include "ch32v00x.h"
#include "tick.h"

#include "buttons.h"

/*
*					BUTTONS CONNECTIONS
*
*			PD2 -> UP
*
*	PC7 -> LEFT		PD3 -> RIGHT	PD4 -> ACTION
*			
*			PC1 -> DOWN
*
*/

volatile button_status_t BUTTONS[BTN_COUNT];

void BTN_Init(void)
{
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD;

	// Clear previous configuration and set new one (input with pull resistors)
	GPIOC->CFGLR &= ~((0b11 << 30) | (0b11 << 6));
	GPIOC->CFGLR |= (0b10 << 30) | (0b10 << 6);

	// Clear previous configuration and set new one (input with pull resistors)
	GPIOD->CFGLR &= ~((0b11 << 10) | (0b11 << 14) | (0b11 << 18));
	GPIOD->CFGLR |= (0b10 << 10) | (0b10 << 14) | (0b10 << 18);

	// Enable pull-ups
	GPIOC->OUTDR |= (1 << 7) | (1 << 1);
	GPIOD->OUTDR |= (1 << 2) | (1 << 3) | (1 << 4);


}

static uint8_t _BTN_CheckState(buttons_names_t button)
{
    switch (button)
    {

        case BTN_ACTION:
        return (GPIOD->INDR >> 4) & 1;

        case BTN_RIGHT:
        return (GPIOD->INDR >> 3) & 1;

        case BTN_UP:
        return (GPIOD->INDR >> 2) & 1;

        case BTN_LEFT:
        return (GPIOC->INDR >> 7) & 1;

        case BTN_DOWN:
        return (GPIOC->INDR >> 1) & 1;

        default:
        return 0;

    }
}

void BTN_Task(void)
{
	for(size_t i = 0; i < BTN_COUNT; i++)
	{
		switch(BUTTONS[i].state)
		{
			case BTN_STATE_IDLE:

				if(_BTN_CheckState(i) == 0)
				{
					BUTTONS[i].state = BTN_STATE_DEBOUNCE;
					BUTTONS[i].last_tick = TICK_Get();
				}

				break;

			case BTN_STATE_DEBOUNCE:

				if ((TICK_Get() - BUTTONS[i].last_tick) > BTN_DEBOUNCE_TIME)
				{
					if (_BTN_CheckState(i) == 0)
					{
						BUTTONS[i].state = BTN_STATE_PRESSED;
					}
					else
					{
						BUTTONS[i].state = BTN_STATE_IDLE;
					}

				}

				break;

			case BTN_STATE_PRESSED:

				if (_BTN_CheckState(i) == 1)
				{
					BUTTONS[i].state = BTN_STATE_IDLE;
				}

				break;
		}
	}
}

uint8_t BTN_IsPressed(buttons_names_t button)
{
	if(BUTTONS[button].state == BTN_STATE_PRESSED) return 1;
	return 0;
}

uint8_t BTN_IsPressedRAW(buttons_names_t button)
{
	if(!_BTN_CheckState(button)) return 1;
	return 0;
}