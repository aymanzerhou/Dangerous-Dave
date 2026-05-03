#include "dave.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const char* LEVEL_DATA[3] = {
    "####################"
    "#..................#"
    "#...T..............#"
    "#..###..........G..#"
    "#............##....#"
    "#.......##.........#"
    "#..................#"
    "#....P.......E...D#"
    "#..####....####...##"
    "#..................#"
    "#..................#"
    "####################",

    "####################"
    "#G.................#"
    "###................#"
    "#....E...G...E.....#"
    "#...####...####....#"
    "#..................#"
    "#.......T..........#"
    "#......###.........#"
    "#..................#"
    "#..P...........D...#"
    "#..####.....########"
    "####################",

    "####################"
    "#T..........E......#"
    "###........###.....#"
    "#.......G..........#"
    "#.....####....E....#"
    "#G.........####....#"
    "###................#"
    "#.......E.......G..#"
    "#.....####....####.#"
    "#..P...............#"
    "#.####........###.D#"
    "####################"
};

SDL_Color getTileColor(TileType type) {
    switch(type) {
        case TILE_WALL: return (SDL_Color){128, 128, 128, 255};
        case TILE_PLATFORM: return (SDL_Color){160, 82, 45, 255};
        case TILE_LADDER: return (SDL_Color){139, 69, 19, 255};
        case TILE_DOOR: return (SDL_Color){0, 128, 0, 255};
        case TILE_TROPHY: return (SDL_Color){255, 215, 0, 255};
        case TILE_GEM: return (SDL_Color){0, 255, 255, 255};
        case TILE_SPIKE: return (SDL_Color){255, 0, 0, 255};
        case TILE_ENEMY: return (SDL_Color){255, 0, 0, 255};
        default: return (SDL_Color){0, 0, 0, 255};
    }
}

void initGame(Game* game) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        exit(1);
    }

    game->window = SDL_CreateWindow("Dangerous Dave",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH * 3, SCREEN_HEIGHT * 3, SDL_WINDOW_RESIZABLE);

    if (!game->window) {
        printf("Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!game->renderer) {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(game->window);
        SDL_Quit();
        exit(1);
    }

    SDL_RenderSetLogicalSize(game->renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    game->player.width = 12;
    game->player.height = 16;
    game->player.lives = 3;
    game->player.score = 0;
    game->currentLevel = 0;
    game->running = true;
    game->paused = false;
    game->lastTime = SDL_GetTicks();

    loadLevel(game, 0);
}

void loadLevel(Game* game, int levelNum) {
    if (levelNum >= 3) {
        game->currentLevel = 0;
        levelNum = 0;
    }

    game->currentLevel = levelNum;
    Level* level = &game->level;

    level->enemyCount = 0;
    level->collectibleCount = 0;

    const char* data = LEVEL_DATA[levelNum];

    for (int y = 0; y < LEVEL_HEIGHT; y++) {
        for (int x = 0; x < LEVEL_WIDTH; x++) {
            char c = data[y * LEVEL_WIDTH + x];
            TileType type = TILE_EMPTY;

            switch(c) {
                case '#': type = TILE_WALL; break;
                case '=': type = TILE_PLATFORM; break;
                case 'P':
                    game->player.x = x * TILE_SIZE + 2;
                    game->player.y = y * TILE_SIZE;
                    game->player.vx = 0;
                    game->player.vy = 0;
                    game->player.onGround = false;
                    game->player.alive = true;
                    game->player.hasTrophy = false;
                    break;
                case 'D':
                    type = TILE_DOOR;
                    level->doorX = x;
                    level->doorY = y;
                    break;
                case 'T':
                    type = TILE_TROPHY;
                    level->collectibles[level->collectibleCount].x = x;
                    level->collectibles[level->collectibleCount].y = y;
                    level->collectibles[level->collectibleCount].type = TILE_TROPHY;
                    level->collectibles[level->collectibleCount].collected = false;
                    level->collectibleCount++;
                    break;
                case 'G':
                    type = TILE_GEM;
                    level->collectibles[level->collectibleCount].x = x;
                    level->collectibles[level->collectibleCount].y = y;
                    level->collectibles[level->collectibleCount].type = TILE_GEM;
                    level->collectibles[level->collectibleCount].collected = false;
                    level->collectibleCount++;
                    break;
                case 'E':
                    type = TILE_EMPTY;
                    level->enemies[level->enemyCount].x = x * TILE_SIZE;
                    level->enemies[level->enemyCount].y = y * TILE_SIZE;
                    level->enemies[level->enemyCount].vx = 1;
                    level->enemies[level->enemyCount].width = 14;
                    level->enemies[level->enemyCount].height = 14;
                    level->enemies[level->enemyCount].type = ENEMY_PATROL;
                    level->enemies[level->enemyCount].alive = true;
                    level->enemies[level->enemyCount].startX = (x - 2) * TILE_SIZE;
                    level->enemies[level->enemyCount].endX = (x + 2) * TILE_SIZE;
                    level->enemies[level->enemyCount].dir = 1;
                    level->enemyCount++;
                    break;
            }
            level->tiles[y][x] = type;
        }
    }
}

