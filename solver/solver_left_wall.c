#include "global.h"

uint8_vector solver_left_wall(bool left, bool front, bool right) {
    uint8_vector_init(&action_queue);
    if (!left) {
        if (is_visited(&mousePos, TO_LEFT)) {
            uint8_vector_push(&action_queue, ACT_TURN_LEFT_MOVE); // turn left
            move_mousePos(&mousePos, TO_LEFT);
            visited[mousePos.y][mousePos.x] = true;
        }
    } else if (!front) {
        if (is_visited(&mousePos, TO_FRONT)) {
            uint8_vector_push(&action_queue, ACT_MOVE_1CELL); // move forward
            move_mousePos(&mousePos, TO_FRONT);
            visited[mousePos.y][mousePos.x] = true;
        }
    } else if (!right) {
        if (is_visited(&mousePos, TO_RIGHT)) {
            uint8_vector_push(&action_queue, ACT_TURN_RIGHT_MOVE); // turn right
            move_mousePos(&mousePos, TO_RIGHT);
            visited[mousePos.y][mousePos.x] = true;
        }
    } else {
        uint8_vector_push(&action_queue, ACT_TURN_BACK); // turn back
        move_mousePos(&mousePos, TO_BACK);
        visited[mousePos.y][mousePos.x] = true;
    }
    return action_queue;
}