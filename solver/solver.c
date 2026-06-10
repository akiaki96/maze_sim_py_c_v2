#include "global.h"

void solver_init_pos() {
    mousePos.x = 0;
    mousePos.y = 1;
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

uint8_vector solver_left_wall_init() {
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

    uint8_vector_push(&action_queue, ACT_MOVE_FIRST_HALF_CELL); // move first half cell

    uint8_vector_push(&action_queue, READ_WALL);

    return action_queue;
}

uint8_vector solver_adachi_init() {
    solver_init_pos();
    solver_init_map();
    uint8_vector_init(&action_queue);
    wall_reset_one(&wallone);
    wall_reset_zero(&wallzero);

    set_wall(&wallzero, 0, 0, Est, true);
    uint8_vector_push(&action_queue, SET_WALL);
    uint8_vector_push(&action_queue, 0);
    uint8_vector_push(&action_queue, 0);
    uint8_vector_push(&action_queue, Est);
    uint8_vector_push(&action_queue, true);

    visited[0][0] = true;
    uint8_vector_push(&action_queue, SET_MOUSE_INFO);
    uint8_vector_push(&action_queue, 0);
    uint8_vector_push(&action_queue, 1);
    uint8_vector_push(&action_queue, NORTH);

    uint8_vector_push(&action_queue, SET_VISITED);
    uint8_vector_push(&action_queue, 0);
    uint8_vector_push(&action_queue, 0);

    uint8_vector_push(&action_queue, ACT_MOVE_FIRST_HALF_CELL); // move first half cell
    uint8_vector_push(&action_queue, READ_WALL);

    return action_queue;
}

uint8_vector solver_adachi_init_no_reset() {
    solver_init_pos();
    solver_init_map();
    uint8_vector_init(&action_queue);
    
    set_wall(&wallzero, 0, 0, Est, true);
    uint8_vector_push(&action_queue, SET_WALL);
    uint8_vector_push(&action_queue, 0);
    uint8_vector_push(&action_queue, 0);
    uint8_vector_push(&action_queue, Est);
    uint8_vector_push(&action_queue, true);

    visited[0][0] = true;
    uint8_vector_push(&action_queue, SET_MOUSE_INFO);
    uint8_vector_push(&action_queue, 0);
    uint8_vector_push(&action_queue, 1);
    uint8_vector_push(&action_queue, NORTH);

    uint8_vector_push(&action_queue, SET_VISITED);
    uint8_vector_push(&action_queue, 0);
    uint8_vector_push(&action_queue, 0);

    uint8_vector_push(&action_queue, ACT_MOVE_FIRST_HALF_CELL); // move first half cell
    uint8_vector_push(&action_queue, READ_WALL);

    return action_queue;
}