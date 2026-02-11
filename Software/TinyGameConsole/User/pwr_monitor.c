#include <stdlib.h>

#include "debug.h"
#include "ch32v00x.h"
#include "tick.h"

#include "pwr_monitor.h"

static uint8_t power_low = 0;
static uint8_t led_state = 0;
static uint32_t last_tick;

void PWR_MONITOR_Init(void)
{
    // Set PD0 as output
    GPIOD->CFGLR &= ~(0xF << 0);
    GPIOD->CFGLR |= (0x1 << 0);

    // Enable PVD to trigger below 3.3 V
    RCC->APB1PCENR |= RCC_APB1Periph_PWR;
    PWR->CTLR |= (1 << 4) | (0b010 << 5);

    // Turn on the LED
    GPIOD->BSHR = (1 << 0);

}

void PWR_MONITOR_Loop(void)
{
    // Check if PVD is triggered
    if(PWR->CSR & (1 << 2))
    {
        power_low = 1;
    }
    else
    {
        power_low = 0;
    }

    if(power_low)
    {
        if((TICK_Get() - last_tick) > 500)
        {
            last_tick = TICK_Get();

            if(led_state == 1) 
            {
                GPIOD->BCR = (1 << 0);
                led_state = 0;
            }
            else
            {
                GPIOD->BSHR = (1 << 0);
                led_state = 1;
            }
        }
    }
    else
    {
        GPIOD->BSHR = (1 << 0);
    }
}

uint8_t PWR_Monitor_IsPowerLow(void)
{
    return power_low;
}