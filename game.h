#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <stdbool.h>
#include "startmenu.h"
#include "mainlevel.h"
#include "gameover.h"
#include "constants.h"

typedef struct LevelInterface {
    void *level;
    bool (*update_func)(void *level, double delta);
    bool (*draw_func)(void *level);
} LevelInterface;

typedef struct Game {
    SDL_Window *window;
    SDL_Renderer *renderer;

    SDL_Texture *font_texture;
    SDL_Texture *number_font_texture;

    Mix_Chunk *eat_sound;
    Mix_Chunk *gameover_sound;

    LevelInterface startmenu;
    LevelInterface mainlevel;
    LevelInterface gameover;
    LevelInterface level;

    bool quit;
    double prev_tick;

    char charbuf[10];
} Game;

int get_text_width(const char *text, int size_factor);
bool game_run(Game *game);
bool game_draw_text(Game* game, const char *text, SDL_Point pos, int size_factor);
bool game_draw_number(Game* game, Uint32 num, SDL_Point pos, int size_factor);
void game_destroy(Game *game);