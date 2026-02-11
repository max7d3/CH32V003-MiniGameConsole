/*
 * engine.h
 *
 *  Created on: Jul 9, 2023
 *      Author: max
 */

#ifndef USER_ENGINE_H_
#define USER_ENGINE_H_

#define ENG_COMMON_BUFFER_SIZE 512

void ENG_SetInFrameCallback(void *callback);
void ENG_SetOutFrameCallback(void *callback);


void ENG_UpdateInFrameCallback(void *callback);

void ENG_MainProcess(void);
void ENG_SetFPS(uint8_t fps);
void ENG_ClearCommonBuf(void);

void *ENG_CommonBufAlloc(uint16_t size);
uint16_t ENG_CommonBufGetFreeBytes(void);

extern uint8_t eng_common_buf[ENG_COMMON_BUFFER_SIZE];


#endif /* USER_ENGINE_H_ */
