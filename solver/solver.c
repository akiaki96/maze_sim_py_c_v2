#include "global.h"

void solver_init_pos() {
    mousePos.x = 0;
    mousePos.y = 0;
    mousePos.dir = NORTH;
}

void solver_init_map() {
    for (int8_t y = 0; y < 16; y++) {
        for (int8_t x = 0; x < 16; x++) {
            map[y][x] = 0x0f;
        }
    }
    for (int8_t y = 0; y < 16; y++) {
        map[y][0] |= 0xf1;
        map[y][15] |= 0xf4;
    }
    for (int8_t x = 0; x < 16; x++) {
        map[0][x] |= 0xf2;
        map[15][x] |= 0xf8;
    }

    for (int8_t y = 0; y < 16; y++)
    {
        for (int8_t x = 0; x < 16; x++)
        {
            visited[y][x] = false;
        }
    }
}    

uint8_vector solver_init_all() {
    solver_init_pos();
    solver_init_map();
    visited[0][0] = true;
    uint8_vector_init(&action_queue);
    uint8_vector_push(&action_queue, SET_MOUSE_INFO);
    uint8_vector_push(&action_queue, 0);
    uint8_vector_push(&action_queue, 1);
    uint8_vector_push(&action_queue, NORTH);

    uint8_vector_push(&action_queue, SET_VISITED);
    uint8_vector_push(&action_queue, 0);
    uint8_vector_push(&action_queue, 0);

    uint8_vector_push(&action_queue, READ_WALL);

    return action_queue;
}