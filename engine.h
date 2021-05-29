#ifndef ENGINE_H
#define ENGINE_H

#define USE_DOUBLE_BUFFER // Wether to use seperate buffer to store rendered data and only then display it
#define SECONDS_PER_FRAME 1/60
//#define DEBUG						// Enable some debuging features

// Double buffer: renderer draws to this buffer, and then on vSync copies it to the screen
// Although this may reduce framerate a bit, it can save us from displaying unfinished renders.
// So we can use 'buffer' in code, while actually having control over what exact buffer is used

#include <stdint.h>
#include "device.h"
#include "fixedpoint.h"

#ifdef USE_DOUBLE_BUFFER
	extern uint32_t buffer[SCREEN_W * SCREEN_H];
#else
    #define buffer unpack_idx4_p32
#endif

#ifdef DEBUG
	#include <stdio.h>	// For testing only: todo remove in final version
	#include <time.h>
	#define MAX_FPS 60	// Actual FPS may be lower, but not higher
#endif

typedef struct {
	fp32_t pos_x;
	fp32_t pos_y;
	fp32_t dir_x;
	fp32_t dir_y;
	fp32_t plane_x;
	fp32_t plane_y;

	#ifdef DEBUG
		double time;    // time of current frame
		double oldTime; // time of previous frame
	#endif
} camera_t;

// Sprite objects in-game
typedef struct
{
	fp32_t x;
	fp32_t y;
	uint8_t visible; 
    unsigned anim_index;
	const uint8_t* textures;
} sprite_t;

typedef struct
{
	fp32_t x;
	fp32_t y;
	int texture;
} enemy_t;

// Time management
extern unsigned long long int frame_count;
float time();	// Returns current time in seconds since game started

#endif