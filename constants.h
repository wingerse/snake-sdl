#pragma once

#define CELL_SIZE 20
#define ROWS 24
#define COLUMNS 32
#define MAX_CELL_COUNT (ROWS * COLUMNS)

#define WINDOW_WIDTH (COLUMNS * CELL_SIZE)
#define WINDOW_HEIGHT (ROWS * CELL_SIZE)

#define FPS 60
#define TICK_DURATION (1000.0 / FPS)

#define START_LENGTH 5

#define GRID_TICK_MS 100