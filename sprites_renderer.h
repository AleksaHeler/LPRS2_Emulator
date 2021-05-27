#ifndef SPRITES_H
#define SPRITES_H

#include "game_data.h"
#include "engine.h"
#include "fmath.h"

///////////////////////////////////////////////////////////////////////////////
// Functions:
void draw_sprite(uint32_t*, uint16_t, uint16_t, uint16_t, uint16_t);
void swap(int*, fp32_t*, int*, fp32_t*);
int partition(int*, fp32_t*, int, int);
void quick_sort(int*, fp32_t*, int, int);
void sort_sprites(int*, fp32_t*, camera_t*, int);


#endif // SPRITES_H