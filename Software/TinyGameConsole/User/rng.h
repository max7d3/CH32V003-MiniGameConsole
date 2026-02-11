/*
 * rng.h
 *
 * Pseudo RNG for games.
 *
 *  Created on: Sep 19, 2024
 *      Author: Max
 */

#ifndef USER_RNG_H_
#define USER_RNG_H_

void RNG_SetSeed(uint32_t seed);
uint32_t RNG_Get(void);
uint32_t RNG_GetRange(uint32_t min, uint32_t max);

#endif /* USER_RNG_H_ */
