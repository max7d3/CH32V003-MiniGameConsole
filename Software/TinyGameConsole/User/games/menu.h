/*
 * menu.h
 *
 *  Created on: Jul 11, 2023
 *      Author: max
 */

#ifndef USER_GAMES_MENU_H_
#define USER_GAMES_MENU_H_

#define MENU_MAX_CHAR_LEN 16
#define MENU_ITEMS_COUNT 7

#define MENU_COLOR_FOREGROUND 10
#define MENU_COLOR_BACKGROUND 0

#define MENU_SOUND_POS 5

typedef struct
{
	char *name;
	void (*Action)(void);

}menu_option_t;

void MENU_SetGameSnake(void);

void MENU_SetGamePong(void);

void MENU_SetGameJumpBird(void);

void MENU_SetGameDinoRun(void);

void MENU_SetGameHighscore(void);

void MENU_ChangeSound(void);

void MENU_SetGameFlashlight(void);

void MENU_MainMenuProcess(void);
void MENU_ForceBTNNotReleased(void);
void MENU_SelectionProcess(void);
#endif /* USER_GAMES_MENU_H_ */
