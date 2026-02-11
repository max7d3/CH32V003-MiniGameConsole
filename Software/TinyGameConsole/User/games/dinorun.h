/*
 * dinorun.h
 *
 *  Created on: Aug 27, 2024
 *      Author: Max
 */

#ifndef USER_GAMES_DINORUN_H_
#define USER_GAMES_DINORUN_H_

#define DINORUN_GRAVITY 3
#define DINORUN_MAX_OBSTACLES 3
#define DINORUN_MAX_OBSTACLE_SPRITES 3

#define DINORUN_START_MAX_SPAWN_DELAY 60
#define DINORUN_START_MIN_SPAWN_DELAY 40

#define DINORUN_TARGET_MAX_SPAWN_DELAY 30
#define DINORUN_TARGET_MIN_SPAWN_DELAY 20

#define DINORUN_SPAWN_DELAY_TARGET_SCORE 10000


typedef enum
{
    DINO_STATE_IDLE,
    DINO_STATE_IN_AIR,
    DINO_STATE_CROUCHING

}dino_state_t;

typedef enum
{
    DINO_OBSTACLE_SINGLE_CACTUS_SMALL,
    DINO_OBSTACLE_SINGLE_CACTUS_BIG,
    DINO_OBSTACLE_DOUBLE_CACTUS_SMALL,
    DINO_OBSTACLE_DOUBLE_CACTUS_BIG,
    DINO_OBSTACLE_GROUP_1,
    DINO_OBSTACLE_PTERODACTYL_HIGH,
    DINO_OBSTACLE_PTERODACTYL_LOW

}dino_obstacle_type_t;

typedef struct
{
    dino_obstacle_type_t type;
    uint8_t in_use;
    term_sprite_t sprite[3];

}dino_obstacle_t;


typedef struct
{

    term_sprite_t dino;
    term_sprite_t dino_legs;
    int8_t dino_acc;
    dino_state_t dino_state;
    uint8_t intial_lock;

    term_sprite_t terrain[5];
    uint8_t terrain_type_map[5];
    uint8_t terrain_move_speed;

    dino_obstacle_t obstacle[3];
    uint16_t last_spawn_score;
    uint8_t spawn_delay;
    uint8_t min_spawn_delay;
    uint8_t max_spawn_delay;

    uint16_t score;

}dino_run_game_state_t;

void DINORUN_Init(void);
void DINORUN_Main(void);

#endif /* USER_GAMES_DINORUN_H_ */
