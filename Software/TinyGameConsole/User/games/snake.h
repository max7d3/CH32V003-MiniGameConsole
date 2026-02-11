/*
 * snake.h
 *
 *  Created on: Jul 11, 2023
 *      Author: max
 */

#ifndef USER_GAMES_SNAKE_H_
#define USER_GAMES_SNAKE_H_



typedef struct
{
    uint8_t frames_per_move;
    uint8_t frames_since_move;
    uint8_t game_over;


    uint8_t *snake_pos_x;
    uint8_t *snake_pos_y;

    uint8_t food_x;
    uint8_t food_y;


    uint8_t snake_dir;
    uint8_t snake_dir_lock;

    uint8_t tail_len;

    uint16_t score;

    uint8_t snake_segments[256];

}snake_game_state_t;

void SNAKE_Init(void);
void SNAKE_Main(void);

#endif /* USER_GAMES_SNAKE_H_ */
