
#include "global.h"

#define TIME_COR 1000

#define genDIR45 \
int8_t dir45[8][2];\
dir45[0][0] = 1;\
dir45[1][0] = 1;\
dir45[2][0] = 0;\
dir45[3][0] = -1;\
dir45[4][0] = -1;\
dir45[5][0] = -1;\
dir45[6][0] = 0;\
dir45[7][0] = 1;\
dir45[0][1] = 0;\
dir45[1][1] = 1;\
dir45[2][1] = 1;\
dir45[3][1] = 1;\
dir45[4][1] = 0;\
dir45[5][1] = -1;\
dir45[6][1] = -1;\
dir45[7][1] = -1;\

#define DONT_CHOOSE_HV 4

const char* op_motion_name[] = {
    "Tr45in", "Tl45in",
    "Tr45out", "Tl45out",
    "Tr135in", "Tl135in",
    "Tr135out",  "Tl135out",
    "TrV90", "TlV90",
    "Tr90", "Tl90",
    "Tr180", "Tl180",
    "Trs90", "Tls90",
};

typedef enum {STOP = 0x60, T0grd = 0x20, T0dia = 0x40,
    Tr45in = 0, Tl45in,
    Tr45out, Tl45out,
    Tr135in, Tl135in,
    Tr135out,  Tl135out,
    TrV90, TlV90,
    Tr90, Tl90,
    Tr180, Tl180,
    Trs90, Tls90,
    Turn180} motion_op;


typedef struct pos_t{
    uint16_t dist;
    int8_t x;
    int8_t y;
    uint8_t hv;
} Pos;

typedef struct {
    int8_t diff_x;
    int8_t diff_y;
    AbsDir lookdir;
    bool lr;
} TurnWall;

const uint16_t EX_STRAIGHT[16] ={82,164,227,273,311,344,374,404,434,464,494,524,554,584,614}; 
const uint16_t EX_DIAGONALT[32]={58,116,174 ,231 ,279 ,315 ,346 ,372 ,397 ,
							419 ,440 ,461 ,483 ,504 ,525 ,546 ,567 ,589 ,610 ,
							631 ,652 ,673 ,695 ,716 ,737 ,758 ,779 ,801 ,822 ,843 ,864 }; 

const TurnWall Est_R = (TurnWall){1, -1, Nth, false};
const TurnWall Est_L = (TurnWall){1, 0, Nth, true};
const TurnWall Nth_R = (TurnWall){0, 1, Est, false};
const TurnWall Nth_L = (TurnWall){-1, 1, Est, true};
const TurnWall Wst_R = (TurnWall){0, 0, Nth, false};
const TurnWall Wst_L = (TurnWall){0, -1, Nth, true};
const TurnWall Sth_R = (TurnWall){-1, 0, Est, false};
const TurnWall Sth_L = (TurnWall){0, 0, Est, true};

const char dirname[][20] = {"→ Est", "↗ NthEst", "↑ Nth", "↖ NthWst", "← Wst", "↙ SthWst", "↓ Sth", "↘ SthEst"};

#define S90 (uint16_t)(303.0f / 500 * TIME_COR)
#define S180 (uint16_t)(407.0f / 500 * TIME_COR)
#define S45 (uint16_t)(210.0f / 500 * TIME_COR)
#define S135 (uint16_t)(323.5f / 500 * TIME_COR)
#define V90 (uint16_t)(219.5f / 500 * TIME_COR)

uint8_t cv_01(uint8_t num) {
    if (num == 0) {
        return 1;
    } else {
        return 0;
    }
}

uint8_t cv_23(uint8_t num) {
    if (num == 2) {
        return 3;
    } else {
        return 2;
    }
}

/*
pos
0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31
x0  x1  x2  x3  y0  y1  y2  y3  hv1 hv2 m0  m1  m2  m3  m4  m5  m6  dr0 dr1 dr2 px0 px1 px2 px3 py0 py1 py2 py3 hv0 hv1           
*/

uint8_t Psg_x(const uint32_t pos) {
    return pos & 0xF;
}
uint8_t Psg_y(const uint32_t pos) {
    return (pos & 0xF0) >> 4;
}
uint8_t Psg_hv(const uint32_t pos) {
    return (pos & 0x300) >> 8;
}
uint8_t Psg_motion(const uint32_t pos) {
    return (pos & 0x1FC00) >> 10;
}
uint8_t Psg_dirl(const uint32_t pos) {
    return (pos & 0xE0000) >> 17;
}
uint8_t Psg_prvx(const uint32_t pos) {
    return (pos & 0xF00000) >> 20;
}
uint8_t Psg_prvy(const uint32_t pos) {
    return (pos & 0xF000000) >> 24;
}
uint8_t Psg_prvhv(const uint32_t pos) {
    return (pos & 0x30000000) >> 28;
}

void Pss_x(uint32_t* pos, uint8_t val) {
    *pos &= ~0xF;
    *pos |= val;
}
void Pss_y(uint32_t* pos, uint8_t val) {
    *pos &= ~0xF0;
    *pos |= (val<<4);
}
void Pss_hv(uint32_t* pos, uint8_t val) {
    *pos &= ~0x300;
    *pos |= (val<<8);
}
void Pss_motion(uint32_t* pos, uint8_t val) {
    *pos &= ~0x1FC00;
    *pos |= (val<<10);
}
void Pss_dirl(uint32_t* pos, uint8_t val) {
    *pos &= ~0xE0000;
    *pos |= (val<<17);
}
void Pss_prvx(uint32_t* pos, uint8_t val) {
    *pos &= ~0xF00000;
    *pos |= (val<<20);
}
void Pss_prvy(uint32_t* pos, uint8_t val) {
    *pos &= ~0xF000000;
    *pos |= (val<<24);
}
void Pss_prvhv(uint32_t* pos, uint8_t val) {
    *pos &= ~0x30000000;
    *pos |= (val<<28);
}

void Ps_set(uint32_t* pos, uint8_t x, uint8_t y, uint8_t hv, uint16_t motion, uint8_t dirl, uint8_t prvx, uint8_t prvy, uint8_t prvhv) {
    Pss_x(pos, x);
    Pss_y(pos, y);
    Pss_hv(pos, hv);
    Pss_motion(pos, motion);
    Pss_dirl(pos, dirl);
    Pss_prvx(pos, prvx);
    Pss_prvy(pos, prvy);
    Pss_prvhv(pos, prvhv);
}

uint32_t Ps_get(const uint8_t x, const uint8_t y, const uint8_t hv, const uint16_t motion, const uint8_t dirl, const uint8_t prvx, const uint8_t prvy, const uint8_t prvhv) {
    return (x) | (y << 4) | (hv << 8) | (motion << 10) | (dirl << 17) | (prvx << 20) | (prvy << 24) | (prvhv << 28);
}

void Ps_print(const uint32_t pos) {
    printf("%2d %2d %d %s\n", Psg_x(pos), Psg_y(pos), Psg_hv(pos), dirname[Psg_dirl(pos)]);
}

void Ps_printAll(const uint32_t nowpos) {
    printf("%10u: ", nowpos);
    printf("%2d %2d %d ", Psg_x(nowpos), Psg_y(nowpos), Psg_hv(nowpos));
    uint16_t motion = Psg_motion(nowpos);
    if (motion & T0dia) {
        printf("T0dia%2d ", motion & 0x1F);
    } else if (motion & T0grd) {
        printf("T0grd%2d", motion & 0x1F);
    } else if (1 <= motion && motion <= 16) {
        printf("%s", op_motion_name[motion]);
    } else {
        printf("STOP%d\n",motion);
    }

    printf(" %s %2d %2d %d\n", dirname[Psg_dirl(nowpos)], Psg_prvx(nowpos), Psg_prvy(nowpos), Psg_prvhv(nowpos));
}

void dijkstra_show_path(uint32_t prvdirl[16][16][4], uint8_t goalx, uint8_t goaly, uint8_t goalhv) {
    uint8_t nowx  = goalx;
    uint8_t nowy  = goaly;
    uint8_t nowhv = goalhv;
    uint8_t nxtx, nxty, nxthv;
    printf("%2d %2d %d <- ", nowx, nowy, nowhv);
    while (!(nowx == 0 && nowy == 0 && nowhv == 0)) {
        nxtx  = Psg_prvx(prvdirl[nowx][nowy][nowhv]);
        nxty  = Psg_prvy(prvdirl[nowx][nowy][nowhv]);
        nxthv = Psg_prvhv(prvdirl[nowx][nowy][nowhv]);
        printf("(%2d %2d %d) <- ", nxtx, nxty, nxthv);
        nowx  = nxtx;
        nowy  = nxty;
        nowhv = nxthv;
    }
    printf("\n");
}

uint8_deque temp_dijkstra_rev;

void dijkstra_gen_path(uint32_t prvdirl[16][16][4], uint8_t goalx, uint8_t goaly, uint8_t goalhv) {
    uint32_t nowpos = prvdirl[goalx][goaly][goalhv];

    uint8_t nowx  = goalx;
    uint8_t nowy  = goaly;
    uint8_t nowhv = goalhv;
    uint8_t nxtx, nxty, nxthv;
    uint8_t motion;

    uint8_deque_init(&temp_dijkstra_rev);

    while (!(nowx == 0 && nowy == 0 && nowhv == 0)) {
        motion = Psg_motion(prvdirl[nowx][nowy][nowhv]);
        printf("%d ", motion);
        if (motion & T0dia) {
            printf("T0dia %2d <- ", motion & 0x1F);
            uint8_deque_push_front(&temp_dijkstra_rev, ACT_MOVE_0SEC_DIA + (motion & 0x1F));
        } else if (motion & T0grd) {
            printf("T0grd %2d <- ", motion & 0x1F);
            uint8_deque_push_front(&temp_dijkstra_rev, ACT_MOVE_0SEC + (motion & 0x1F));
        } else if (motion == Tr45in) {
            printf("Tr45in <- ");
            uint8_deque_push_front(&temp_dijkstra_rev, ACT_S45_in_RIGHT);
        } else if (motion == Tl45in) {
            printf("Tl45in <- ");
            uint8_deque_push_front(&temp_dijkstra_rev, ACT_S45_in_LEFT);
        } else if (motion == Tr45out) {
            printf("Tr45out <- ");
            uint8_deque_push_front(&temp_dijkstra_rev, ACT_S45_out_RIGHT);
        } else if (motion == Tl45out) {
            printf("Tl45out <- ");
            uint8_deque_push_front(&temp_dijkstra_rev, ACT_S45_out_LEFT);
        } else if (motion == Tr135in) {
            printf("Tr135in <- ");
            uint8_deque_push_front(&temp_dijkstra_rev, ACT_S135_in_RIGHT);
        } else if (motion == Tl135in) {
            printf("Tl135in <- ");
            uint8_deque_push_front(&temp_dijkstra_rev, ACT_S135_in_LEFT);
        } else if (motion == Tr135out) {
            printf("Tr135out <- ");
            uint8_deque_push_front(&temp_dijkstra_rev, ACT_S135_out_RIGHT);
        } else if (motion == Tl135out) {
            printf("Tl135out <- ");
            uint8_deque_push_front(&temp_dijkstra_rev, ACT_S135_out_LEFT);
        } else if (motion == TrV90) {
            printf("TrV90 <- ");
            uint8_deque_push_front(&temp_dijkstra_rev, ACT_V90_RIGHT);
        } else if (motion == TlV90) {
            printf("TlV90 <- ");
            uint8_deque_push_front(&temp_dijkstra_rev, ACT_V90_LEFT);
        } else if (motion == Tr90) {
            printf("Tr90 <- ");
            uint8_deque_push_front(&temp_dijkstra_rev, ACT_S90_RIGHT);
        } else if (motion == Tl90) {
            printf("Tl90 <- ");
            uint8_deque_push_front(&temp_dijkstra_rev, ACT_S90_LEFT);
        } else if (motion == Tr180) {
            printf("Tr180 <- ");
            uint8_deque_push_front(&temp_dijkstra_rev, ACT_S180_RIGHT);
        } else if (motion == Tl180) {
            printf("Tl180 <- ");
            uint8_deque_push_front(&temp_dijkstra_rev, ACT_S180_LEFT);
        } else if (motion == Trs90) {
            printf("Trs90 <- ");
            uint8_deque_push_front(&temp_dijkstra_rev, ACT_S90_RIGHT);
        } else if (motion == Tls90) {
            printf("Tls90 <- ");
            uint8_deque_push_front(&temp_dijkstra_rev, ACT_S90_LEFT);
        } else {
            printf("STOP ");
        }

        nxtx  = Psg_prvx(prvdirl[nowx][nowy][nowhv]);
        nxty  = Psg_prvy(prvdirl[nowx][nowy][nowhv]);
        nxthv = Psg_prvhv(prvdirl[nowx][nowy][nowhv]);
        nowx  = nxtx;
        nowy  = nxty;
        nowhv = nxthv;
    }
    printf("\n");
}

bool straight_dia(const Wall* wall, uint16_t dist[16][16][4], uint32_t prvdirl[16][16][4], PQ_cost* que, uint32_t *nowpos, uint32_t *tpos, const TurnWall* tw, AbsDir nxtdir, uint8_t tcnt) {
    // tpos.x += tw.diffx
    Pss_x(tpos, Psg_x(*tpos) + tw->diff_x);

    // tpos.y += tw.diffy
    Pss_y(tpos, Psg_y(*tpos) + tw->diff_y);

    // tpos.hv = cv_23(tpos.hv)
    Pss_hv(tpos, cv_23(Psg_hv(*tpos)));

    // if not get_wall_abs(tpos.x, tpos.y, tw.lookdir)
    if (!get_wall_abs(wall, Psg_x(*tpos), Psg_y(*tpos), tw->lookdir)) {
        // if dist[tpos] > dist[now] + EX_DIAGONALT[tcnt]
        if (dist[Psg_x(*tpos)][Psg_y(*tpos)][Psg_hv(*tpos)] > dist[Psg_x(*nowpos)][Psg_y(*nowpos)][Psg_hv(*nowpos)] + EX_DIAGONALT[tcnt]) {
            // dist[tpos] = dist[now] + cost
            dist[Psg_x(*tpos)][Psg_y(*tpos)][Psg_hv(*tpos)]= dist[Psg_x(*nowpos)][Psg_y(*nowpos)][Psg_hv(*nowpos)] + EX_DIAGONALT[tcnt];


            Pss_motion(tpos, T0dia | (tcnt + 1));
            Pss_prvx(tpos, Psg_x(*nowpos));
            Pss_prvy(tpos, Psg_y(*nowpos));
            Pss_prvhv(tpos, Psg_hv(*nowpos));
            Pss_dirl(tpos, Psg_dirl(*nowpos));
            prvdirl[Psg_x(*tpos)][Psg_y(*tpos)][Psg_hv(*tpos)] = *tpos;

            // push priority queue
            PQ_cost_push(que, (cost){dist[Psg_x(*tpos)][Psg_y(*tpos)][Psg_hv(*tpos)], *tpos});

            return true;
        }
    }

    return false;
}

bool turn_grid_90(const Wall* wall, uint16_t dist[16][16][4], uint32_t prvdirl[16][16][4], PQ_cost* que, uint32_t *nowpos, const TurnWall* tw, AbsDir nxtdir) {
    uint8_t nx = Psg_x(*nowpos) + tw->diff_x;
    uint8_t ny = Psg_y(*nowpos) + tw->diff_y;
    uint8_t  nh = cv_01(Psg_hv(*nowpos));

    if (!get_wall_abs(wall, nx, ny, tw->lookdir) &&
        dist[nx][ny][nh] > dist[Psg_x(*nowpos)][Psg_y(*nowpos)][Psg_hv(*nowpos)] + S90)
    {
        dist[nx][ny][nh] = dist[Psg_x(*nowpos)][Psg_y(*nowpos)][Psg_hv(*nowpos)] + S90;

        uint32_t newpos = *nowpos;
        Pss_x(&newpos, nx); Pss_y(&newpos, ny); Pss_hv(&newpos, nh);
        Pss_motion(&newpos, tw->lr ? Tl90 : Tr90);

        Pss_prvx(&newpos, Psg_x(*nowpos));
        Pss_prvy(&newpos, Psg_y(*nowpos));
        Pss_prvhv(&newpos, Psg_hv(*nowpos));
        Pss_dirl(&newpos, nxtdir);

        prvdirl[nx][ny][nh] = newpos;

        PQ_cost_push(que, (cost){dist[nx][ny][nh], newpos});
        return true;
    }
    return false;
}

bool turn_grid_180(const Wall* wall, uint16_t dist[16][16][4], uint32_t prvdirl[16][16][4], PQ_cost* que, uint32_t *nowpos, uint32_t *tpos, const TurnWall *tw1, TurnWall tw2, AbsDir nxtdir){
    uint8_t x = Psg_x(*nowpos), y = Psg_y(*nowpos), h = Psg_hv(*nowpos);
    uint8_t x1 = x + tw1->diff_x, y1 = y + tw1->diff_y;
    uint8_t x2 = x1 + tw2.diff_x, y2 = y1 + tw2.diff_y;

    if (!get_wall_abs(wall, x1, y1, tw1->lookdir) &&
        !get_wall_abs(wall, x2, y2, tw2.lookdir) &&
        dist[x2][y2][h] > dist[x][y][h] + S180)
    {
        dist[x2][y2][h] = dist[x][y][h] + S180;

        // tpos に書き込み
        Pss_x(tpos, x2);
        Pss_y(tpos, y2);
        Pss_hv(tpos, h);
        Pss_motion(tpos, tw2.lr ? Tl180 : Tr180);

        // 直前情報
        Pss_prvx(tpos, x); Pss_prvy(tpos, y); Pss_prvhv(tpos, h);
        Pss_dirl(tpos, nxtdir);

        prvdirl[x2][y2][h] = *tpos;


        PQ_cost_push(que, (cost){dist[x2][y2][h], *tpos});
        return true;
    }
    return false;
}


bool turn_45_in(const Wall* wall, uint16_t dist[16][16][4], uint32_t prvdirl[16][16][4], PQ_cost* que, uint32_t *now, uint32_t *tpos, const TurnWall* tw, AbsDir nxtdir)
{
    uint8_t hv2 = cv_01(Psg_hv(*now)) + 2;
    uint8_t nx = Psg_x(*now) + tw->diff_x, ny = Psg_y(*now) + tw->diff_y;

    if (!get_wall_abs(wall, nx, ny, tw->lookdir)) {
        uint16_t nd = dist[Psg_x(*now)][Psg_y(*now)][Psg_hv(*now)] + S45;
        if (dist[nx][ny][hv2] > nd) {
            dist[nx][ny][hv2] = nd;

            Pss_x(tpos, nx);  Pss_y(tpos, ny);  Pss_hv(tpos, hv2);
            Pss_motion(tpos, (tw->lr ? Tl45in : Tr45in));

            Pss_prvx(tpos, Psg_x(*now));
            Pss_prvy(tpos, Psg_y(*now));
            Pss_prvhv(tpos, Psg_hv(*now));
            Pss_dirl(tpos, nxtdir);

            prvdirl[nx][ny][hv2] = *tpos;

            PQ_cost_push(que, (cost){ nd, *tpos });
            return true;
        }
    }
    return false;
}

bool turn_45_out(const Wall* wall, uint16_t dist[16][16][4], uint32_t prvdirl[16][16][4], PQ_cost* que, uint32_t *now, uint32_t *tpos, const TurnWall* tw, AbsDir nxtdir)
{
    uint8_t hv2 = cv_23(Psg_hv(*now)) - 2;
    uint8_t nx = Psg_x(*now) + tw->diff_x, ny = Psg_y(*now) + tw->diff_y;

    if (!get_wall_abs(wall, nx, ny, tw->lookdir)) {
        uint16_t nd = dist[Psg_x(*now)][Psg_y(*now)][Psg_hv(*now)] + S45;
        if (dist[nx][ny][hv2] > nd) {
            dist[nx][ny][hv2] = nd;

            Pss_x(tpos, nx);  Pss_y(tpos, ny);  Pss_hv(tpos, hv2);
            Pss_motion(tpos, (tw->lr ? Tl45out : Tr45out));

            Pss_prvx(tpos, Psg_x(*now));
            Pss_prvy(tpos, Psg_y(*now));
            Pss_prvhv(tpos, Psg_hv(*now));
            Pss_dirl(tpos, nxtdir);

            prvdirl[nx][ny][hv2] = *tpos;

            PQ_cost_push(que, (cost){ nd, *tpos });
            return true;
        }
    }
    return false;
}
bool turn_135_in(const Wall* wall, uint16_t dist[16][16][4], uint32_t prvdirl[16][16][4], PQ_cost* que, uint32_t *now, uint32_t *tpos, const TurnWall* tw1, TurnWall tw2, AbsDir nxtdir)
{
    tw2.diff_x += tw1->diff_x;  tw2.diff_y += tw1->diff_y;

    uint8_t nx1 = Psg_x(*now) + tw1->diff_x, ny1 = Psg_y(*now) + tw1->diff_y;
    uint8_t nx2 = Psg_x(*now) + tw2.diff_x,  ny2 = Psg_y(*now) + tw2.diff_y;
    uint8_t hv2 = Psg_hv(*now) + 2;

    if (!get_wall_abs(wall, nx1, ny1, tw1->lookdir) &&
        !get_wall_abs(wall, nx2, ny2, tw2.lookdir))
    {
        uint16_t nd = dist[Psg_x(*now)][Psg_y(*now)][Psg_hv(*now)] + S135;
        if (dist[nx2][ny2][hv2] > nd) {
            dist[nx2][ny2][hv2] = nd;

            Pss_x(tpos, nx2);  Pss_y(tpos, ny2);  Pss_hv(tpos, hv2);
            Pss_motion(tpos, (tw2.lr ? Tl135in : Tr135in));

            Pss_prvx(tpos, Psg_x(*now));
            Pss_prvy(tpos, Psg_y(*now));
            Pss_prvhv(tpos, Psg_hv(*now));
            Pss_dirl(tpos, nxtdir);

            prvdirl[nx2][ny2][hv2] = *tpos;

            PQ_cost_push(que, (cost){ nd, *tpos });
            return true;
        }
    }
    return false;
}
bool turn_135_out(const Wall* wall, uint16_t dist[16][16][4], uint32_t prvdirl[16][16][4], PQ_cost* que, uint32_t *now, uint32_t *tpos, const TurnWall* tw1, TurnWall tw2, AbsDir nxtdir)
{
    tw2.diff_x += tw1->diff_x;  tw2.diff_y += tw1->diff_y;

    uint8_t nx1 = Psg_x(*now) + tw1->diff_x, ny1 = Psg_y(*now) + tw1->diff_y;
    uint8_t nx2 = Psg_x(*now) + tw2.diff_x,  ny2 = Psg_y(*now) + tw2.diff_y;
    uint8_t hv2 = Psg_hv(*now) - 2;

    if (!get_wall_abs(wall, nx1, ny1, tw1->lookdir) &&
        !get_wall_abs(wall, nx2, ny2, tw2.lookdir))
    {
        uint16_t nd = dist[Psg_x(*now)][Psg_y(*now)][Psg_hv(*now)] + S135;
        if (dist[nx2][ny2][hv2] > nd) {
            dist[nx2][ny2][hv2] = nd;

            Pss_x(tpos, nx2);  Pss_y(tpos, ny2);  Pss_hv(tpos, hv2);
            Pss_motion(tpos, (tw2.lr ? Tl135out : Tr135out));

            Pss_prvx(tpos, Psg_x(*now));
            Pss_prvy(tpos, Psg_y(*now));
            Pss_prvhv(tpos, Psg_hv(*now));
            Pss_dirl(tpos, nxtdir);

            prvdirl[nx2][ny2][hv2] = *tpos;

            PQ_cost_push(que, (cost){ nd, *tpos });
            return true;
        }
    }
    return false;
}
bool turn_V90(const Wall* wall, uint16_t dist[16][16][4], uint32_t prvdirl[16][16][4], PQ_cost* que, uint32_t *now, uint32_t *tpos, const TurnWall* tw1, TurnWall tw2, AbsDir nxtdir)
{
    tw2.diff_x += tw1->diff_x;  tw2.diff_y += tw1->diff_y;

    uint8_t nx1 = Psg_x(*now) + tw1->diff_x, ny1 = Psg_y(*now) + tw1->diff_y;
    uint8_t nx2 = Psg_x(*now) + tw2.diff_x,  ny2 = Psg_y(*now) + tw2.diff_y;
    uint8_t hv2 = Psg_hv(*now);

    if (!get_wall_abs(wall, nx1, ny1, tw1->lookdir) &&
        !get_wall_abs(wall, nx2, ny2, tw2.lookdir))
    {
        uint16_t nd = dist[Psg_x(*now)][Psg_y(*now)][Psg_hv(*now)] + V90;
        if (dist[nx2][ny2][hv2] > nd) {
            dist[nx2][ny2][hv2] = nd;

            Pss_x(tpos, nx2);  Pss_y(tpos, ny2);  Pss_hv(tpos, hv2);
            Pss_motion(tpos, (tw2.lr ? TlV90 : TrV90));

            Pss_prvx(tpos, Psg_x(*now));
            Pss_prvy(tpos, Psg_y(*now));
            Pss_prvhv(tpos, Psg_hv(*now));
            Pss_dirl(tpos, nxtdir);

            prvdirl[nx2][ny2][hv2] = *tpos;

            PQ_cost_push(que, (cost){ nd, *tpos });
            return true;
        }
    }
    return false;
}

uint16_t dijkstra(const Wall* wall ,uint8_t goalx, uint8_t goaly, uint8_t goalhv, bool acc, bool show) {
    uint16_t dist[16][16][4];
    MazeBit visited[4];
    uint32_t prvdirl[16][16][4];
    uint8_vector goals;
    uint8_vector_init(&goals);
    uint8_vector_push(&goals, xy_to_pos(7,7));
    genDIR45

    for (uint8_t x = 0; x < 16; x++) {
        for (uint8_t y = 0; y < 16; y++) {
            for (uint8_t i = 0; i < 4; i++) {
                dist[x][y][i] = 0xFFFF;
            }
        }
    }
    for (int8_t i = 0; i < 4; i++) {
        mazebit_zero(&visited[i]);
    }
    
    

    PQ_cost que;
    PQ_cost_init(&que);

    PQ_cost_push(&que, (cost){0, Ps_get(0,0,0,0,Nth,0,0,0)});
    dist[0][0][0] = 0;

    while (que.size != 0) {
        cost now = PQ_cost_pop(&que);
        uint32_t nowpos = now.snd;
        if (show) {
            printf("now: %d\n", dist[Psg_x(nowpos)][Psg_y(nowpos)][Psg_hv(nowpos)]);
            // Ps_print(nowpos);
            Ps_printAll(nowpos);
            // show_path_map(wall, dist, goalx, goaly, 0, &que);
            // print_wall(wall);
            printf("que size: %d\n", que.size);
            for (int16_t i = 0; i < que.size; i++) {
                Ps_print(que.value[i].snd);
            }
            printf("\n");
            
        }
        if (mazebit_get(&visited[Psg_hv(nowpos)], Psg_x(nowpos), Psg_y(nowpos))) {
            continue;
        } else {
            mazebit_set(&visited[Psg_hv(nowpos)], Psg_x(nowpos), Psg_y(nowpos), true);
        }
        // printf("true,nowpos %d\n", nowpos);
        // Ps_print(nowpos);

        uint32_t tpos = nowpos;
        uint8_t tcnt = 0;
        AbsDir nowdir = Psg_dirl(nowpos);

        if (Psg_hv(tpos) == 0 || Psg_hv(tpos) == 1) {
            while (true) {
                if (0 <= Psg_x(tpos) + dir45[nowdir][0] && Psg_x(tpos) + dir45[nowdir][0] < 16) {
                    Pss_x(&tpos, Psg_x(tpos) + dir45[nowdir][0]);
                } else {
                    break;
                }
                if (0 <= Psg_y(tpos) + dir45[nowdir][1] && Psg_y(tpos) + dir45[nowdir][1] < 16) {
                    Pss_y(&tpos, Psg_y(tpos) + dir45[nowdir][1]);
                } else {
                    break;
                }

                // printf("wall %d\n", !get_wall_abs(wall, Psg_x(tpos), Psg_y(tpos), nowdir%4));
                // printf("dist %d\n", dist[Psg_x(tpos)][Psg_y(tpos)][Psg_hv(tpos)] > dist[Psg_x(nowpos)][Psg_y(nowpos)][Psg_hv(nowpos)] + EX_STRAIGHT[tcnt]);
                // Ps_print(tpos);
                if ((!get_wall_abs(wall, Psg_x(tpos), Psg_y(tpos), nowdir%4) && dist[Psg_x(tpos)][Psg_y(tpos)][Psg_hv(tpos)] > dist[Psg_x(nowpos)][Psg_y(nowpos)][Psg_hv(nowpos)] + EX_STRAIGHT[tcnt])) {
                    dist[Psg_x(tpos)][Psg_y(tpos)][Psg_hv(tpos)] = dist[Psg_x(nowpos)][Psg_y(nowpos)][Psg_hv(nowpos)] + EX_STRAIGHT[tcnt];
                    Pss_motion(&tpos, T0grd | (2*tcnt));
                    Pss_prvx(&tpos, Psg_x(nowpos));
                    Pss_prvy(&tpos, Psg_y(nowpos));
                    Pss_prvhv(&tpos, Psg_hv(nowpos));
                    Pss_dirl(&tpos, Psg_dirl(nowpos));
                    prvdirl[Psg_x(tpos)][Psg_y(tpos)][Psg_hv(tpos)] = tpos;
                    PQ_cost_push(&que, (cost){dist[Psg_x(tpos)][Psg_y(tpos)][Psg_hv(tpos)], tpos});
                    tcnt++;
                    if (!acc) {break;}
                } else {
                    break;
                }
            }
        }

        bool flag = true;
        while (flag) {
            if (Psg_hv(tpos) == 2) {
                if (nowdir == NthEst)
                    flag = straight_dia(wall, dist, prvdirl, &que, &nowpos, &tpos, &Nth_R,  NthEst, tcnt);
                else if (nowdir == NthWst)
                    flag = straight_dia(wall, dist, prvdirl, &que, &nowpos, &tpos, &Nth_L,  NthWst, tcnt);
                else if (nowdir == SthWst)
                    flag = straight_dia(wall, dist, prvdirl, &que, &nowpos, &tpos, &Sth_R,  SthWst, tcnt);
                else if (nowdir == SthEst)
                    flag = straight_dia(wall, dist, prvdirl, &que, &nowpos, &tpos, &Sth_L,  SthEst, tcnt);
            } else if (Psg_hv(tpos) == 3) {
                if (nowdir == NthEst)
                    flag = straight_dia(wall, dist, prvdirl, &que, &nowpos, &tpos, &Est_L,  NthEst, tcnt);
                else if (nowdir == NthWst)
                    flag = straight_dia(wall, dist, prvdirl, &que, &nowpos, &tpos, &Wst_R,  NthWst, tcnt);
                else if (nowdir == SthWst)
                    flag = straight_dia(wall, dist, prvdirl, &que, &nowpos, &tpos, &Wst_L,  SthWst, tcnt);
                else if (nowdir == SthEst)
                    flag = straight_dia(wall, dist, prvdirl, &que, &nowpos, &tpos, &Est_R,  SthEst, tcnt);
            } else {
                break;
            }
            if (!acc) {
                break;
            }
            tcnt++;
        }

        if (nowdir == Est) {
            // S90 Right
            turn_grid_90(wall, dist, prvdirl, &que, &nowpos, &Est_R,  Sth);
            turn_grid_180(wall, dist, prvdirl, &que, &nowpos, &tpos, &Est_R, Sth_R, Wst);
            turn_135_in(wall, dist, prvdirl, &que, &nowpos, &tpos, &Est_R,  Sth_R,  SthWst);
            turn_45_in (wall, dist, prvdirl, &que, &nowpos, &tpos, &Est_R,           SthEst);
            // S90 Left
            turn_grid_90(wall, dist, prvdirl, &que, &nowpos, &Est_L,  Nth);
            turn_grid_180(wall, dist, prvdirl, &que, &nowpos, &tpos, &Est_L, Nth_L, Wst);
            turn_135_in (wall, dist, prvdirl, &que, &nowpos, &tpos, &Est_L, Nth_L, NthWst);
            turn_45_in  (wall, dist, prvdirl, &que, &nowpos, &tpos, &Est_L,          NthEst);

        } else if (nowdir == Nth) {
            // S90 Right
            turn_grid_90(wall, dist, prvdirl, &que, &nowpos, &Nth_R, Est);
            turn_grid_180(wall, dist, prvdirl, &que, &nowpos, &tpos, &Nth_R, Est_R, Sth);
            turn_135_in (wall, dist, prvdirl, &que, &nowpos, &tpos, &Nth_R, Est_R,  SthEst);
            turn_45_in  (wall, dist, prvdirl, &que, &nowpos, &tpos, &Nth_R,          NthEst);
            // S90 Left
            turn_grid_90(wall, dist, prvdirl, &que, &nowpos, &Nth_L, Wst);
            turn_grid_180(wall, dist, prvdirl, &que, &nowpos, &tpos, &Nth_L, Wst_L, Sth);
            turn_135_in (wall, dist, prvdirl, &que, &nowpos, &tpos, &Nth_L, Wst_L,  SthWst);
            turn_45_in  (wall, dist, prvdirl, &que, &nowpos, &tpos, &Nth_L,          NthWst);

        } else if (nowdir == Wst) {
            // S90 Right
            turn_grid_90(wall, dist, prvdirl, &que, &nowpos, &Wst_R, Nth);
            turn_grid_180(wall, dist, prvdirl, &que, &nowpos, &tpos, &Wst_R, Nth_R, Est);
            turn_135_in (wall, dist, prvdirl, &que, &nowpos, &tpos, &Wst_R, Nth_R,  NthEst);
            turn_45_in  (wall, dist, prvdirl, &que, &nowpos, &tpos, &Wst_R,          NthWst);
            // S90 Left
            turn_grid_90(wall, dist, prvdirl, &que, &nowpos, &Wst_L, Sth);
            turn_grid_180(wall, dist, prvdirl, &que, &nowpos, &tpos, &Wst_L, Sth_L, Est);
            turn_135_in (wall, dist, prvdirl, &que, &nowpos, &tpos, &Wst_L, Sth_L,  SthEst);
            turn_45_in  (wall, dist, prvdirl, &que, &nowpos, &tpos, &Wst_L,          SthWst);

        } else if (nowdir == Sth) {
            // S90 Right
            turn_grid_90(wall, dist, prvdirl, &que, &nowpos, &Sth_R, Wst);
            turn_grid_180(wall, dist, prvdirl, &que, &nowpos, &tpos, &Sth_R, Wst_R, Nth);
            turn_135_in (wall, dist, prvdirl, &que, &nowpos, &tpos, &Sth_R, Wst_R,  NthWst);
            turn_45_in  (wall, dist, prvdirl, &que, &nowpos, &tpos, &Sth_R,          SthWst);
            // S90 Left
            turn_grid_90(wall, dist, prvdirl, &que, &nowpos, &Sth_L, Est);
            turn_grid_180(wall, dist, prvdirl, &que, &nowpos, &tpos, &Sth_L, Est_L, Nth);
            turn_135_in (wall, dist, prvdirl, &que, &nowpos, &tpos, &Sth_L, Est_L,  NthEst);
            turn_45_in  (wall, dist, prvdirl, &que, &nowpos, &tpos, &Sth_L,          SthEst);

        } else if (nowdir == NthEst) {
            if (Psg_hv(nowpos) == 2) {
                // Right (hv: 2 -> 1)
                turn_45_out (wall, dist, prvdirl, &que, &nowpos, &tpos, &Nth_R, Est);
                turn_135_out(wall, dist, prvdirl, &que, &nowpos, &tpos, &Nth_R, Est_R, Sth);
                turn_V90    (wall, dist, prvdirl, &que, &nowpos, &tpos, &Nth_R, Est_R, SthEst);
            } else {
                // Left (hv: 3 -> 0)
                turn_45_out (wall, dist, prvdirl, &que, &nowpos, &tpos, &Est_L, Nth);
                turn_135_out(wall, dist, prvdirl, &que, &nowpos, &tpos, &Est_L, Nth_L, Wst);
                turn_V90    (wall, dist, prvdirl, &que, &nowpos, &tpos, &Est_L, Nth_L, NthWst);
            }
        } else if (nowdir == NthWst) {
            if (Psg_hv(nowpos) == 2) {
                // Left (hv: 2 -> 1)
                turn_45_out (wall, dist, prvdirl, &que, &nowpos, &tpos, &Nth_L, Wst);
                turn_135_out(wall, dist, prvdirl, &que, &nowpos, &tpos, &Nth_L, Wst_L, Sth);
                turn_V90    (wall, dist, prvdirl, &que, &nowpos, &tpos, &Nth_L, Wst_L, SthWst);
            } else {
                // Right (hv: 3 -> 0)
                turn_45_out (wall, dist, prvdirl, &que, &nowpos, &tpos, &Wst_R, Nth);
                turn_135_out(wall, dist, prvdirl, &que, &nowpos, &tpos, &Wst_R, Nth_R, Est);
                turn_V90    (wall, dist, prvdirl, &que, &nowpos, &tpos, &Wst_R, Nth_R, NthEst);
            }
        } else if (nowdir == SthWst) {
            if (Psg_hv(nowpos) == 2) {
                // Right (hv: 2 -> 1)
                turn_45_out (wall, dist, prvdirl, &que, &nowpos, &tpos, &Sth_R, Wst);
                turn_135_out(wall, dist, prvdirl, &que, &nowpos, &tpos, &Sth_R, Wst_R, Nth);
                turn_V90    (wall, dist, prvdirl, &que, &nowpos, &tpos, &Sth_R, Wst_R, NthWst);
            } else {
                // Left (hv: 3 -> 0)
                turn_45_out (wall, dist, prvdirl, &que, &nowpos, &tpos, &Wst_L, Sth);
                turn_135_out(wall, dist, prvdirl, &que, &nowpos, &tpos, &Wst_L, Sth_L, Est);
                turn_V90    (wall, dist, prvdirl, &que, &nowpos, &tpos, &Wst_L, Sth_L, SthEst);
            }
        } else if (nowdir == SthEst) {
            if (Psg_hv(nowpos) == 2) {
                // Left (hv: 2 -> 1)
                turn_45_out (wall, dist, prvdirl, &que, &nowpos, &tpos, &Sth_L, Est);
                turn_135_out(wall, dist, prvdirl, &que, &nowpos, &tpos, &Sth_L, Est_L, Nth);
                turn_V90    (wall, dist, prvdirl, &que, &nowpos, &tpos, &Sth_L, Est_L, NthEst);
            } else {
                // Right (hv: 3 -> 0)
                turn_45_out (wall, dist, prvdirl, &que, &nowpos, &tpos, &Est_R, Sth);
                turn_135_out(wall, dist, prvdirl, &que, &nowpos, &tpos, &Est_R, Sth_R, Wst);
                turn_V90    (wall, dist, prvdirl, &que, &nowpos, &tpos, &Est_R, Sth_R, SthWst);
            }
        }
    }

    uint16_t min_dist = 0xFFFF;
    uint8_t nowx, nowy;
    uint8_t gx,gy,ghv;
    if (goalhv == DONT_CHOOSE_HV) {
        for (int16_t cnt = 0; cnt < goals.size; cnt++) {
            nowx = goals.value[cnt] & 0xF;
            nowy = (goals.value[cnt] >> 4) & 0xF;
            if (dist[nowx][nowy][0] < min_dist) {
                printf("%2d %2d %d, dist:%d\n", nowx, nowy, 0, dist[nowx][nowy][0]);
                min_dist = dist[nowx][nowy][0];
                gx = nowx;
                gy = nowy;
                ghv = 0;
            }
            if (dist[nowx][nowy][1] < min_dist) {
                printf("%2d %2d %d, dist:%d\n", nowx, nowy, 1, dist[nowx][nowy][1]);
                min_dist = dist[nowx][nowy][1];
                gx = nowx;
                gy = nowy;
                ghv = 1;
            }
            if (1 <= nowy) {
                if (dist[nowx][nowy-1][0] < min_dist) {
                    printf("%2d %2d %d, dist:%d\n", nowx, nowy-1, 0, dist[nowx][nowy-1][0]);
                    min_dist = dist[nowx][nowy-1][0];
                    gx = nowx;
                    gy = nowy-1;
                    ghv = 0;
                }
            }
            if (1 <= nowx) {
                if (dist[nowx-1][nowy][1] < min_dist) {
                    printf("%2d %2d %d, dist:%d\n", nowx-1, nowy, 0, dist[nowx-1][nowy][1]);
                    min_dist = dist[nowx-1][nowy][1];
                    gx = nowx-1;
                    gy = nowy;
                    ghv = 1;
                }            
            }
        }
    } else {
        gx = goalx;
        gy = goaly;
        ghv = goalhv;
    }
    

    printf("goal: %2d %2d %d\n", gx, gy, ghv);
    dijkstra_gen_path(prvdirl, gx, gy, ghv);
    // dijkstra_show_path(prvdirl, goalx, goaly, goalhv);

    return dist[gx][gy][ghv];

    // return 0;
}
















uint8_vector solver_time_based_dijekstra_init(void) {
    uint8_vector_init(&action_queue);

    // Dijkstra経路計算を実行 (ゴール: (7,7), HV=自動選択, 加速, 表示)
    dijkstra(&wallone, 7, 7, DONT_CHOOSE_HV, true, false);

    uint8_t s = temp_dijkstra_rev.size;
    for (int8_t i = 0; i < s; i++) {
        uint8_vector_push(&action_queue, uint8_deque_pop_front(&temp_dijkstra_rev));
    }

    uint8_vector_push(&action_queue, ACT_FINISH);

    if (ACT_MOVE_0SEC <= action_queue.value[0] <= ACT_MOVE_32SEC) {
        action_queue.value[0] += 2;
    }

    return action_queue;
}

uint8_vector solver_time_based_dijekstra(bool left, bool front, bool right) {
    uint8_vector_init(&action_queue);

    return action_queue;
}