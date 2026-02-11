/*
 * engine.c
 *
 *  Created on: Jul 9, 2023
 *      Author: max
 */

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

void (*ENG_InFrameCallback)(void) = NULL;
void (*ENG_OutFrameCallback)(void) = NULL;

void (*ENG_InFrameCallbackToSet)(void) = NULL;

static uint32_t frame_time = 33;
static uint32_t last_frame_time = 33;

static uint8_t engine_activ = 1;
static uint8_t update = 0;

uint8_t eng_common_buf[ENG_COMMON_BUFFER_SIZE];
static uint16_t common_buf_pos = 0;

void ENG_SetInFrameCallback(void *callback)
{
	ENG_InFrameCallback = callback;
}

void ENG_UpdateInFrameCallback(void *callback)
{
	ENG_InFrameCallbackToSet = callback;
	update = 1;
}

void ENG_SetOutFrameCallback(void *callback)
{
	ENG_OutFrameCallback = callback;
}

void ENG_SetFrameTime(uint8_t ms)
{
	frame_time = ms;
}

void ENG_SetFPS(uint8_t fps)
{
	frame_time = 1000/fps;
}

void ENG_Deactivate(void)
{
	engine_activ = 0;
}

void ENG_ClearCommonBuf(void)
{
    for(uint16_t i = 0; i < ENG_COMMON_BUFFER_SIZE; i++)
    {
        eng_common_buf[i] = 0;
    }
	//memset(eng_common_buf, 0, 512); high flash usage
	common_buf_pos = 0;
}

void *ENG_CommonBufAlloc(uint16_t size)
{
    uint16_t current_pos = common_buf_pos;
    common_buf_pos += size;
    return (common_buf_pos < ENG_COMMON_BUFFER_SIZE - 1) ? eng_common_buf + current_pos : NULL;
}

uint16_t ENG_CommonBufGetFreeBytes(void)
{
    return ENG_COMMON_BUFFER_SIZE - common_buf_pos;
}

void ENG_MainProcess(void)
{
    while(engine_activ)
    {
    	if(update){ ENG_SetInFrameCallback(ENG_InFrameCallbackToSet); update = 0;}

    	if ((TICK_Get() - last_frame_time) > frame_time)
    	{
    		last_frame_time = TICK_Get();

        	if(ENG_InFrameCallback != NULL)
        	{
        		ENG_InFrameCallback();
        	}

        	tGFX_Update();
    	}

    	BTN_Task();
		PWR_MONITOR_Loop();

    	if(ENG_OutFrameCallback != NULL)
    	{
    		ENG_OutFrameCallback();
    	}
    }
}
