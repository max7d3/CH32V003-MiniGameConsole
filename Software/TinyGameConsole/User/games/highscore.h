/*
 * highscore.h
 *
 *  Created on: Sep 27, 2024
 *      Author: Max
 */

#ifndef USER_GAMES_HIGHSCORE_H_
#define USER_GAMES_HIGHSCORE_H_

// The last page of the 16 kB of total memory. Capacity: 64 bytes
#define HIGHSCORE_PAGE_ADR 0x08003FC0
#define HIGHSCORE_INIT_MAGIC_NUMBER 0x42

#define HIGHSCORE_DISPLAY_LINES 7

typedef enum
{
    HIGHSCORE_INIT_FLAG,
    HIGHSCORE_SNAKE,
    HIGHSCORE_JUMPBIRD,
    HIGHSCORE_DINORUN,
    HIGHSCORE_PONG_P_WINS,
    HIGHSCORE_PONG_E_WINS,
    HIGHSCORE_DATA_ID_COUNT,

}highscore_data_id_t;

typedef struct
{
    const char *name;
    highscore_data_id_t data_id;

}highscore_display_data_t;


void HIGHSCORE_Init(void);
void HIGHSCORE_Main(void);

uint32_t HIGHSCORE_Read(highscore_data_id_t id);
void HIGHSCORE_Write(highscore_data_id_t id, uint32_t val);
uint32_t HIGHSCORE_CheckInitialization(void);

#endif /* USER_GAMES_HIGHSCORE_H_ */
