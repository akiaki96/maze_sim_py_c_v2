
#include "global.h"

uint8_vector solver_time_based_dijekstra_init(void) {
    uint8_vector_init(&action_queue);

    uint8_vector_push(&action_queue, ACT_MOVE_FIRST_HALF_CELL);


    uint8_vector_push(&action_queue, ACT_FINISH);
    return action_queue;
}

uint8_vector solver_time_based_dijekstra(bool left, bool front, bool right) {
    uint8_vector_init(&action_queue);

    return action_queue;
}