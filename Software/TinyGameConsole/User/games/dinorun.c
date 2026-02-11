/*
 * dinorun.c
 *
 *  Created on: Aug 27, 2024
 *      Author: Max
 */


#include <string.h>
#include <stdlib.h>

#include "debug.h"
#include "ch32v00x.h"
#include "termGFX.h"
#include "st7735.h"
#include "buttons.h"
#include "tick.h"
#include "sound.h"
#include "engine.h"
#include "menu.h"
#include "gameover.h"
#include "rng.h"
#include "highscore.h"

#include "dinorun.h"


static const uint8_t dino_main_body_bitmap[] =
{
        0b00000000, 0b00000000, 0b00000000, 0b01010101, 0b01010100,
        0b00000000, 0b00000000, 0b00000001, 0b01010101, 0b01010101,
        0b00000000, 0b00000000, 0b00000001, 0b01000101, 0b01010101,
        0b00000000, 0b00000000, 0b00000001, 0b01010101, 0b01010101,
        0b00000000, 0b00000000, 0b00000001, 0b01010101, 0b01010101,
        0b01000000, 0b00000000, 0b00000001, 0b01010000, 0b00000000,
        0b01010000, 0b00000000, 0b00000101, 0b01010101, 0b01010000,
        0b01010100, 0b00000000, 0b01010101, 0b01010000, 0b00000000,
        0b00010101, 0b00000001, 0b01010101, 0b01010000, 0b00000000,
        0b00000101, 0b01010101, 0b01010101, 0b01010101, 0b00000000,
        0b00000001, 0b01010101, 0b01010101, 0b01010001, 0b00000000,
        0b00000000, 0b01010101, 0b01010101, 0b01010000, 0b00000000,
        0b00000000, 0b00010101, 0b01010101, 0b01010000, 0b00000000
};

static const uint8_t dino_crouching_body_bitmap[] =
{
    0b00010100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000101, 0b01010101, 0b01000000,
    0b00010101, 0b00000000, 0b00010101, 0b01010101, 0b01000000, 0b00010101, 0b01010101, 0b01010000,
    0b00000101, 0b01000000, 0b01010101, 0b01010101, 0b01010001, 0b01010100, 0b01010101, 0b01010000,
    0b00000001, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010000,
    0b00000000, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010000,
    0b00000000, 0b00010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b00000000, 0b00000000,
    0b00000000, 0b00000101, 0b01010101, 0b01010101, 0b01010100, 0b00010101, 0b01010101, 0b00000000,
    0b00000000, 0b00000001, 0b01010101, 0b01010101, 0b01010100, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00010000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00010100, 0b00000000, 0b00000000, 0b00000000,
};

static const uint8_t dino_legs_idle_bitmap[] =
{
        0b01010101, 0b01010100,
        0b00010101, 0b01010100,
        0b00010101, 0b00010100,
        0b00010100, 0b00010100,
        0b00010100, 0b00010000,
        0b00010000, 0b00010000,
        0b00010101, 0b00010101
};

static const uint8_t dino_legs_step_1_bitmap[] =
{
    0b01010101, 0b01010100,
    0b00010101, 0b01010100,
    0b00010101, 0b00010100,
    0b00010100, 0b00010000,
    0b00010100, 0b00010101,
    0b00010000, 0b00000000,
    0b00010101, 0b00000000,
};

static const uint8_t dino_legs_step_2_bitmap[] =
{
    0b01010101, 0b01010100,
    0b00010101, 0b01010100,
    0b00010100, 0b00010100,
    0b00010000, 0b00010100,
    0b00010101, 0b00010000,
    0b00000000, 0b00010000,
    0b00000000, 0b00010101,
};


static const uint8_t terrain_type_0[] =
{
        0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,
        0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
        0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01010000,
        0b00000000, 0b00000000, 0b00000101, 0b01000000, 0b00000000, 0b00000000, 0b00000001, 0b00000000, 0b00000000, 0b01000000,
        0b00000000, 0b00010000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 0b01000000, 0b00000000,
        0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00010100, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
        0b00000101, 0b00000000, 0b00000000, 0b00000100, 0b00000000, 0b00010100, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
        0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
};

static const uint8_t terrain_type_1[] =
{
        0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,
        0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
        0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000100, 0b00000000,
        0b00000000, 0b00000100, 0b00000000, 0b00000001, 0b00000000, 0b00000001, 0b00000000, 0b00000000, 0b00000000, 0b00000100,
        0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
        0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000100, 0b00000000, 0b00000001, 0b00000000, 0b00000000,
        0b00000100, 0b00000000, 0b00000001, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
        0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
};

static const uint8_t terrain_type_2[] =
{
        0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,
        0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
        0b00000000, 0b00000000, 0b00000000, 0b00000100, 0b00000000, 0b00000000, 0b00010100, 0b00000000, 0b00000000, 0b00000100,
        0b00000000, 0b00000001, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
        0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
        0b00000101, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01000000, 0b00000000,
        0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00010100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
        0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
};

static const uint8_t terrain_type_3[] =
{
        0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,
        0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
        0b00000001, 0b00000000, 0b00000000, 0b00000000, 0b01000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
        0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
        0b00000000, 0b00000000, 0b01010000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 0b01000000, 0b00000000,
        0b00000000, 0b00000000, 0b01010000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00010100,
        0b00000000, 0b00000000, 0b00000000, 0b00000001, 0b01000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
        0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
};


static const uint8_t cactus_small_bitmap[] =
{
        0b00000000, 0b01000000, 0b00000000,
        0b00000001, 0b01010000, 0b00000000,
        0b00000001, 0b01010001, 0b00000000,
        0b00010001, 0b01010001, 0b01000000,
        0b01010001, 0b01010001, 0b01000000,
        0b01010001, 0b01010001, 0b01000000,
        0b01010001, 0b01010101, 0b01000000,
        0b01010101, 0b01010101, 0b00000000,
        0b00010101, 0b01010000, 0b00000000,
        0b00000001, 0b01010000, 0b00000000,
        0b00000001, 0b01010000, 0b00000000,
        0b00000001, 0b01010000, 0b00000000,
        0b00000001, 0b01010000, 0b00000000,
        0b00000001, 0b01010000, 0b00000000,

};

static const uint8_t cactus_big_bitmap[] =
{
    0b00000000, 0b00000001, 0b01000000, 0b00000000,
    0b00000000, 0b00000101, 0b01010000, 0b00000000,
    0b00000000, 0b00000101, 0b01010000, 0b00000000,
    0b00000001, 0b00000101, 0b01010000, 0b00000000,
    0b00000101, 0b01000101, 0b01010000, 0b00000000,
    0b00000101, 0b01000101, 0b01010000, 0b00000000,
    0b00000101, 0b01000101, 0b01010000, 0b00000000,
    0b00000101, 0b01010101, 0b01010000, 0b01000000,
    0b00000001, 0b01010101, 0b01010001, 0b01010000,
    0b00000000, 0b01010101, 0b01010001, 0b01010000,
    0b00000000, 0b00000101, 0b01010001, 0b01010000,
    0b00000000, 0b00000101, 0b01010101, 0b01010000,
    0b00000000, 0b00000101, 0b01010101, 0b01000000,
    0b00000000, 0b00000101, 0b01010101, 0b00000000,
    0b00000000, 0b00000101, 0b01010000, 0b00000000,
    0b00000000, 0b00000101, 0b01010000, 0b00000000,
    0b00000000, 0b00000101, 0b01010000, 0b00000000,
    0b00000000, 0b00000101, 0b01010000, 0b00000000,
    0b00000000, 0b00000101, 0b01010000, 0b00000000,
    0b00000000, 0b00000101, 0b01010000, 0b00000000,
};

static const uint8_t pterodactyl_wing_down_bitmap[] =
{
    0b00000000, 0b01010000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000001, 0b01010100, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000101, 0b01010100, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00010101, 0b01010101, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b01010101, 0b01010101, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000101, 0b01010101, 0b01010100, 0b00000000, 0b00000000,
    0b00000000, 0b00000001, 0b01010101, 0b01010101, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00010101, 0b01010101, 0b01010101, 0b01010101,
    0b00000000, 0b00000000, 0b00010101, 0b01010101, 0b01010101, 0b00000000,
    0b00000000, 0b00000000, 0b00010101, 0b01010101, 0b01010101, 0b01010000,
    0b00000000, 0b00000000, 0b00010101, 0b01010101, 0b01010000, 0b00000000,
    0b00000000, 0b00000000, 0b00010101, 0b01000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00010101, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00010100, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00010100, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00010000, 0b00000000, 0b00000000, 0b00000000,
};

static const uint8_t pterodactyl_wing_up_bitmap[] =
{
    0b00000000, 0b00000000, 0b01000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b01010000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b01010100, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b01010000, 0b00010101, 0b00000000, 0b00000000, 0b00000000,
    0b00000001, 0b01010100, 0b00010101, 0b01000000, 0b00000000, 0b00000000,
    0b00000101, 0b01010100, 0b00010101, 0b01010000, 0b00000000, 0b00000000,
    0b00010101, 0b01010101, 0b00010101, 0b01010100, 0b00000000, 0b00000000,
    0b01010101, 0b01010101, 0b00010101, 0b01010101, 0b00000000, 0b00000000,
    0b00000000, 0b00000101, 0b01010101, 0b01010101, 0b01000000, 0b00000000,
    0b00000000, 0b00000001, 0b01010101, 0b01010101, 0b01010000, 0b00000000,
    0b00000000, 0b00000000, 0b01010101, 0b01010101, 0b01010101, 0b01010101,
    0b00000000, 0b00000000, 0b00010101, 0b01010101, 0b01010101, 0b00000000,
    0b00000000, 0b00000000, 0b00000101, 0b01010101, 0b01010101, 0b01010000,
    0b00000000, 0b00000000, 0b00000001, 0b01010101, 0b01010000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
};

static const uint16_t dinorun_game_over_sound_seq[] = {100, 30, 200, 15, 100, 10};


static const term_sprite_t cactus_small_default_data =
{
        .data = (uint8_t*)cactus_small_bitmap,
        .pallet = 3,
        .size_x = 9,
        .size_y = 14,
        .x = 170,
        .y = 62,

};

//56 16
static const term_sprite_t cactus_big_default_data =
{
        .data = (uint8_t*)cactus_big_bitmap,
        .pallet = 3,
        .size_x = 16,
        .size_y = 20,
        .x = 170,
        .y = 56,

};

static const term_sprite_t dino_idle_default_data =
{
        .data = (uint8_t*)dino_main_body_bitmap,
        .pallet = 3,
        .size_x = 20,
        .size_y = 13,
        .x = 4,
        .y = 58,
};

static const term_sprite_t dino_crouching_default_data =
{
        .data = (uint8_t*)dino_crouching_body_bitmap,
        .pallet = 3,
        .size_x = 30,
        .size_y = 10,
        .x = 2,
        .y = 64,
};


static const term_sprite_t pterodactyl_default_data =
{
        .data = (uint8_t*)pterodactyl_wing_up_bitmap,
        .pallet = 3,
        .size_x = 24,
        .size_y = 16,
        .x = 170,
        .y = 42,
};


static dino_run_game_state_t *gs;


void DINORUN_SetupTerrain(void)
{
    for(uint8_t i = 0; i < 5; i++)
    {
        uint8_t type = RNG_Get() % 4;
        gs->terrain_type_map[i] = type;

        gs->terrain[i].pallet = 3;
        gs->terrain[i].size_x = 40;
        gs->terrain[i].size_y = 8;
        gs->terrain[i].y = 72;
        gs->terrain[i].x = i * 40;

        switch(type)
        {
            case 0:
                gs->terrain[i].data = (uint8_t*)terrain_type_0;
            break;

            case 1:
                gs->terrain[i].data = (uint8_t*)terrain_type_1;
            break;

            case 2:
                gs->terrain[i].data = (uint8_t*)terrain_type_2;
            break;

            case 3:
                gs->terrain[i].data = (uint8_t*)terrain_type_3;
            break;
        }

        tGFX_SetSprite(&(gs->terrain[i]), i);
    }
}


void DINORUN_MoveTerrain(void)
{
    for(uint8_t i = 0; i < 5; i++)
    {
        gs->terrain[i].x -= gs->terrain_move_speed;
    }

    if(gs->terrain[0].x <= -39)
    {
        for(uint8_t i = 0; i < 4; i++)
        {
            gs->terrain_type_map[i] = gs->terrain_type_map[i + 1];
        }

        gs->terrain_type_map[4] = RNG_Get() % 4;

        for(uint8_t i = 0; i < 5; i++)
        {
            gs->terrain[i].x += 40;

            switch(gs->terrain_type_map[i])
            {
                case 0:
                    gs->terrain[i].data = (uint8_t*)terrain_type_0;
                break;

                case 1:
                    gs->terrain[i].data = (uint8_t*)terrain_type_1;
                break;

                case 2:
                    gs->terrain[i].data = (uint8_t*)terrain_type_2;
                break;

                case 3:
                    gs->terrain[i].data = (uint8_t*)terrain_type_3;
                break;
            }

        }
    }

    // tGFX_SetSprite updates look-up map of blocks with sprites. They won't change here so it isn't needed.

}


void DINORUN_ProcessGravity(void)
{
    if(gs->dino_state == DINO_STATE_IN_AIR)
    {
        gs->dino_acc += DINORUN_GRAVITY;
        gs->dino.y += gs->dino_acc / 4;
        gs->dino_legs.y += gs->dino_acc / 4;

        if(gs->dino.y > 58 || gs->dino_legs.y > 71)
        {
            gs->dino_state = DINO_STATE_IDLE;
            gs->dino_acc = 0;
            gs->dino.y = 58;
            gs->dino_legs.y = 71;
        }
    }
}


uint8_t DINORUN_CheckCollision(void)
{
    for(uint8_t i = 0; i < DINORUN_MAX_OBSTACLES; i++)
    {
        if(gs->obstacle[i].in_use == 1)
        {
            for(uint8_t j = 0; j < DINORUN_MAX_OBSTACLE_SPRITES; j++)
            {
                if(gs->obstacle[i].sprite[j].data != NULL)
                {
                    if((gs->dino.x + gs->dino.size_x < gs->obstacle[i].sprite[j].x) || (gs->dino.x > gs->obstacle[i].sprite[j].x + gs->obstacle[i].sprite[j].size_x - 6 /* Make tail non-colliding for player convenience */) || (gs->dino.y + gs->dino.size_y <  gs->obstacle[i].sprite[j].y) || (gs->dino.y > gs->obstacle[i].sprite[j].y + gs->obstacle[i].sprite[j].size_y))
                    {
                        if((gs->dino_legs.x + gs->dino_legs.size_x < gs->obstacle[i].sprite[j].x) || (gs->dino_legs.x > gs->obstacle[i].sprite[j].x + gs->obstacle[i].sprite[j].size_x) || (gs->dino_legs.y + gs->dino_legs.size_y <  gs->obstacle[i].sprite[j].y) || (gs->dino_legs.y > gs->obstacle[i].sprite[j].y + gs->obstacle[i].sprite[j].size_y))
                        {
                           continue;
                        }
                        else
                        {
                            return 1;
                        }

                    }
                    else
                    {
                        return 1;
                    }
                }
            }
        }
    }

    return 0;
}


void DINORUN_SpawnObstacle(void)
{
    uint8_t free_idx = 0xFF;

    // Find free slot
    for(uint8_t i = 0; i < DINORUN_MAX_OBSTACLES; i++)
    {
        if(gs->obstacle[i].in_use == 0)
        {
            free_idx = i;
            break;
        }
    }

    // Can't spawn obstacle. No free slots.
    if(free_idx == 0xFF) return;

    gs->obstacle[free_idx].type = RNG_GetRange(DINO_OBSTACLE_SINGLE_CACTUS_SMALL, DINO_OBSTACLE_PTERODACTYL_LOW);
    gs->obstacle[free_idx].in_use = 1;

    switch(gs->obstacle[free_idx].type)
    {
        case DINO_OBSTACLE_SINGLE_CACTUS_SMALL:
            gs->obstacle[free_idx].sprite[0] = cactus_small_default_data;
            gs->obstacle[free_idx].sprite[1].data = NULL;
            gs->obstacle[free_idx].sprite[2].data = NULL;
            break;

        case DINO_OBSTACLE_SINGLE_CACTUS_BIG:
            gs->obstacle[free_idx].sprite[0] = cactus_big_default_data;
            gs->obstacle[free_idx].sprite[1].data = NULL;
            gs->obstacle[free_idx].sprite[2].data = NULL;
            break;

        case DINO_OBSTACLE_DOUBLE_CACTUS_SMALL:
            gs->obstacle[free_idx].sprite[0] = cactus_small_default_data;
            gs->obstacle[free_idx].sprite[1] = cactus_small_default_data;
            gs->obstacle[free_idx].sprite[1].x += 10;
            gs->obstacle[free_idx].sprite[2].data = NULL;
            break;

        case DINO_OBSTACLE_DOUBLE_CACTUS_BIG:
            gs->obstacle[free_idx].sprite[0] = cactus_big_default_data;
            gs->obstacle[free_idx].sprite[1] = cactus_big_default_data;
            gs->obstacle[free_idx].sprite[1].x += 12;
            gs->obstacle[free_idx].sprite[2].data = NULL;
            break;

        case DINO_OBSTACLE_GROUP_1:
            gs->obstacle[free_idx].sprite[0] = cactus_small_default_data;
            gs->obstacle[free_idx].sprite[1] = cactus_big_default_data;
            gs->obstacle[free_idx].sprite[1].x += 9;
            gs->obstacle[free_idx].sprite[2] = cactus_small_default_data;
            gs->obstacle[free_idx].sprite[2].x += 9 + 16;
            break;

        case DINO_OBSTACLE_PTERODACTYL_HIGH:
            gs->obstacle[free_idx].sprite[0] = pterodactyl_default_data;
            gs->obstacle[free_idx].sprite[1].data = NULL;
            gs->obstacle[free_idx].sprite[2].data = NULL;
            break;

        case DINO_OBSTACLE_PTERODACTYL_LOW:
            gs->obstacle[free_idx].sprite[0] = pterodactyl_default_data;
            gs->obstacle[free_idx].sprite[0].y += 16;
            gs->obstacle[free_idx].sprite[1].data = NULL;
            gs->obstacle[free_idx].sprite[2].data = NULL;
            break;

    }

}

// Move obstacles and delete them if they aren't visible
void DINORUN_UpdateObstacles(void)
{
    uint8_t highest_idx = 0;

    for(uint8_t i = 0; i < DINORUN_MAX_OBSTACLES; i++)
    {
        if(gs->obstacle[i].in_use == 1)
        {
            for(uint8_t j = 0; j < DINORUN_MAX_OBSTACLE_SPRITES; j++)
            {
                if(gs->obstacle[i].sprite[j].data != NULL)
                {
                    highest_idx = j;

                    // Move with terrain
                    gs->obstacle[i].sprite[j].x -= gs->terrain_move_speed;
                    tGFX_SetSprite(&gs->obstacle[i].sprite[j], 5 + j + (i*3));
                }
            }
            // Despawn obstacle if it is not visible
            if(gs->obstacle[i].sprite[highest_idx].x < -26)
            {
                gs->obstacle[i].in_use = 0;
            }

        }
    }
}


void DINORUN_PrintScore(void)
{
    char int_buf[8];

    tGFX_SetCursor(9, 0);
    tGFX_Print("SCORE:00000", 8, 15);

    itoa(gs->score, int_buf, 10);
    tGFX_SetCursor(20 - strlen(int_buf), 0);
    tGFX_Print(int_buf, 8, 15);

}


void DINORUN_PterodactylAnimation(void)
{
    static uint8_t frame;

    frame++;

    if(frame > 4)
    {
        frame = 0;

        for(uint8_t i = 0; i < DINORUN_MAX_OBSTACLES; i++)
        {
            if(gs->obstacle[i].in_use == 1)
            {
                for(uint8_t j = 0; j < DINORUN_MAX_OBSTACLE_SPRITES; j++)
                {
                    if(gs->obstacle[i].sprite[j].data == pterodactyl_wing_down_bitmap)
                    {
                        gs->obstacle[i].sprite[j].data = (uint8_t*)pterodactyl_wing_up_bitmap;
                        gs->obstacle[i].sprite[j].y -= 2;

                    }
                    else if(gs->obstacle[i].sprite[j].data == pterodactyl_wing_up_bitmap)
                    {
                        gs->obstacle[i].sprite[j].data = (uint8_t*)pterodactyl_wing_down_bitmap;
                        gs->obstacle[i].sprite[j].y += 2;

                    }
                }
            }
        }
    }
}


void DINORUN_LegsAnimation(void)
{
    static uint8_t frame;

    frame++;

    if(frame > 4)
    {
        frame = 0;
        if(gs->dino_legs.data == (uint8_t*)dino_legs_step_1_bitmap)
        {
            gs->dino_legs.data = (uint8_t*)dino_legs_step_2_bitmap;
        }
        else
        {
            gs->dino_legs.data = (uint8_t*)dino_legs_step_1_bitmap;
        }
    }

    if(gs->dino_state == DINO_STATE_IN_AIR)
    {
        gs->dino_legs.data = (uint8_t*)dino_legs_idle_bitmap;
    }
}


void DINORUN_Init(void)
{
    // Setup RAM
    ENG_ClearCommonBuf();
    gs = ENG_CommonBufAlloc(sizeof(dino_run_game_state_t));

    // Clear background
    tGFX_Clear(15);

    // Load and setup sprites
    tGFX_SetSpritesCount(16);

    gs->dino = dino_idle_default_data;

    gs->dino_legs.data = (uint8_t*)dino_legs_idle_bitmap;
    gs->dino_legs.pallet = 3;
    gs->dino_legs.size_x = 8;
    gs->dino_legs.size_y = 7;
    gs->dino_legs.x = 10;
    gs->dino_legs.y = 71;

    DINORUN_SetupTerrain();

    // Setup game variables
    gs->terrain_move_speed = 3;
    gs->intial_lock = 1;
    gs->score = 0;
    gs->max_spawn_delay = DINORUN_START_MAX_SPAWN_DELAY;
    gs->min_spawn_delay = DINORUN_START_MIN_SPAWN_DELAY;

    ENG_UpdateInFrameCallback(&DINORUN_Main);

}


void DINORUN_Main(void)
{

    // Process input
    if(BTN_IsPressed(BTN_ACTION) && (gs->dino_state == DINO_STATE_IDLE) && !(gs->intial_lock))
    {
        gs->dino_acc = -36;
        gs->dino_state = DINO_STATE_IN_AIR;
        SND_PlayNow(350, 10);
    }

    // Game choice is confirmed by pressing action button. Lock it until user release it to not jump immediately after game start.
    if(!BTN_IsPressed(BTN_ACTION) && (gs->intial_lock))
    {
        gs->intial_lock = 0;
    }

    if(BTN_IsPressed(BTN_DOWN) && (gs->dino_state == DINO_STATE_IDLE))
    {
        gs->dino = dino_crouching_default_data;
        gs->dino_state = DINO_STATE_CROUCHING;
    }

    if(!BTN_IsPressed(BTN_DOWN) && (gs->dino_state == DINO_STATE_CROUCHING))
    {
        gs->dino = dino_idle_default_data;
        gs->dino_state = DINO_STATE_IDLE;
    }

    // Update score
    gs->score += 1;

    DINORUN_LegsAnimation();
    DINORUN_PterodactylAnimation();

    DINORUN_MoveTerrain();

    DINORUN_UpdateObstacles();

    // Spawn new obstacles and increase difficulty
    if(gs->score > gs->last_spawn_score + gs->spawn_delay)
    {
        DINORUN_SpawnObstacle();
        gs->last_spawn_score = gs->score;
        gs->spawn_delay = RNG_GetRange(gs->min_spawn_delay, gs->max_spawn_delay);

        // Interpolate delay based on score
        if(gs->score < DINORUN_SPAWN_DELAY_TARGET_SCORE)
        {
            gs->max_spawn_delay = ((DINORUN_SPAWN_DELAY_TARGET_SCORE - gs->score) * DINORUN_START_MAX_SPAWN_DELAY + gs->score * DINORUN_TARGET_MAX_SPAWN_DELAY) / DINORUN_SPAWN_DELAY_TARGET_SCORE;
            gs->min_spawn_delay = ((DINORUN_SPAWN_DELAY_TARGET_SCORE - gs->score) * DINORUN_START_MIN_SPAWN_DELAY + gs->score * DINORUN_TARGET_MIN_SPAWN_DELAY) / DINORUN_SPAWN_DELAY_TARGET_SCORE;
        }
        else
        {
            gs->max_spawn_delay = DINORUN_TARGET_MAX_SPAWN_DELAY;
            gs->min_spawn_delay = DINORUN_TARGET_MIN_SPAWN_DELAY;
        }
    }

    DINORUN_ProcessGravity();

    DINORUN_PrintScore();

    // Update dino sprites
    tGFX_SetSprite(&(gs->dino), 15);
    tGFX_SetSprite(&(gs->dino_legs), 14);

    // Game Over
    if(DINORUN_CheckCollision())
    {
        uint32_t highscore = HIGHSCORE_Read(HIGHSCORE_DINORUN);

        SND_PlaySequence((uint16_t*)dinorun_game_over_sound_seq, 2);

        // Clear used sprite slots
        for(uint8_t i = 0; i < 16; i++)
        {
            tGFX_ClearSprite(i);
        }

        // Update highscore
        if(highscore < gs->score)
        {
            HIGHSCORE_Write(HIGHSCORE_DINORUN, gs->score);
            highscore = gs->score;
        }

        GAMEOVER_Setup(GAMEOVER_TYPE_SCORE, gs->score, highscore, BTN_UP, NULL);
        GAMEOVER_Show();
    }
}
