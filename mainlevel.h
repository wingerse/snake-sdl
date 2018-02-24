#pragma once

#include <SDL.h>
#include <stdbool.h>
#include "snake.h"

typedef struct Game Game;

typedef struct MainLevel {
    Game *game;
    int score;
    Uint32 timer;
    Snake snake;
    Food food;
    Direction dir;
} MainLevel;

void mainlevel_reset(void *level);
bool mainlevel_update(void *level, double delta);
bool mainlevel_draw(void *level);