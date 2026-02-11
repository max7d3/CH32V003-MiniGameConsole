/*
 * pong.h
 *
 *  Created on: Jul 14, 2023
 *      Author: max
 */

#ifndef USER_GAMES_PONG_H_
#define USER_GAMES_PONG_H_

#define PONG_COLL_SOUND_ENV_FREQ 333
#define PONG_COLL_SOUND_PADDLE_FREQ 333
#define PONG_SOUND_LOSS_POINT_FREQ 600


enum pong_char_elements
{
	PONG_LINE_A = 140,
	PONG_LINE_B,
	PONG_U_SEG_0,
	PONG_U_SEG_1,
	PONG_U_SEG_2,
	PONG_U_SEG_3,
	PONG_U_SEG_4,
	PONG_U_SEG_5,
	PONG_U_SEG_6,
	PONG_U_SEG_7,
	PONG_U_SEG_8,
	PONG_U_SEG_9,

	PONG_D_SEG_0,
	PONG_D_SEG_1,
	PONG_D_SEG_2,
	PONG_D_SEG_3,
	PONG_D_SEG_4,
	PONG_D_SEG_5,
	PONG_D_SEG_6,
	PONG_D_SEG_7,
	PONG_D_SEG_8,
	PONG_D_SEG_9,

};

typedef enum
{
	PONG_BALL_DIR_RIGHT_UP,
	PONG_BALL_DIR_RIGHT_DOWN,
	PONG_BALL_DIR_LEFT_UP,
	PONG_BALL_DIR_LEFT_DOWN,


}pong_ball_dir_t;

typedef enum
{
	PONG_BALL_ANGLE_0,
	PONG_BALL_ANGLE_30,
	PONG_BALL_ANGLE_45,
	PONG_BALL_ANGLE_60

}pong_ball_angle_t;

typedef enum
{
	PONG_ENEMY_START,
	PONG_ENEMY_MOVE,

}pong_enemy_state_t;

typedef struct
{
    term_sprite_t ball;
    term_sprite_t paddle_p;
    term_sprite_t paddle_e;

    pong_ball_dir_t ball_dir;
    pong_ball_angle_t ball_angle;

    uint8_t p_score;
    uint8_t e_score;

}pong_game_state_t;



void PONG_Init(void);
void PONG_Main(void);

#endif /* USER_GAMES_PONG_H_ */
