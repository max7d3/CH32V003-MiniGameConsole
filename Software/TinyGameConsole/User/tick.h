/*
 * tick.h
 *
 *  Created on: Jul 4, 2023
 *      Author: max
 */

#ifndef USER_TICK_H_
#define USER_TICK_H_

void TICK_Init(void);
uint32_t TICK_Get(void);
void Delay(uint32_t ms);

#endif /* USER_TICK_H_ */
