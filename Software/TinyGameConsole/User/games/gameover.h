/*
 * gameover.h
 *
 *  Created on: Sep 19, 2024
 *      Author: Max
 */

#ifndef USER_GAMES_GAMEOVER_H_
#define USER_GAMES_GAMEOVER_H_

typedef enum
{
    GAMEOVER_TYPE_SCORE,
    GAMEOVER_TYPE_MESSAGE

}gameover_type_t;

void GAMEOVER_Setup(gameover_type_t type, uint32_t score, uint32_t highscore, buttons_names_t button, const char *msg);
void GAMEOVER_Show(void);

#endif /* USER_GAMES_GAMEOVER_H_ */
