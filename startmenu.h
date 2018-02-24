#pragma once

#include <stdbool.h>
#include <SDL.h>

typedef struct Game Game;

typedef struct StartMenu {
    Game *game;
} StartMenu;

bool startmenu_update(void *level, double delta);
bool startmenu_draw(void *level);