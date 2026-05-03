#ifndef DAVE_H
#define DAVE_H

#include <SDL.h>
#include <stdbool.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define TILE_SIZE 16
#define LEVEL_WIDTH 20
#define LEVEL_HEIGHT 12
#define PLAYER_SPEED 2
#define JUMP_FORCE 8
#define GRAVITY 0.4f

typedef enum {
    TILE_EMPTY,
    TILE_WALL,
    TILE_PLATFORM,
    TILE_LADDER,
    TILE_DOOR,
    TILE_TROPHY,
    TILE_GEM,
    TILE_SPIKE,
    TILE_ENEMY
} TileType;

typedef enum {
    ENEMY_PATROL,
    ENEMY_FLY
} EnemyType;

typedef struct {
    float x, y;
    float vx, vy;
    int width, height;
    bool onGround;
    bool alive;
    int score;
    bool hasTrophy;
    int lives;
} Player;

typedef struct {
    float x, y;
    float vx;
    int width, height;
    EnemyType type;
    bool alive;
    int startX, endX;
    int dir;
} Enemy;

typedef struct {
    int x, y;
    TileType type;
    bool collected;
} Collectible;


