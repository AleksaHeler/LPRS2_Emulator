#include "player.h"

camera_t player_camera;

void player_init() {
    player_camera.pos_x = FP32F(22.5), player_camera.pos_y = FP32F(12.5); // player x and y start position
    player_camera.dir_x = FP32(-1), player_camera.dir_y = FP32(0);       // initial player direction vector

    // Camera plane is perpendicular to the direction, but we 
    //  can change the length of it. The ratio between the length
    //  of the direction and the camera plane determinates the FOV.
    //  FOV is 2 * atan(0.66/1.0)=66°, which is perfect for a first person shooter game
    player_camera.plane_x = FP32F(0), player_camera.plane_y = FP32F(0.66); // the 2d raycaster version of camera plane
}

// Registering inputs in main menu
int player_menu() {
    if(joypad.a) return 1;
    if(joypad.b) return -1;
    return 0;
}

void player_update() {
    /////////////////////////////////////
    // Poll controls:
    // The speed modifiers use frameTime, and a constant value, to 
    // determinate the speed of the moving and rotating of the input 
    // keys. Thanks to using the frameTime, we can make sure that the 
    // moving and rotating speed is independent of the processor speed
    //double moveSpeed = frameTime * 5.0; //the constant value is in squares/second
    //double rotSpeed = frameTime * 3.0;  //the constant value is in radians/second
    
    fp32_t move_speed = FP32F(5.0/60.0);
    fp32_t rotation_speed = FP32F(3.0/60.0);

    fp32_t player_width = FP32F(0.4);

    // Temp variables for better code readability
    fp32_t pos_x = player_camera.pos_x;
    fp32_t pos_y = player_camera.pos_y;
    fp32_t dir_x = player_camera.dir_x;
    fp32_t dir_y = player_camera.dir_y;
    fp32_t plane_x = player_camera.plane_x;
    fp32_t plane_y = player_camera.plane_y;

    // Move forward if no wall in front of the player
    if(joypad.up) {
        if(world_map[fp32_to_int(pos_x + fp32_mul(dir_x, move_speed) + fp32_mul(dir_x, player_width))][fp32_to_int(pos_y)] == 0) 
            pos_x += fp32_mul(dir_x, move_speed);
        if(world_map[fp32_to_int(pos_x)][fp32_to_int(pos_y + fp32_mul(dir_y, move_speed) + fp32_mul(dir_y, player_width))] == 0) 
            pos_y += fp32_mul(dir_y, move_speed);
    }
    // Move backwards if no wall behind the player
    if(joypad.down) {
        if(world_map[fp32_to_int(pos_x - fp32_mul(dir_x, move_speed) - fp32_mul(dir_x, player_width))][fp32_to_int(pos_y)] == 0) 
            pos_x -= fp32_mul(dir_x, move_speed);
        if(world_map[fp32_to_int(pos_x)][fp32_to_int(pos_y - fp32_mul(dir_y, move_speed) - fp32_mul(dir_y, player_width))] == 0) 
            pos_y -= fp32_mul(dir_y, move_speed);
    }

    // Rotate to the right
    if(joypad.right) {
        // Both camera direction and camera plane must be rotated
        // Rotating vectors by multiplying it with rotation matrix:
        //   [ cos(a) -sin(a) ]
        //   [ sin(a)  cos(a) ]
        fp32_t old_dir_x = dir_x;
        dir_x = fp32_mul(dir_x, my_cos(-rotation_speed)) - fp32_mul(dir_y, my_sin(-rotation_speed));
        dir_y = fp32_mul(old_dir_x, my_sin(-rotation_speed)) + fp32_mul(dir_y, my_cos(-rotation_speed));
        fp32_t old_plane_x = plane_x;
        plane_x = fp32_mul(plane_x, my_cos(-rotation_speed)) - fp32_mul(plane_y, my_sin(-rotation_speed));
        plane_y = fp32_mul(old_plane_x, my_sin(-rotation_speed)) + fp32_mul(plane_y, my_cos(-rotation_speed));
    }
    // Rotate to the left
    if(joypad.left) {
        // Both camera direction and camera plane must be rotated
        fp32_t old_dir_x = dir_x;
        dir_x = fp32_mul(dir_x, my_cos(rotation_speed)) - fp32_mul(dir_y, my_sin(rotation_speed));
        dir_y = fp32_mul(old_dir_x, my_sin(rotation_speed)) + fp32_mul(dir_y, my_cos(rotation_speed));
        fp32_t old_plane_x = plane_x;
        plane_x = fp32_mul(plane_x, my_cos(rotation_speed)) - fp32_mul(plane_y, my_sin(rotation_speed));
        plane_y = fp32_mul(old_plane_x, my_sin(rotation_speed)) + fp32_mul(plane_y, my_cos(rotation_speed));
    }

    // Animate enemies (maybe move to another place?)
    // TODO: make movement speed constant with time and not percentage of distance
    for(int i = 0; i < num_enemies; i++){
        // Distances from enemy struct, squared so we dont have to use sqrt()
        fp32_t enemy_to_player_dist = fp32_mul(enemies_data[i].dist_to_player, enemies_data[i].dist_to_player);
        fp32_t enemy_max_shot_distance = fp32_mul(enemies_data[i].max_shot_distance, enemies_data[i].max_shot_distance);
        fp32_t enemy_view_distance = fp32_mul(enemies_data[i].view_distance, enemies_data[i].view_distance);

        /////////////// Calculate distance to player ///////////////
        // {dist_x, dist_y} is a vector pointing from enemy to player
        fp32_t dist_x = player_camera.pos_x - enemies_data[i].sprite->x;
        fp32_t dist_y = player_camera.pos_y - enemies_data[i].sprite->y;
        fp32_t dist_to_player = fp32_mul(dist_x, dist_x) + fp32_mul(dist_y, dist_y);

        /////////////// Use DDA alg. to find wall we are looking at ///////////////
        int hit = 0;
        int map_x = fp32_to_int(enemies_data[i].sprite->x);
        int map_y = fp32_to_int(enemies_data[i].sprite->y);
        int side;
        int step_x;
        int step_y;
        fp32_t side_dist_x;
        fp32_t side_dist_y;
        fp32_t delta_dist_x = fp32_abs(fp32_div(FP32(1), dist_x));
        fp32_t delta_dist_y = fp32_abs(fp32_div(FP32(1), dist_y));
        if(dist_x < 0) {
            step_x = -1;
            side_dist_x = fp32_mul(enemies_data[i].sprite->x - fp32_from_int(map_x), delta_dist_x);
        } else {
            step_x = 1;
            side_dist_x = fp32_mul(fp32_from_int(map_x + 1) - enemies_data[i].sprite->x, delta_dist_x);
        }
        if(dist_y < 0) {
            step_y = -1;
            side_dist_y = fp32_mul(enemies_data[i].sprite->y - fp32_from_int(map_y), delta_dist_y);
        } else {
            step_y = 1;
            side_dist_y = fp32_mul(fp32_from_int(map_y + 1) - enemies_data[i].sprite->y, delta_dist_y);
        }
        fp32_t dist_to_wall;
        dda(&hit, &map_x, &map_y, &step_x, &step_y, &side_dist_x, &side_dist_y, &delta_dist_x, &delta_dist_y, &side, &dist_to_wall);
        
        /////////////// Compare coordinates of wall the enemy is looking at when looking at player and 
        /////////////// player coordinates, in case the player is in front of the wall we are looking at player
        int player_map_x = fp32_to_int(player_camera.pos_x);
        int player_map_y = fp32_to_int(player_camera.pos_y);
        int player_to_wall_vector_x = map_x - player_map_x;
        int player_to_wall_vector_y = map_y - player_map_y;
        int looking_at_player = 1;  // Assume we are looking at player
        if(dist_x < 0 && player_to_wall_vector_x > 0)
            looking_at_player = 0;
        if(dist_x > 0 && player_to_wall_vector_x < 0)
            looking_at_player = 0;
        if(dist_y < 0 && player_to_wall_vector_y > 0)
            looking_at_player = 0;
        if(dist_y > 0 && player_to_wall_vector_y < 0)
            looking_at_player = 0;

        /////////////// If we are looking at the player (and not a wall) 
        if(looking_at_player && dist_to_player < enemy_view_distance){
            /////////////// If distance is in some range (a to b) -> move towards player until distance is 'a'
            if(dist_to_player - FP32F(0.1) > enemy_to_player_dist){
                enemies_data[i].sprite->x += fp32_mul(dist_x, FP32F(0.002));
                enemies_data[i].sprite->y += fp32_mul(dist_y, FP32F(0.002));
            }

            /////////////// If distance is too close (less than 'a') -> move backwards if there is no wall there
            if (dist_to_player + FP32F(0.1) < enemy_to_player_dist) {
                // check if there is a wall behind this position
                fp32_t new_pos_x = enemies_data[i].sprite->x - fp32_mul(dist_x, FP32F(0.015));
                fp32_t new_pos_y = enemies_data[i].sprite->y - fp32_mul(dist_y, FP32F(0.015));
                int new_pos_x_int = fp32_to_int(new_pos_x);
                int new_pos_y_int = fp32_to_int(new_pos_y);
                if(world_map[new_pos_x_int][new_pos_y_int] == 0){
                    enemies_data[i].sprite->x = new_pos_x;
                    enemies_data[i].sprite->y = new_pos_y;
                }
            }

            /////////////// If distance to player is in shooting range (a to b) -> shoot at player on regular interval
            if (dist_to_player > enemy_to_player_dist && dist_to_player < enemy_max_shot_distance) {
                // TODO: implement HP system and shooting system
            }
        }
    }

    // Temp variables for better code readability
    player_camera.pos_x = pos_x;
    player_camera.pos_y = pos_y;
    player_camera.dir_x = dir_x;
    player_camera.dir_y = dir_y;
    player_camera.plane_x = plane_x;
    player_camera.plane_y = plane_y;
}