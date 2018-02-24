#include <stdbool.h>
#include <SDL.h>
#include "collision.h"

static bool proj_overlap(int p1, int p2, int p3, int p4) 
{
    if (p1 <= p3) {
        return !(p1 + p2 < p3);
    } else {
        return !(p3 + p4 < p1);
    }
}

bool rect_collides(SDL_Rect *a, SDL_Rect *b) 
{
    int a_y1 = a->y;
    int a_y2 = a->y + a->h;
    int b_y1 = b->y; 
    int b_y2 = b->y + b->h;
    
    int a_x1 = a->x;
    int a_x2 = a->x + a->w;
    int b_x1 = b->x;
    int b_x2 = b->x + b->w;

    return proj_overlap(a_y1, a_y2, b_y1, b_y2) && proj_overlap(a_x1, a_x2, b_x1, b_x2);
}