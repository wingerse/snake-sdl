#pragma once

#include <SDL.h>
#include "constants.h"
#include "stdbool.h"

typedef enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

typedef struct Cell {
    SDL_Point pos;
    Direction direction;
} Cell;

typedef struct Bend {
    SDL_Point pos;
    Direction direction;
} Bend;

typedef struct Snake {
    int length;
    Cell array[ROWS * COLUMNS];

    // use a circular buffer queue
    Bend bends[ROWS * COLUMNS];
    size_t bend_head_index;
    size_t bend_tail_index;
} Snake;


bool snake_render(Snake *snake, SDL_Renderer *renderer);

typedef struct Food {
    SDL_Point pos;
    int points;
} Food;

bool food_render(Food *food, SDL_Renderer *renderer);

bool point_equals(SDL_Point a, SDL_Point b);

void snake_init(Snake *snake, SDL_Point start_pos, int length, Direction dir);
void snake_head_change_dir(Snake *snake, Direction dir);
SDL_Point snake_move_predict(Snake*snake);
void snake_move(Snake *snake, bool ate_food);
bool snake_check_collision(Snake *snake);
void snake_grow_tail(Snake *snake);
bool food_respawn(Food *food, Snake *snake);