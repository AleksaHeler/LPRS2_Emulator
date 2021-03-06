#ifndef PLAYER_H
#define PLAYER_H

#include "engine.h"
#include "game_data.h"
#include "fmath.h"
#include "raycast_renderer.h"
#include "enemy.h"

extern player_t player;

void player_init();
int player_menu();
int player_update();
void player_take_damage(uint8_t damage);

#endif