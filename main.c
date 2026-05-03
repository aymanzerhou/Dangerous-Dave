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

void cleanupGame(Game* game) {
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    SDL_Quit();
}

void processInput(Game* game) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            game->running = false;
        }
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_r) {
                resetLevel(game);
            }
            if (event.key.keysym.sym == SDLK_p) {
                game->paused = !game->paused;
            }
        }
    }
}

void movePlayer(Player* player, Level* level) {
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    player->vx = 0;
    if (keys[SDL_SCANCODE_LEFT]) player->vx = -PLAYER_SPEED;
    if (keys[SDL_SCANCODE_RIGHT]) player->vx = PLAYER_SPEED;
    if (keys[SDL_SCANCODE_UP] && player->onGround) {
        player->vy = -JUMP_FORCE;
        player->onGround = false;
    }

    player->vy += GRAVITY;

    float newX = player->x + player->vx;
    if (!checkCollision(newX, player->y, player->width, player->height, level)) {
        player->x = newX;
    }

    float newY = player->y + player->vy;
    player->onGround = false;

    if (!checkCollision(player->x, newY, player->width, player->height, level)) {
        player->y = newY;
    } else {
        if (player->vy > 0) {
            player->onGround = true;
        }
        player->vy = 0;
    }

    if (player->y > SCREEN_HEIGHT) {
        player->alive = false;
    }
}

bool checkCollision(float x, float y, int w, int h, Level* level) {
    int left = (int)(x / TILE_SIZE);
    int right = (int)((x + w - 1) / TILE_SIZE);
    int top = (int)(y / TILE_SIZE);
    int bottom = (int)((y + h - 1) / TILE_SIZE);

    if (left < 0 || right >= LEVEL_WIDTH || top < 0 || bottom >= LEVEL_HEIGHT) {
        return true;
    }

    for (int ty = top; ty <= bottom; ty++) {
        for (int tx = left; tx <= right; tx++) {
            TileType tile = level->tiles[ty][tx];
            if (tile == TILE_WALL || tile == TILE_PLATFORM) {
                return true;
            }
        }
    }
    return false;
}

void updateEnemies(Level* level, Player* player) {
    for (int i = 0; i < level->enemyCount; i++) {
        Enemy* e = &level->enemies[i];
        if (!e->alive) continue;

        if (e->type == ENEMY_PATROL) {
            e->x += e->vx * e->dir;
            if (e->x <= e->startX || e->x >= e->endX) {
                e->dir *= -1;
            }
        }

        float px = player->x + player->width / 2;
        float py = player->y + player->height / 2;
        float ex = e->x + e->width / 2;
        float ey = e->y + e->height / 2;

        float dx = px - ex;
        float dy = py - ey;
        float dist = sqrt(dx * dx + dy * dy);

        if (dist < 10) {
            player->alive = false;
        }
    }
}

void checkCollectibles(Level* level, Player* player) {
    float px = player->x + player->width / 2;
    float py = player->y + player->height / 2;

    for (int i = 0; i < level->collectibleCount; i++) {
        Collectible* c = &level->collectibles[i];
        if (c->collected) continue;

        float cx = c->x * TILE_SIZE + TILE_SIZE / 2;
        float cy = c->y * TILE_SIZE + TILE_SIZE / 2;
        float dx = px - cx;
        float dy = py - cy;
        float dist = sqrt(dx * dx + dy * dy);

        if (dist < 12) {
            c->collected = true;
            level->tiles[c->y][c->x] = TILE_EMPTY;

            if (c->type == TILE_TROPHY) {
                player->hasTrophy = true;
                player->score += 1000;
            } else if (c->type == TILE_GEM) {
                player->score += 100;
            }
        }
    }

    int doorX = level->doorX * TILE_SIZE + TILE_SIZE / 2;
    int doorY = level->doorY * TILE_SIZE + TILE_SIZE / 2;
    float ddx = px - doorX;
    float ddy = py - doorY;
    float doorDist = sqrt(ddx * ddx + ddy * ddy);

    if (doorDist < 12 && player->hasTrophy) {
    }
}

void checkLevelComplete(Game* game) {
    Level* level = &game->level;
    Player* player = &game->player;

    float px = player->x + player->width / 2;
    float py = player->y + player->height / 2;
    int doorX = level->doorX * TILE_SIZE + TILE_SIZE / 2;
    int doorY = level->doorY * TILE_SIZE + TILE_SIZE / 2;
    float ddx = px - doorX;
    float ddy = py - doorY;
    float doorDist = sqrt(ddx * ddx + ddy * ddy);

    if (doorDist < 12 && player->hasTrophy) {
        loadLevel(game, game->currentLevel + 1);
    }
}

void resetLevel(Game* game) {
    game->player.lives--;
    if (game->player.lives <= 0) {
        game->player.score = 0;
        game->player.lives = 3;
        game->currentLevel = 0;
    }
    loadLevel(game, game->currentLevel);
}

void updateGame(Game* game) {
    if (game->paused) return;

    if (!game->player.alive) {
        resetLevel(game);
        return;
    }

    movePlayer(&game->player, &game->level);
    updateEnemies(&game->level, &game->player);
    checkCollectibles(&game->level, &game->player);
    checkLevelComplete(game);
}

void renderGame(Game* game) {
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    SDL_RenderClear(game->renderer);

    Level* level = &game->level;

    for (int y = 0; y < LEVEL_HEIGHT; y++) {
        for (int x = 0; x < LEVEL_WIDTH; x++) {
            TileType tile = level->tiles[y][x];
            if (tile != TILE_EMPTY) {
                SDL_Rect rect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                SDL_Color c = getTileColor(tile);
                SDL_SetRenderDrawColor(game->renderer, c.r, c.g, c.b, c.a);
                SDL_RenderFillRect(game->renderer, &rect);

                if (tile == TILE_WALL || tile == TILE_PLATFORM) {
                    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
                    SDL_RenderDrawRect(game->renderer, &rect);
                }
            }
        }
    }

    for (int i = 0; i < level->enemyCount; i++) {
        Enemy* e = &level->enemies[i];
        if (e->alive) {
            SDL_Rect rect = {(int)e->x, (int)e->y, e->width, e->height};
            SDL_SetRenderDrawColor(game->renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(game->renderer, &rect);
            SDL_SetRenderDrawColor(game->renderer, 128, 0, 0, 255);
            SDL_RenderDrawRect(game->renderer, &rect);
        }
    }

    Player* p = &game->player;
    SDL_Rect playerRect = {(int)p->x, (int)p->y, p->width, p->height};
    SDL_SetRenderDrawColor(game->renderer, 255, 255, 0, 255);
    SDL_RenderFillRect(game->renderer, &playerRect);
    SDL_SetRenderDrawColor(game->renderer, 200, 200, 0, 255);
    SDL_RenderDrawRect(game->renderer, &playerRect);

    static char hud[128];
    snprintf(hud, sizeof(hud), "Score: %d  Lives: %d  Level: %d %s",
        p->score, p->lives, game->currentLevel + 1,
        p->hasTrophy ? "[TROPHY]" : "");

    SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);

    if (game->paused) {
        SDL_SetRenderDrawColor(game->renderer, 128, 128, 128, 200);
        SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(game->renderer, &overlay);
    }

    SDL_RenderPresent(game->renderer);
}

int main(int argc, char* argv[]) {
    Game game;
    initGame(&game);

    while (game.running) {
        processInput(&game);
        updateGame(&game);
        renderGame(&game);

        SDL_Delay(16);
    }

    cleanupGame(&game);
    return 0;
}
