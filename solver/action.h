#ifndef SOLVER_ACTION_H
#define SOLVER_ACTION_H

typedef enum {
    ACT_NONE = 40,
    SET_MOUSE_INFO,
    SET_VISITED,
    // 基本移動（今のmacroと一致）
    ACT_MOVE_1CELL,
    ACT_TURN_LEFT_MOVE,
    ACT_TURN_RIGHT_MOVE,
    ACT_TURN_BACK,

    // 将来拡張用
    ACT_MOVE_HALF_CELL,
    ACT_BIG_TURN_LEFT,
    ACT_BIG_TURN_RIGHT,
    ACT_SLALOM_LEFT,
    ACT_SLALOM_RIGHT,

    ACT_FINISH,
    ACT_ERROR
} Action;

#endif /* SOLVER_ACTION_H */