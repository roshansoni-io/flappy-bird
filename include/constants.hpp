#ifndef Constants
#define Constants

#include <raylib.h>

//  Scaling 
const float SCALE = 1.5f;

//  Window 
const int SCREEN_WIDTH = (int)(288 * SCALE);
const int SCREEN_HEIGHT = (int)(512 * SCALE);

//  Bird 
const float GRAVITY = 1000.0f * SCALE;
const float BIRD_JUMP_FORCE = -300.0f * SCALE;
const float BIRD_SCALE = 1.5f;
const float BIRD_COLLISION_SCALE = 0.7f;

//  Pipes 
const float PIPE_SPEED = 150.0f * SCALE;
const float PIPE_SPAWN_DELAY = 1.5f;
const float PIPE_GAP_SIZE = 120.0f * SCALE;

//  Background 
const float BG_SCROLL_SPEED = 30.0f * SCALE;
const float BG_SCALE = 1.5f;

enum GameState {
    MENU,
    PLAYING,
    GAME_OVER
};

#endif 
