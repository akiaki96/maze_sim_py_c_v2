#include "global.h"

#define def_deque_c(T, name)\
void name##_init(name *deque) {\
    deque->top = 0;\
    deque->bottom = 0;\
    deque->size = 0;\
    deque->capacity = sizeof(deque->array) / sizeof(T);\
}\
bool name##_is_empty(const name *deque) {\
    return (deque->size == 0);\
}\
bool name##_is_full(const name *deque) {\
    return (deque->size == deque->capacity);\
}\
bool name##_push_front(name *deque, T num) {\
    if (name##_is_full(deque)) return false;\
    deque->top = (deque->top - 1 + deque->capacity) % deque->capacity;\
    deque->array[deque->top] = num;\
    deque->size += 1;\
    return true;\
}\
bool name##_push_back(name *deque, T num)\
{\
    if (name##_is_full(deque))\
        return (false);\
    if (deque->size == 0) {\
        deque->array[deque->top] = num;\
        deque->bottom = (deque->top + 1) % deque->capacity;\
    } else {\
        deque->array[deque->bottom] = num;\
        deque->bottom = (deque->bottom + 1) % deque->capacity;\
    }\
    deque->size += 1;\
    return true;\
}\
T name##_pop_front(name *deque) {\
    if (name##_is_empty(deque)) return false;\
    T out = deque->array[deque->top];\
    deque->top = (deque->top + 1) % deque->capacity;\
    deque->size -= 1;\
    return out;\
}\
T name##_pop_back(name *deque) {\
    if (name##_is_empty(deque))\
        return (false);\
    \
    deque->bottom = (deque->bottom - 1 + deque->capacity) % deque->capacity;\
    T out = deque->array[deque->bottom];\
    deque->size -= 1;\
    return out;\
}\
T name##_peek_at_Nth(const name *deque, T index) {\
    uint16_t position;\
    if (index >= deque->size)\
        return 0;\
    position = (deque->top + index) % deque->capacity;\
    return deque->array[position];\
}\

#define def_vector_c(T, name, len)                           \
    void name##_init(name* vec) {                       \
        vec->value[len] = 0;                                  \
    }                                                   \
                                                        \
    void name##_push(name* vec, T value) {              \
        vec->value[vec->value[len]++] = value;                \
    }                                                   \
                                                        \
    T name##_pop(name* vec) {                           \
        return vec->value[vec->value[len]--];                 \
    }                                                   \
                                                        \
    void name##_insert(name* vec, int16_t place, T item){\
        for (int16_t i = vec->value[len]-1; i >= place; i--) { \
            vec->value[i+1] = vec->value[i];            \
        }                                               \
        vec->value[place] = item;                       \
        vec->value[len]++;                                    \
    }                                                   \
                                                        \
    void name##_remove(name* vec, int16_t place) {       \
        for (int16_t i = place; i < vec->value[len]; i++) {    \
            vec->value[i] = vec->value[i+1];            \
        }                                               \
        vec->value[len]--;                                    \
    }                                                   \

def_deque_c(uint8_t, uint8_deque)
def_vector_c(uint8_t, uint8_vector, 255)


bool is_visited(MousePos* pos, int8_t look_dir) {
    uint8_t dir = (pos->dir + look_dir + 4) % 4;
    if (dir == EAST) {
        return visited[pos->y][pos->x+1];
    } else if (dir == NORTH) {
        return visited[pos->y+1][pos->x];
    } else if (dir == WEST) {
        return visited[pos->y][pos->x-1];
    } else if (dir == SOUTH) {
        return visited[pos->y-1][pos->x];
    }
    return false;
}

void move_mousePos(MousePos* pos, int8_t move_dir) {
    if (move_dir == TO_LEFT) {
        pos->dir = (pos->dir + 3) % 4;
    } else if (move_dir == TO_RIGHT) {
        pos->dir = (pos->dir + 1) % 4;
    } else if (move_dir == TO_FRONT) {
        if (pos->dir == EAST) {
            pos->x += 1;
        } else if (pos->dir == NORTH) {
            pos->y += 1;
        } else if (pos->dir == WEST) {
            pos->x -= 1;
        } else if (pos->dir == SOUTH) {
            pos->y -= 1;
        }
    }
}