/*
 * sound.h
 *
 *  Created on: Jul 8, 2023
 *      Author: max
 */

#ifndef USER_SOUND_H_
#define USER_SOUND_H_

typedef enum
{
	SOUND_PM_MUTE,
	SOUND_PM_SINGLE,
	SOUND_PM_SEQUENCE

}sound_play_mode_t;

typedef enum
{
	SOUND_SPS_START,
	SOUND_SPS_MAIN,
	SOUND_SPS_STOP

}sound_single_play_state_t;

typedef enum
{
	SOUND_SEQPS_START,
	SOUND_SEQPS_MAIN,
	SOUND_SEQPS_STOP

}sound_sequence_play_state_t;




void SND_Init(void);
void SND_DisableOutput(void);
void SND_EnableOutput(void);
void SND_SetFreq(uint16_t freq);
void SND_PlayNow(uint16_t freq, uint8_t time);
void SND_PlaySequence(uint16_t *buffer, uint8_t len);
void SND_Unmute(void);
void SND_Mute(void);
uint8_t SND_GetMuteState(void);
void SND_IRQHandler(void);

#endif /* USER_SOUND_H_ */
