#include <SDL.h>
#include "game.h"
#include "mainlevel.h"
#include "snake.h"
#include <stdbool.h>
#include "constants.h"
#include <stdio.h>
#include <SDL_mixer.h>

void mainlevel_reset(void *level) 
{
    MainLevel *lvl = level;
    lvl->timer = 0;
    lvl->score = 0;
    lvl->food.pos = (SDL_Point){0, 0};
    lvl->dir = RIGHT;
    snake_init(&lvl->snake, (SDL_Point){30, 10}, 5, lvl->dir);
    food_respawn(&lvl->food, &lvl->snake);
}

bool mainlevel_update(void *level, double delta)
{
    MainLevel *lvl = level;
    lvl->timer += delta;

    const Uint8 *keystate = SDL_GetKeyboardState(NULL);

    if (keystate[SDL_SCANCODE_UP]) lvl->dir = UP;
    else if (keystate[SDL_SCANCODE_DOWN]) lvl->dir = DOWN;
    else if (keystate[SDL_SCANCODE_LEFT]) lvl->dir = LEFT;
    else if (keystate[SDL_SCANCODE_RIGHT]) lvl->dir = RIGHT;

    if (lvl->timer >= GRID_TICK_MS) {
        snake_head_change_dir(&lvl->snake, lvl->dir);
        SDL_Point pos = snake_move_predict(&lvl->snake);
        bool ate_food = false;
        if (point_equals(pos, lvl->food.pos)) {
            Mix_PlayChannel(-1, lvl->game->eat_sound, 0);
            food_respawn(&lvl->food, &lvl->snake);
            lvl->score++;
            ate_food = true;
        }
        snake_move(&lvl->snake, ate_food);
        if (snake_check_collision(&lvl->snake)) {
            Mix_PlayChannel(-1, lvl->game->gameover_sound, 0);
            lvl->game->level = lvl->game->startmenu;
        }

        lvl->timer = 0;
    }
    return true;
}

bool mainlevel_draw(void *level)
{
    MainLevel *lvl = level;
    int res = SDL_SetRenderDrawColor(lvl->game->renderer, 0x00, 0x00, 0x00, 0xff);
    if (res == -1) return false;
    res = SDL_RenderClear(lvl->game->renderer);
    if (res == -1) return false;
    if (!game_draw_text(lvl->game, "Snake", (SDL_Point){(WINDOW_WIDTH / 2) - (get_text_width("Snake", 2) / 2), 10}, 2)) 
        return false;
    if (!game_draw_number(lvl->game, lvl->score, (SDL_Point){10, 10}, 2)) return false;

    if (!snake_render(&lvl->snake, lvl->game->renderer)) return false;
    if (!food_render(&lvl->food, lvl->game->renderer)) return false;
    return true;
}