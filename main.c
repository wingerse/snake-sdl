#include <SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include "game.h"

int main(int argc, char* args[])
{
    Game game;
    if (!game_run(&game)) {
        fprintf(stderr, "error : %s", SDL_GetError());
    }
    game_destroy(&game);
    
    return 0;
}
