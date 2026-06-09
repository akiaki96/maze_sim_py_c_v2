#include "global.h"

uint8_t xy_to_pos(uint8_t x, uint8_t y) {
    return y*MAZE_SIZE + x;
}

void pos_to_xy(uint8_t pos, uint8_t* x, uint8_t* y) {
    *x = pos % MAZE_SIZE;
    *y = pos / MAZE_SIZE;
}

void wall_reset_zero(Wall *wall) {
    for (int8_t i = 0; i < MAZE_SIZE; i++) {
        wall->wall_hor[i] = 0;
        wall->wall_ver[i] = 0;
    }
}

void wall_reset_one(Wall* wall) {
    for (int8_t i = 0; i < MAZE_SIZE; i++) {
        wall->wall_hor[i] = 0xFFFF;
        wall->wall_ver[i] = 0xFFFF;
    }
}

void set_wall(Wall* wall, uint8_t x, uint8_t y, AbsDir dir, bool tf) {
    if (x >= 16) {
        return;
    }
    if (y >= 16) {
        return;
    }
    if (x == 0 && dir == Wst) {
        return;
    } else if (x == MAZE_SIZE - 1 && dir == Est) {
        return;
    } else if (y == 0 && dir == Sth) {
        return;
    } else if (y == MAZE_SIZE - 1 && dir == Nth) {
        return;
    }

    if (tf) {
        if (dir == Est) {
            wall->wall_ver[y] |= 1 << (x);
        } else if (dir == Wst) {
            wall->wall_ver[y] |= 1 << (x-1);
        } else if (dir == Nth) {
            wall->wall_hor[x] |= 1 << (y);
        } else if (dir == Sth) {
            wall->wall_hor[x] |= 1 << (y-1);
        }
    } else {
        if (dir == Est) {
            wall->wall_ver[y] &= ~(1 << (x));
        } else if (dir == Wst) {
            wall->wall_ver[y] &= ~(1 << (x-1));
        } else if (dir == Nth) {
            wall->wall_hor[x] &= ~(1 << (y));
        } else if (dir == Sth) {
            wall->wall_hor[x] &= ~(1 << (y-1));
        }
    }
    return;
}

bool get_wall_abs(const Wall* wall, uint8_t x, uint8_t y, AbsDir dir) {
    if (x >= 16) {
        return true;
    }
    if (y >= 16) {
        return true;
    }
    if (x == 0 && dir == Wst) {
        return true;
    } else if (x == MAZE_SIZE - 1 && dir == Est) {
        return true;
    } else if (y == 0 && dir == Sth) {
        return true;
    } else if (y == MAZE_SIZE - 1 && dir == Nth) {
        return true;
    }

    if (dir == Est) {
        return (wall->wall_ver[y] >> (x)) & 1;
    } else if (dir == Wst) {
        // if ((wall->wall_ver[y] >> (x-1)) & 1) {
        //     printf("%2d %2d\n", x, y);
        // }
        return (wall->wall_ver[y] >> (x-1)) & 1;
    } else if (dir == Nth) {
        return (wall->wall_hor[x] >> (y)) & 1;
    } else if (dir == Sth) {
        return (wall->wall_hor[x] >> (y-1)) & 1;
    }
    return true;
}

void print_wall(Wall* wall) {

    printf("+");
    for (int8_t i = 0; i < MAZE_SIZE; i++) {
        printf("---+");
    }
    printf("\n");

    for (int8_t y = MAZE_SIZE-1; y >= 0; y--) {
        printf("|");
        for (int8_t x = 0; x < MAZE_SIZE; x++) {
            // printf("%d\n", get_wall_abs(&wall, x, y, Wst));
            if (get_wall_abs(wall, x, y, Est)) {
                printf("   |");
            } else {
                printf("    ");
            }
        }
        printf("\n");
        printf("+");
        for (int8_t x = 0; x < MAZE_SIZE; x++) {
            if (get_wall_abs(wall, x, y, Sth)) {
                printf("---+");
            } else {
                printf("   +");
            }
        }
        printf("\n");
    }
}

void print_wall_dist(Wall* wall, uint16_t dist[16][16][4], uint8_t page) {
    printf("+");
    for (int8_t i = 0; i < MAZE_SIZE; i++) {
        printf("----+");
    }
    printf("\n");

    for (int8_t y = MAZE_SIZE-1; y >= 0; y--) {
        printf("|");
        for (int8_t x = 0; x < MAZE_SIZE; x++) {
            // printf("%d\n", get_wall_abs(&wall, x, y, Wst));
            if (get_wall_abs(wall, x, y, Est)) {
                if (dist[x][y][page] == 0xFFFF) {
                    printf("FFFF|");
                } else {
                    printf("%4d|", dist[x][y][page]);
                }
            } else {
                if (dist[x][y][page] == 0xFFFF) {
                    printf("FFFF ");
                } else {
                    printf("%4d ", dist[x][y][page]);
                }
            }
        }
        printf("\n");
        printf("+");
        for (int8_t x = 0; x < MAZE_SIZE; x++) {
            if (get_wall_abs(wall, x, y, Sth)) {
                printf("----+");
            } else {
                printf("    +");
            }
        }
        printf("\n");
    }
}


void mazebit_zero(MazeBit* maze) {
    for (int8_t i = 0; i < MAZE_SIZE; i++) {
        maze->mbit[i] = 0x0000;
    }
}

void mazebit_one(MazeBit* maze) {
    for (int8_t i = 0; i < MAZE_SIZE; i++) {
        maze->mbit[i] = 0xFFFF;
    }
}

bool mazebit_get(MazeBit* maze, uint8_t x, uint8_t y) {
    return (maze->mbit[y])>>x & 1;
}

void mazebit_set(MazeBit* maze, uint8_t x, uint8_t y, bool tf) {
    if (tf) {
        (maze->mbit[y]) |= (1<<x);
    } else {
        (maze->mbit[y]) &= (~(1<<x));
    }
}