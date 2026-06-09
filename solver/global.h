#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
/* ------------- */

#define def_deque_h(T, name, max_len)\
typedef struct {\
    uint16_t   top;       \
    uint16_t   bottom;    \
    uint16_t   size;      \
    T   array[max_len]; \
    uint16_t capacity;\
} name;\
void name##_init(name*);\
bool name##_is_empty(const name*);\
bool name##_is_full(const name*);\
bool name##_push_front(name*, T);\
bool name##_push_back(name*, T);\
T name##_pop_front(name*);\
T name##_pop_back(name*);\
T name##_peek_at_Nth(const name*, T);\

#define def_vector_h(T, name, len)              \
    typedef struct {                            \
        T value[len];                           \
        int16_t size;                          \
    } name;                                     \
                                                \
    void name##_init(name* vec);                \
    void name##_push(name* vec, T value);       \
    T name##_pop(name* vec);                    \
    void name##_insert(name*, int16_t, T);       \
    void name##_remove(name*, int16_t);          \

def_deque_h(uint8_t, uint8_deque, 256)
def_vector_h(uint8_t, uint8_vector, 256)

typedef struct
{
    uint8_t x;
    uint8_t y;
    uint8_t dir;
} MousePos;

#define EAST 0
#define NORTHEAST 1
#define NORTH 2
#define NORTHWEST 3
#define WEST 4
#define SOUTHWEST 5
#define SOUTH 6
#define SOUTHEAST 7

#define TO_FRONT 0
#define TO_LEFT 2
#define TO_BACK 4
#define TO_RIGHT 6

bool is_visited(MousePos* pos, int8_t look_dir);
void move_mousePos(MousePos* pos, int8_t move_dir);

/* solver.c */
void solver_init_pos();
void solver_init_map();
uint8_vector solver_left_wall_init();

/* action */
#include "action.h"

#ifdef GLOBAL_C
#define LET_GLOBAL
#else
#define LET_GLOBAL extern
#endif

LET_GLOBAL uint8_vector action_queue;
LET_GLOBAL MousePos mousePos;
LET_GLOBAL uint8_t map[16][16];
LET_GLOBAL uint8_t visited[16][16];

/* maze.c */
#define MAZE_SIZE 16
// MAZE_SIZE == 16
typedef struct wall_t {
    uint16_t wall_ver[MAZE_SIZE];
    uint16_t wall_hor[MAZE_SIZE];
} Wall;

typedef struct {
    uint16_t mbit[MAZE_SIZE];
} MazeBit;
typedef enum {Est, NthEst, Nth, NthWst, Wst, SthWst, Sth, SthEst} AbsDir;
typedef enum {R0, Rl45, Rl90, Rl135, R180, Rr135, Rr90, Rr45} RelDir;

uint8_t xy_to_pos(uint8_t, uint8_t);
void pos_to_xy(uint8_t, uint8_t*, uint8_t*);

void wall_reset_zero(Wall*);
void wall_reset_one(Wall*);
void set_wall(Wall*, uint8_t, uint8_t, AbsDir, bool);
bool get_wall_abs(const Wall*, uint8_t, uint8_t, AbsDir);
void print_wall(Wall*);
void print_wall_dist(Wall*, uint16_t[16][16][4], uint8_t);
void mazebit_zero(MazeBit*);
void mazebit_one(MazeBit*);
bool mazebit_get(MazeBit*, uint8_t, uint8_t);
void mazebit_set(MazeBit*, uint8_t, uint8_t, bool);

LET_GLOBAL Wall wallzero, wallone;

/* ---------- */
#ifdef __cplusplus
}
#endif

#endif // GLOBAL_H