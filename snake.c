#include "snake.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool snake_render(Snake* snake, SDL_Renderer* renderer)
{
    for (int i = 0; i < snake->length; i++) {
        Cell* cell = &snake->array[i];
        
        int res = SDL_SetRenderDrawColor(renderer, 0xff, 0, 0, 0xff);
        if (res == -1) return false;
        SDL_Rect dst = { cell->pos.x * CELL_SIZE, cell->pos.y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
        res = SDL_RenderFillRect(renderer, &dst);
        if (res == -1) return false;

        if (i == 0) {
            const int eye_size = 4;
            SDL_Rect eye1 = {.w = eye_size, .h = eye_size};
            SDL_Rect eye2 = {.w = eye_size, .h = eye_size};

            switch (cell->direction) {
            case UP:
            case DOWN:
                eye1.x = CELL_SIZE / 8 - eye_size / 2;
                eye1.y = CELL_SIZE / 2 - eye_size / 2;
                eye2.x = CELL_SIZE - (CELL_SIZE / 8 - eye_size / 2 + eye_size);
                eye2.y = CELL_SIZE / 2 - eye_size / 2;
                break;
            case LEFT:
            case RIGHT:
                eye1.x = CELL_SIZE / 2 - eye_size / 2;
                eye1.y = CELL_SIZE / 8 - eye_size / 2;
                eye2.x = CELL_SIZE / 2 - eye_size / 2;
                eye2.y = CELL_SIZE - (CELL_SIZE / 8 - eye_size / 2 + eye_size);
                break;
            }
            eye1.x += cell->pos.x * CELL_SIZE;
            eye1.y += cell->pos.y * CELL_SIZE;
            eye2.x += cell->pos.x * CELL_SIZE;
            eye2.y += cell->pos.y * CELL_SIZE;
            res = SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
            if (res == -1) return false;
            res = SDL_RenderFillRect(renderer, &eye1);
            if (res == -1) return false;
            res = SDL_RenderFillRect(renderer, &eye2);
            if (res == -1) return false;
        }
    }

    return true;
}

bool food_render(Food* food, SDL_Renderer* renderer)
{
    int res = SDL_SetRenderDrawColor(renderer, 0x2B, 0x4C, 0x6F, 0xff);
    if (res == -1)
        return false;
    SDL_Rect dst = { food->pos.x * CELL_SIZE, food->pos.y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
    res = SDL_RenderFillRect(renderer, &dst);
    return res == 0;
}

void snake_init(Snake* snake, SDL_Point start_pos, int length, Direction dir)
{
    snake->length = length;
    snake->bend_head_index = 0;
    snake->bend_tail_index = 0;

    SDL_Point pos = start_pos;
    for (int i = 0; i < length; i++) {
        Cell* cell = &snake->array[i];

        cell->pos = pos;
        cell->direction = dir;

        // going opposite here from normal movement because we init from head to tail
        switch (dir) {
        case UP:
            pos.y++;
            break;
        case DOWN:
            pos.y--;
            break;
        case LEFT:
            pos.x++;
            break;
        case RIGHT:
            pos.x--;
            break;
        }
    }
}

bool point_equals(SDL_Point a, SDL_Point b)
{
    return a.x == b.x && a.y == b.y;
}

void snake_head_change_dir(Snake* snake, Direction dir)
{
    Direction current_dir = snake->array[0].direction;
    if (current_dir == dir || 
    (current_dir == UP && dir == DOWN) || (current_dir == DOWN && dir == UP) || 
    (current_dir == LEFT && dir == RIGHT) || (current_dir == RIGHT && dir == LEFT))
        return;

    snake->array[0].direction = dir;

    snake->bends[snake->bend_head_index] = (Bend){ snake->array[0].pos, dir };
    snake->bend_head_index = (snake->bend_head_index + 1) % MAX_CELL_COUNT;
    if (snake->bend_head_index == snake->bend_tail_index) {
        snake->bend_tail_index = (snake->bend_tail_index + 1) % MAX_CELL_COUNT;
    }
}

static SDL_Point cell_move_predict(Cell *cell) 
{
    SDL_Point pos = cell->pos;
    switch (cell->direction) {
    case UP:
        pos.y--;
        if (pos.y < 0)
            pos.y = ROWS - 1;
        break;
    case DOWN:
        pos.y++;
        if (pos.y == ROWS)
            pos.y = 0;
        break;
    case LEFT:
        pos.x--;
        if (pos.x < 0)
            pos.x = COLUMNS - 1;
        break;
    case RIGHT:
        pos.x++;
        if (pos.x == COLUMNS)
            pos.x = 0;
        break;
    }

    return pos;
}

SDL_Point snake_move_predict(Snake *snake) 
{
    return cell_move_predict(&snake->array[0]);
}

void snake_move(Snake* snake, bool grow_tail)
{
    Cell prev_cell;
    for (int i = 0; i < snake->length; i++) {
        Cell *cell = &snake->array[i];

        prev_cell = *cell;
        cell->pos = cell_move_predict(cell);

        for (size_t j = snake->bend_tail_index; j != snake->bend_head_index; j = (j + 1) % MAX_CELL_COUNT) {
            if (point_equals(cell->pos, snake->bends[j].pos)) {
                if (i != 0)
                    cell->direction = snake->bends[j].direction;
                // if snake tail
                if (i == snake->length - 1 &&  !grow_tail) {
                    snake->bend_tail_index = (snake->bend_tail_index + 1) % MAX_CELL_COUNT;
                }
                break;
            }
        }
    }

    if (grow_tail) {
        snake->array[snake->length] = prev_cell;
        snake->length++;
    }
}

bool snake_check_collision(Snake* snake)
{
    for (int i = 1; i < snake->length; i++) {
        if (point_equals(snake->array[i].pos, snake->array[0].pos)) {
            return true;
        }
    }

    return false;
}

// returns true if there was a place to respawn
bool food_respawn(Food *food, Snake *snake) 
{
    static SDL_Point respawnable_positions[COLUMNS][ROWS];

    for (int column = 0; column < COLUMNS; column++) {
        for (int row = 0; row < ROWS; row++) {
            respawnable_positions[column][row].x = column;
            respawnable_positions[column][row].y = row;
        }   
    }

    int respawnable_positions_len = 0;

    SDL_Point *linear_array = (SDL_Point *)respawnable_positions;
    int snake_array_start_pos = 0;

    int last_not_in_snake_index = 0;
    for (int i = 0; i < ROWS * COLUMNS; i++) {
        bool found = false;
        for (int j = snake_array_start_pos; j < snake->length; j++) {
            if (point_equals(snake->array[j].pos, linear_array[i])) {
                found = true;
                snake_array_start_pos++;
                break;
            }
        }

        if (!found) {
            linear_array[last_not_in_snake_index] = linear_array[i];
            last_not_in_snake_index++;
        }
    }

    respawnable_positions_len = last_not_in_snake_index;
    if (respawnable_positions_len == 0) return false;

    SDL_Point pos = linear_array[rand() % respawnable_positions_len];
    food->pos = pos;

    return true;
}