/*
 * tick.c
 *
 *  Created on: Jul 4, 2023
 *      Author: max
 */


#include "debug.h"
#include "ch32v00x.h"

#include "tick.h"
#include "sound.h"

static volatile uint32_t _tick;

void SysTick_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void TICK_Init(void)
{
	SysTick->SR &= ~(1 << 0);
	SysTick->CMP = ((SystemCoreClock / 8000000) * 1000) - 1;
	SysTick->CNT = 0;
	SysTick->CTLR = (1 << 3) |  (1 << 1) | 1; // Enable SysTick, IRQ and auto-reload ST
	NVIC_EnableIRQ(SysTicK_IRQn);
}

uint32_t TICK_Get(void)
{
    return _tick;
}

void Delay(uint32_t ms)
{
    uint32_t start_tick = _tick;

    while ((_tick - start_tick) < ms)
    {
        /* DO NOTHING */
    }

}

void SysTick_Handler(void)
{
	SysTick->SR = 0;
	_tick++;

	if(!(_tick % 10))
	{
		SND_IRQHandler();
	}
}