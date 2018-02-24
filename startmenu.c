#include <SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include "game.h"
#include "startmenu.h"
#include "mainlevel.h"
#include <SDL_mixer.h>

bool startmenu_update(void *level, double delta)
{
    StartMenu *lvl = level;
    const Uint8 *keyboard_state = SDL_GetKeyboardState(NULL);
    if (keyboard_state[SDL_SCANCODE_RETURN]) {
        Mix_PlayChannel(-1, lvl->game->eat_sound, 0);
        mainlevel_reset(lvl->game->mainlevel.level);
        lvl->game->level = lvl->game->mainlevel;
    }
    return true;
}

bool startmenu_draw(void *level) 
{
    StartMenu *lvl = level;
    int res = SDL_SetRenderDrawColor(lvl->game->renderer, 0x00, 0x00, 0x00, 0xff);
    if (res == -1) return false;
    res = SDL_RenderClear(lvl->game->renderer);
    if (res == -1) return false;

    const char *s = "Press Enter to start the game!";
    return game_draw_text(lvl->game, s, (SDL_Point){(WINDOW_WIDTH / 2) - get_text_width(s, 2) / 2, WINDOW_HEIGHT / 2 - 16}, 2);
}

