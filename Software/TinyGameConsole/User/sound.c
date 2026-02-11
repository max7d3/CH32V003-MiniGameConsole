/*
 * sound.c
 *
 *  Created on: Jul 8, 2023
 *      Author: max
 */

#include "debug.h"
#include "ch32v00x.h"
#include "sound.h"

static volatile sound_play_mode_t play_mode;
static volatile sound_single_play_state_t sps_state;
static volatile sound_sequence_play_state_t seqps_state;
static volatile uint16_t *seq_play_buf;
static volatile uint8_t buf_len;
static volatile uint8_t play_time;
static volatile uint8_t time_counter;
static volatile uint8_t mute;

void SND_Init(void)
{
	RCC->APB1PCENR |= RCC_APB1Periph_TIM2;

	// GPIO PC0 settings (OUTPUT, Multiplexing Push-Pull Mode)
	GPIOC->CFGLR |= (0b11 << 0);
    GPIOC->CFGLR &= ~(0b11 << 2);
    GPIOC->CFGLR |= (0b10 << 2);

	// Timer 2 settings
	TIM2->CHCTLR2 = (0b110 << 4) | (1 << 3); // OC3M Mode: PWM mode 1 | CCR3 Preload Enable
	TIM2->PSC = 999; // 48 MHz / 1000 = 48kHz

	// Auto-relaod and Camptuer Compare Value (CH3) Controls PWM freq. This values result in 50% PWM signal of freq_X and duty 50%: ATRLR = 48000 / freq_X - 1; CH3CVR = 48000 / freq_X / 2
	TIM2->ATRLR = 48000 / 1000 - 1;
	TIM2->CH3CVR = 48000 / 1000 / 2 ;

	// Update configuration
	TIM2->SWEVGR = 1;

	// Start counter | Enable auto-reload
	TIM2->CTLR1 = 1 | (1 << 7);

}

void SND_DisableOutput(void)
{
	TIM2->CCER &= ~(1 << 8);
}

void SND_EnableOutput(void)
{
	TIM2->CCER |= (1 << 8);
}

void SND_SetFreq(uint16_t freq)
{
	if(freq > 6000 || freq < 100) return; // Protect hearing and buzzer

	TIM2->ATRLR = 48000/freq - 1;
	TIM2->CH3CVR = 48000/freq/2 ;
}

// Time is time * 10 ms
void SND_PlayNow(uint16_t freq, uint8_t time)
{
	SND_SetFreq(freq);
	play_time = time;

	play_mode = SOUND_PM_SINGLE;
	sps_state = SOUND_SPS_START;
}

// Len should be actuall lenght - 1 !!
void SND_PlaySequence(uint16_t *buffer, uint8_t len)
{
	seq_play_buf = buffer;
	buf_len = len;
	play_mode = SOUND_PM_SEQUENCE;
	seqps_state = SOUND_SEQPS_START;
}

static void SND_SinglePlay(void)
{
	switch(sps_state)
	{
		case SOUND_SPS_START:

			time_counter = 0;
			SND_EnableOutput();
			sps_state = SOUND_SPS_MAIN;

			break;

		case SOUND_SPS_MAIN:

			time_counter++;
			if(time_counter > play_time) sps_state = SOUND_SPS_STOP;

			break;

		case SOUND_SPS_STOP:

			SND_DisableOutput();
			play_mode = SOUND_PM_MUTE;

			break;
	}
}

static void SND_SequencePlay(void)
{
	static uint16_t buf_pos;

	switch(seqps_state)
	{
		case SOUND_SEQPS_START:

			buf_pos = 0;
			time_counter = 0;
			SND_SetFreq(seq_play_buf[buf_pos]);
			play_time = seq_play_buf[buf_pos + 1];
			SND_EnableOutput();
			seqps_state = SOUND_SEQPS_MAIN;

			break;

		case SOUND_SEQPS_MAIN:

			time_counter++;

			if(time_counter >= play_time)
			{
				if(buf_pos < buf_len*2)
				{
					buf_pos += 2;
					time_counter = 0;
					SND_SetFreq(seq_play_buf[buf_pos]);
					play_time = seq_play_buf[buf_pos + 1];
				}
				else
				{
					SND_DisableOutput();
					seqps_state = SOUND_SEQPS_STOP;
				}
			}

			break;

		case SOUND_SEQPS_STOP:

			play_mode = SOUND_PM_MUTE;

			break;
	}
}

void SND_Mute(void)
{
	SND_DisableOutput();
	mute = 1;
}

void SND_Unmute(void)
{
	mute = 0;
}

uint8_t SND_GetMuteState(void)
{
	return mute;
}

// This function should be executed every 10 ms
void SND_IRQHandler(void)
{
	if(!mute)
	{
		switch(play_mode)
		{
			case SOUND_PM_MUTE:
				break;

			case SOUND_PM_SINGLE:
				SND_SinglePlay();
				break;

			case SOUND_PM_SEQUENCE:
				SND_SequencePlay();
				break;
		}
	}
}