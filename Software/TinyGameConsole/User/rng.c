/*
 * rng.h
 *
 * Pseudo RNG for games.
 *
 *  Created on: Sep 19, 2024
 *      Author: Max
 */


#define LCG_A 1664525
#define LCG_C 1013904223
#define LCG_M 4294967296 // 2^32

#include <stdint.h>
#include "rng.h"

static uint32_t lcg_seed = 1;

void RNG_SetSeed(uint32_t seed)
{
    lcg_seed = seed;
}

// Simple LCG function to generate a pseudo-random number
uint32_t RNG_Get(void)
{
    lcg_seed = (LCG_A * lcg_seed + LCG_C) % LCG_M;
    return lcg_seed;
}

uint32_t RNG_GetRange(uint32_t min, uint32_t max)
{
    return min + (RNG_Get() % (max - min + 1));
}
