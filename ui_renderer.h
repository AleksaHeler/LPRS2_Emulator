#ifndef UI_RENDERER_H
#define UI_RENDERER_H

#include "game_data.h"
#include "sprites_renderer.h"

///////////////////////////////////////////////////////////////////////////////
// Settings / parameters:

///////////////////////////////////////////////////////////////////////////////
// Sprite list for drawing:

///////////////////////////////////////////////////////////////////////////////
// Functions:
void draw_num(uint16_t number, uint16_t x, uint16_t y, uint8_t color);
void draw_hud(player_t *player);

#endif // UI_RENDERER_H