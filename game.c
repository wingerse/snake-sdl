#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "game.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "startmenu.h"
#include "mainlevel.h"
#include "gameover.h"

SDL_Rect number_font_rects[11] = {
    { 0, 0, 10, 13 }, { 11, 0, 6, 13 }, { 18, 0, 8, 13 }, { 28, 0, 10, 13 },
    { 40, 0, 9, 13 }, { 51, 0, 8, 13 }, { 61, 0, 8, 13 }, { 70, 0, 8, 13 },
    { 80, 0, 8, 13 }, { 88, 0, 9, 13 }, { 97, 0, 9, 13 }
};

SDL_Rect font_rects[75] = {
    {0, 0, 12, 16}, {12, 0, 10, 16}, {22, 0, 10, 16}, {32, 0, 13, 16}, {45, 0, 9, 16}, {54, 0, 11, 16}, 
    {65, 0, 11, 16}, {76, 0, 12, 16}, {88, 0, 6, 16}, {94, 0, 6, 16}, {100, 0, 11, 16}, {111, 0, 8, 16}, 
    {119, 0, 13, 16}, {132, 0, 11, 16}, {143, 0, 12, 16}, {155, 0, 11, 16}, {0, 16, 12, 16}, {12, 16, 13, 16}, 
    {25, 16, 10, 16}, {35, 16, 13, 16}, {48, 16, 11, 16}, {59, 16, 16, 16}, {75, 16, 18, 16}, {93, 16, 13, 16}, 
    {106, 16, 13, 16}, {119, 16, 12, 16}, {131, 16, 6, 16}, {140, 16, 7, 16}, {147, 16, 5, 16}, {1, 32, 8, 16}, 
    {11, 32, 7, 16}, {20, 32, 7, 16}, {28, 32, 10, 16}, {39, 32, 8, 16}, {48, 32, 8, 16}, {57, 32, 8, 16}, 
    {67, 32, 8, 16}, {77, 32, 4, 16}, {82, 32, 4, 16}, {88, 32, 8, 16}, {97, 32, 6, 16}, {105, 32, 9, 16}, 
    {116, 32, 8, 16}, {125, 32, 9, 16}, {135, 32, 9, 16}, {146, 32, 9, 16}, {156, 32, 8, 16}, {0, 48, 8, 16}, 
    {8, 48, 10, 16}, {19, 48, 8, 16}, {29, 48, 10, 16}, {42, 48, 14, 16}, {57, 48, 10, 16}, {69, 48, 9, 16}, 
    {80, 48, 9, 16}, {91, 48, 4, 16}, {97, 48, 3, 16}, {102, 48, 5, 16}, {109, 48, 8, 16}, {119, 48, 6, 16}, 
    {127, 48, 4, 16}, {131, 48, 8, 16}, {0, 64, 9, 16}, {12, 64, 5, 16}, {18, 64, 10, 16}, {29, 64, 9, 16}, 
    {39, 64, 10, 16}, {52, 64, 8, 16}, {61, 64, 9, 16}, {70, 64, 9, 16}, {79, 64, 9, 16}, {89, 64, 11, 16}, 
    {117, 64, 5, 16}, {122, 64, 7, 16}, {157, 64, 4, 16}
};

static SDL_Texture* load_texture(const char* path, SDL_Renderer* renderer)
{
    SDL_Surface* surface = IMG_Load(path);
    if (!surface)
        return NULL;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

static bool init(Game* game)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
        return false;
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
        return false;
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
        return false;

    game->window = SDL_CreateWindow("Snake",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN);

    if (!game->window)
        return false;

    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);
    if (!game->renderer)
        return false;

    game->font_texture = load_texture("font-acilis.png", game->renderer);
    if (!game->font_texture)
        return false;

    game->number_font_texture = load_texture("scoreFont.png", game->renderer);
    if (!game->number_font_texture)
        return false;

    game->eat_sound = Mix_LoadWAV("eat.wav");
    if (!game->eat_sound)
        return false;

    game->gameover_sound = Mix_LoadWAV("gameover.wav");
    if (!game->gameover_sound)
        return false;
    
    StartMenu *startmenu = malloc(sizeof(*startmenu));
    if (!startmenu) {
        SDL_SetError("malloc failure");
        return false;
    }
    MainLevel *mainlevel = malloc(sizeof(*mainlevel));
    if (!mainlevel) {
        SDL_SetError("malloc failure");
        return false;
    }
    GameOver *gameover = malloc(sizeof(*gameover));
    if (!gameover) {
        SDL_SetError("malloc failure");
        return false;
    }

    startmenu->game = game;
    mainlevel->game = game;
    gameover->game = game;

    game->startmenu = (LevelInterface){startmenu, startmenu_update, startmenu_draw};
    game->mainlevel = (LevelInterface){mainlevel, mainlevel_update, mainlevel_draw};
    game->gameover = (LevelInterface){gameover, gameover_update, gameover_draw};
    game->level = game->startmenu;
    game->quit = false;

    return true;
}

static void handle_events(Game *game) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            game->quit = true;
            break;
        }
    }
}

bool game_run(Game* game)
{
    if (!init(game))
        return false;
    game->prev_tick = SDL_GetTicks();
    SDL_SetRenderDrawColor(game->renderer, 0xff, 0xff, 0xff, 0xff);
    while (!game->quit) {
        double prev_tick = game->prev_tick;
        Uint32 now = SDL_GetTicks();
        double delta = now - prev_tick;
        if (delta >= TICK_DURATION) {
            game->prev_tick = now;
            handle_events(game);
            LevelInterface current_level = game->level;
            if (!game->level.update_func(game->level.level, delta)) return false;
            // if level has been changed, it's update func has to be called before draw func. so skip this until next frame
            if (game->level.level == current_level.level) {
                if (!game->level.draw_func(game->level.level)) return false;
            }
            SDL_RenderPresent(game->renderer);
        } else {
            Uint32 sleep_time = (prev_tick + TICK_DURATION) - (delta + prev_tick);
            SDL_Delay(sleep_time);
        }
    }

    return true;
}

void game_destroy(Game *game) 
{
    free(game->startmenu.level);
    free(game->mainlevel.level);
    free(game->gameover.level);
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    Mix_FreeChunk(game->eat_sound);
    Mix_FreeChunk(game->gameover_sound);

    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}

static SDL_Rect *get_char_rect(const char c) 
{
    SDL_Rect *r;
    if (isupper(c)) {
        r = &font_rects[c - 65];
    } else if (islower(c)) {
        r = &font_rects[c - 97 + 29];
    } else if (isdigit(c)) {
        r = &font_rects[c - 48 + 62];
    } else {
        switch (c) {
        case '(': r = &font_rects[26]; break;
        case ')': r = &font_rects[27]; break;
        case ':': r = &font_rects[28]; break;
        case '.': r = &font_rects[55]; break;
        case ',': r = &font_rects[56]; break;
        case ';': r = &font_rects[57]; break;
        case '?': r = &font_rects[58]; break;
        case '"': r = &font_rects[59]; break;
        case '\'': r = &font_rects[60]; break;
        case '-': r = &font_rects[61]; break;
        case '<': r = &font_rects[72]; break;
        case '>': r = &font_rects[73]; break;
        default : r = &font_rects[74]; break;
        }
    }

    return r;
}

bool game_draw_text(Game* game, const char *text, SDL_Point pos, int size_factor)
{
    int x = 0;
    const char *p = text;
    for (;*p != '\0'; p++) {
        SDL_Rect* r = get_char_rect(*p);
        SDL_Rect dst = { pos.x + x, pos.y, r->w * size_factor, r->h * size_factor };

        int res = SDL_RenderCopy(game->renderer, game->font_texture, r, &dst);
        if (res == -1) return false;
        x += r->w * size_factor;
    }

    return true;
}

int get_text_width(const char *text, int size_factor)
{
    int x = 0;
    const char *p = text;
    for (;*p != '\0'; p++) {
        SDL_Rect* r = get_char_rect(*p);
        x += r->w * size_factor;
    }

    return x;
}

bool game_draw_number(Game* game, Uint32 num, SDL_Point pos, int size_factor)
{
    sprintf(game->charbuf, "%" PRIu32, num);
    char* p = game->charbuf;

    int x = 0;
    for (;*p != '\0'; p++) {
        SDL_Rect* r;
        if (isdigit(*p))
            r = &number_font_rects[*p - 48];
        else
            r = &number_font_rects[10];

        SDL_Rect dst = { pos.x + x, pos.y, r->w * size_factor, r->h * size_factor };

        int res = SDL_RenderCopy(game->renderer, game->number_font_texture, r, &dst);
        if (res == -1) return false;
        x += r->w * size_factor;
    }

    return true;
}
