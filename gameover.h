#pragma once

#include <SDL.h>
#include <stdbool.h>

typedef struct Game Game;

typedef struct GameOver {
    Game *game;
} GameOver;

bool gameover_update(void *level, double delta);
bool gameover_draw(void *level);