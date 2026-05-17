#include "global.h"

uint8_vector solver_left_wall(bool left, bool front, bool right) {
    uint8_vector_init(&action_queue);
    uint8_vector_push(&action_queue, ACT_None); // dummy
    if (!left && !is_visited(&mousePos, TO_LEFT)) {
        uint8_vector_push(&action_queue, ACT_TURN_LEFT_MOVE); // turn left
        move_mousePos(&mousePos, TO_LEFT);
        visited[mousePos.y][mousePos.x] = true;
    } else if (!front && !is_visited(&mousePos, TO_FRONT)) {
        uint8_vector_push(&action_queue, ACT_MOVE_1CELL); // move forward
        move_mousePos(&mousePos, TO_FRONT);
        visited[mousePos.y][mousePos.x] = true;
    } else if (!right && !is_visited(&mousePos, TO_RIGHT)) {
        uint8_vector_push(&action_queue, ACT_TURN_RIGHT_MOVE); // turn right
        move_mousePos(&mousePos, TO_RIGHT);
        visited[mousePos.y][mousePos.x] = true;
    } else {
        if (!left) {
            uint8_vector_push(&action_queue, ACT_TURN_LEFT_MOVE); // turn left
            move_mousePos(&mousePos, TO_LEFT);
            visited[mousePos.y][mousePos.x] = true;
        } else if (!front) {
            uint8_vector_push(&action_queue, ACT_MOVE_1CELL); // move forward
            move_mousePos(&mousePos, TO_FRONT);
            visited[mousePos.y][mousePos.x] = true;
        } else if (!right) {
            uint8_vector_push(&action_queue, ACT_TURN_RIGHT_MOVE); // turn right
            move_mousePos(&mousePos, TO_RIGHT);
            visited[mousePos.y][mousePos.x] = true;
        } else {
            uint8_vector_push(&action_queue, ACT_TURN_BACK); // turn back
            move_mousePos(&mousePos, TO_BACK);
            visited[mousePos.y][mousePos.x] = true;
        }
    }

    uint8_vector_push(&action_queue, SET_MOUSE_INFO);
    uint8_vector_push(&action_queue, mousePos.x);
    uint8_vector_push(&action_queue, mousePos.y);
    uint8_vector_push(&action_queue, mousePos.dir);
    
    uint8_vector_push(&action_queue, SET_VISITED);
    uint8_vector_push(&action_queue, mousePos.x);
    uint8_vector_push(&action_queue, mousePos.y);
    
    return action_queue;
}