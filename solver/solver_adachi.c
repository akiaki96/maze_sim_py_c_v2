#include "global.h"

uint8_t dist_map[MAZE_SIZE][MAZE_SIZE];
uint8_t prev_pos[MAZE_SIZE][MAZE_SIZE];

uint8_t xy_to_num(uint8_t x, uint8_t y) {
    return (uint8_t)(y*MAZE_SIZE + x);
}

/*
Est + Rl90 -> Nth
*/
AbsDir relToAbsDir(AbsDir absdir, RelDir reldir) {
    return (absdir+reldir) & 7;
}

/*
Est, Nth -> Rl90
*/
RelDir absToRelDir(AbsDir before, AbsDir after) {
    return (after - before) & 7;
}

void adv_pos(uint8_t* nowx, uint8_t* nowy, AbsDir nowdir) {
    if (nowdir == Est) {
        *nowx += 1;
    } else if (nowdir == Nth) {
        *nowy += 1;
    } else if (nowdir == Wst) {
        *nowx -= 1;
    } else if (nowdir == Sth) {
        *nowy -= 1;
    }
}

uint8_t calc_genpath(Wall* wallzero, uint8_t nowx, uint8_t nowy, AbsDir nowdir, uint8_t goalx, uint8_t goaly) {
    for (int8_t y = 0; y < MAZE_SIZE; y++) {
        for (int8_t x = 0; x < MAZE_SIZE; x++) {
            dist_map[y][x] = 0xFF;
            prev_pos[y][x] = 0xFF;
        }
    }

    dist_map[nowy][nowx] = 0;

    // BFS
    uint8_deque queue;
    uint8_deque_init(&queue);
    
    uint8_t pos = xy_to_num(nowx, nowy);
    uint8_deque_push_back(&queue, pos);
    
    while (!uint8_deque_is_empty(&queue)) {
        uint8_t current_pos = uint8_deque_pop_front(&queue);
        uint8_t x, y;
        pos_to_xy(current_pos, &x, &y);
        uint16_t current_dist = dist_map[y][x];
        
        // Try all 4 directions
        AbsDir directions[] = {Est, Nth, Wst, Sth};
        int8_t dx[] = {1, 0, -1, 0};
        int8_t dy[] = {0, 1, 0, -1};
        
        for (int i = 0; i < 4; i++) {
            int8_t nx = x + dx[i];
            int8_t ny = y + dy[i];
            
            // Check bounds
            if (nx < 0 || nx >= MAZE_SIZE || ny < 0 || ny >= MAZE_SIZE) {
                continue;
            }
            // Check if there's a wall
            if (get_wall_abs(wallzero, x, y, directions[i])) {
                continue;
            }
            // Check if already visited
            if (dist_map[ny][nx] != 0xFF) {
                continue;
            }
            
            // Update distance and push to queue
            dist_map[ny][nx] = current_dist + 1;
            uint8_t next_pos = xy_to_num(nx, ny);
            uint8_deque_push_back(&queue, next_pos);
            prev_pos[ny][nx] = current_pos;
        }
    }

    uint8_t tempx, tempy;
    uint8_t current_pos = xy_to_num(goalx, goaly);
    uint8_t temp_pos = xy_to_num(goalx, goaly);
    uint8_t result_dir;
    while (current_pos != 0xFF) {
        pos_to_xy(current_pos, &tempx, &tempy);
        current_pos = prev_pos[tempy][tempx];
        if (current_pos - temp_pos == 1) {
            result_dir = Wst;
        } else if (current_pos - temp_pos == -1) {
            result_dir = Est;
        } else if (current_pos - temp_pos == MAZE_SIZE) {
            result_dir = Sth;
        } else if (current_pos - temp_pos == -MAZE_SIZE) {
            result_dir = Nth;
        }
        temp_pos = current_pos;
    }

    return result_dir;
}

uint8_vector solver_adachi(bool left, bool front, bool right) {

    uint8_vector_init(&action_queue);

    uint8_vector_push(&action_queue, SET_VISITED);
    uint8_vector_push(&action_queue, mousePos.x);
    uint8_vector_push(&action_queue, mousePos.y);

    set_wall(&wallone, mousePos.x, mousePos.y, relToAbsDir(mousePos.dir, Rl90), left);
    set_wall(&wallzero, mousePos.x, mousePos.y, relToAbsDir(mousePos.dir, Rl90), left);
    set_wall(&wallone, mousePos.x, mousePos.y, relToAbsDir(mousePos.dir, R0), front);
    set_wall(&wallzero, mousePos.x, mousePos.y, relToAbsDir(mousePos.dir, R0), front);
    set_wall(&wallone, mousePos.x, mousePos.y, relToAbsDir(mousePos.dir, Rr90), right);
    set_wall(&wallzero, mousePos.x, mousePos.y, relToAbsDir(mousePos.dir, Rr90), right);

    // print_wall(&wallzero);

    if (mousePos.x == 7 && mousePos.y == 7) {
        uint8_vector_push(&action_queue, ACT_FINISH);
        return action_queue;
    }

    uint8_t next_dir = calc_genpath(&wallzero, mousePos.x, mousePos.y, mousePos.dir, 7, 7);
    if (next_dir == mousePos.dir) {
        uint8_vector_push(&action_queue, ACT_MOVE_1CELL);
    } else if (next_dir == relToAbsDir(mousePos.dir, Rl90)) {
        uint8_vector_push(&action_queue, ACT_TURN_LEFT_MOVE);
        mousePos.dir = relToAbsDir(mousePos.dir, Rl90);
    } else if (next_dir == relToAbsDir(mousePos.dir, Rr90)) {
        uint8_vector_push(&action_queue, ACT_TURN_RIGHT_MOVE);
        mousePos.dir = relToAbsDir(mousePos.dir, Rr90);
    } else {
        uint8_vector_push(&action_queue, ACT_TURN_BACK);
        mousePos.dir = relToAbsDir(mousePos.dir, R180);
    }
    adv_pos(&mousePos.x, &mousePos.y, mousePos.dir);

    uint8_vector_push(&action_queue, SET_MOUSE_INFO);
    uint8_vector_push(&action_queue, mousePos.x);
    uint8_vector_push(&action_queue, mousePos.y);
    uint8_vector_push(&action_queue, mousePos.dir);

    uint8_vector_push(&action_queue, READ_WALL);

    return action_queue;
}