/*
 * jumpbird.h
 *
 *  Created on: Aug 26, 2023
 *      Author: Maksym
 */

#ifndef USER_GAMES_JUMPBIRD_H_
#define USER_GAMES_JUMPBIRD_H_

#define JUMPBIRD_PIPE_MAX_LEN 48
#define JUMPBIRD_GRAVITY 2

#define JUMPBIRD_POINT_FREQ 404
#define JUMPBIRD_JUMP_FREQ 303

typedef struct
{
    term_sprite_t pipes_upper[3];
    term_sprite_t pipes_upper_cap[3];

    term_sprite_t pipes_lower[3];
    term_sprite_t pipes_lower_cap[3];

    term_sprite_t bird;

    int16_t pipe_pos[3];

    int8_t pipe_gap_pos[3];

    int8_t bird_acc;

    uint8_t released;

    uint8_t collision;

    uint8_t score;

}jump_bird_game_state_t;

void JUMPBIRD_Init(void);
void JUMPBIRD_Main(void);


#endif /* USER_GAMES_JUMPBIRD_H_ */
