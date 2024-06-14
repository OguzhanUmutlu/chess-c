#include "legal_moves.h"

#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

const uint8_t COST_R = 4;
const uint8_t COST_N = 8;
const uint8_t COST_B = 13;
const uint8_t COST_Q = 21;
const uint8_t COST_K = 8;
const uint8_t COST_P = 4;

static uint8_t PIECE_COSTS[114];

void init_piece_costs() {
    PIECE_COSTS['r'] = COST_R;
    PIECE_COSTS['R'] = COST_R;
    PIECE_COSTS['n'] = COST_N;
    PIECE_COSTS['N'] = COST_N;
    PIECE_COSTS['b'] = COST_B;
    PIECE_COSTS['B'] = COST_B;
    PIECE_COSTS['q'] = COST_Q;
    PIECE_COSTS['Q'] = COST_Q;
    PIECE_COSTS['k'] = COST_K;
    PIECE_COSTS['K'] = COST_K;
    PIECE_COSTS['p'] = COST_P;
    PIECE_COSTS['P'] = COST_P;
    PIECE_COSTS[' '] = 0;
}

#define mov_st(max)                                                 \
    uint8_t* moves = (uint8_t*)malloc((max + 1) * sizeof(uint8_t)); \
    if (moves == NULL) {                                            \
        malloc_fail();                                              \
        return NULL;                                                \
    }                                                               \
    char piece = _p(x, y);                                          \
    bool color = is_white(piece);                                   \
    int _j = 0

#define mov_end(max)     \
    moves[_j] = ui8_max; \
    return moves

#define mov_add(x, y) moves[_j++] = ch_pos(x, y)
#define mov_add_cap_mov(x, y)                       \
    do {                                            \
        char _pt = _p(x, y);                        \
        if (_pt == ' ' || is_white(_pt) != color) { \
            mov_add(x, y);                          \
        }                                           \
    } while (0)

#define mov_add_cap(x, y)                           \
    do {                                            \
        char _pt = _p(x, y);                        \
        if (_pt != ' ' && is_white(_pt) != color) { \
            mov_add(x, y);                          \
        }                                           \
    } while (0)

#define mov_add_mov(x, y)  \
    if (_p(x, y) == ' ') { \
        mov_add(x, y);     \
    }

#define mov_line(dx, dy)                                 \
    do {                                                 \
        int x0 = x + dx;                                 \
        int y0 = y + dy;                                 \
        while (x0 >= 0 && y0 >= 0 && x0 < 8 && y0 < 8) { \
            char _pt = _p(x0, y0);                       \
            if (_pt != ' ') {                            \
                if (is_white(_pt) == color) break;       \
                mov_add(x0, y0);                         \
                break;                                   \
            }                                            \
            mov_add(x0, y0);                             \
            x0 += dx;                                    \
            y0 += dy;                                    \
        }                                                \
    } while (0)

uint16_t* moves_all_check(Board* board) {
    int cost = 0;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            char piece = _p(x, y);
            if (piece == ' ' || is_white(piece) != board->turn) continue;
            cost += PIECE_COSTS[piece];
        }
    }
    uint16_t* moves = (uint16_t*)malloc(cost * sizeof(uint16_t));
    if (moves == NULL) {
        malloc_fail();
        return NULL;
    }
    int i = 0;
    uint8_t* moves_p;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            char piece = _p(x, y);
            if (piece == ' ' || is_white(piece) != board->turn) continue;
            moves_p = moves_piece_check(board, x, y);
            int pos = ch_pos(x, y);
            for (int j = 0; moves_p[j] != ui8_max; j++) {
                moves[i++] = moves_p[j] * 8 * 8 + pos;
            }
            free(moves_p);
        }
    }
    moves[i] = ui16_max;
    return moves;
}

uint8_t* moves_piece_check(Board* board, uint8_t x, uint8_t y) {
    int j = 0;
    uint8_t* moves_all = moves_piece(board, x, y);
    uint8_t* moves = (uint8_t*)malloc((PIECE_COSTS[_p(x, y)] + 1) * sizeof(uint8_t));
    if (moves == NULL) {
        malloc_fail();
        return NULL;
    }
    char piece = _p(x, y);
    bool color = is_white(piece);
    for (int i = 0; moves_all[i] != ui8_max; i++) {
        uint8_t mov = moves_all[i];
        uint8_t x2 = mov % 8;
        uint8_t y2 = mov / 8;

        char target = _p(x2, y2);
        bool en_pas = (piece == 'p' || piece == 'P') && x != x2 && target == ' ';

        _p(x, y) = ' ';
        _p(x2, y2) = piece;
        if (en_pas) {
            _p(x2, y) = ' ';
        }

        if (!is_under_check(board, color) && !__invalid_castling(board, piece, x, y, x2, y2)) {
            moves[j++] = mov;
        }

        _p(x, y) = piece;
        _p(x2, y2) = target;
        if (en_pas) {
            _p(x2, y) = color ? 'p' : 'P';
        }
    }
    moves[j] = ui8_max;
    free(moves_all);
    return moves;
}

uint8_t* moves_piece(Board* board, uint8_t x, uint8_t y) {
    switch (_p(x, y)) {
        case 'r':
        case 'R':
            return moves_r(board, x, y);
        case 'n':
        case 'N':
            return moves_n(board, x, y);
        case 'b':
        case 'B':
            return moves_b(board, x, y);
        case 'q':
        case 'Q':
            return moves_q(board, x, y);
        case 'k':
        case 'K':
            return moves_k(board, x, y);
        case 'p':
        case 'P':
            return moves_p(board, x, y);
        case ' ': {
            uint8_t* moves = (uint8_t*)malloc(sizeof(uint8_t));
            if (moves == NULL) {
                malloc_fail();
                return NULL;
            }
            moves[0] = ui8_max;
            return moves;
        }
        default:
            printf("Something went wrong in moves_piece()");
            return NULL;
    }
}

uint8_t* moves_r(Board* board, uint8_t x, uint8_t y) {
    mov_st(COST_R);
    mov_line(1, 0);
    mov_line(-1, 0);
    mov_line(0, 1);
    mov_line(0, -1);
    mov_end(COST_R);
}

uint8_t* moves_n(Board* board, uint8_t x, uint8_t y) {
    mov_st(COST_N);
    if (x <= 5 && y <= 6) mov_add_cap_mov(x + 2, y + 1);
    if (x >= 2 && y <= 6) mov_add_cap_mov(x - 2, y + 1);
    if (x <= 5 && y >= 1) mov_add_cap_mov(x + 2, y - 1);
    if (x >= 2 && y >= 1) mov_add_cap_mov(x - 2, y - 1);
    if (x <= 6 && y <= 5) mov_add_cap_mov(x + 1, y + 2);
    if (x >= 1 && y <= 5) mov_add_cap_mov(x - 1, y + 2);
    if (x <= 6 && y >= 2) mov_add_cap_mov(x + 1, y - 2);
    if (x >= 1 && y >= 2) mov_add_cap_mov(x - 1, y - 2);
    mov_end(COST_N);
}

uint8_t* moves_b(Board* board, uint8_t x, uint8_t y) {
    mov_st(COST_B);
    mov_line(1, 1);
    mov_line(-1, -1);
    mov_line(1, -1);
    mov_line(-1, 1);
    mov_end(COST_B);
}

uint8_t* moves_q(Board* board, uint8_t x, uint8_t y) {
    mov_st(COST_Q);
    mov_line(1, 0);
    mov_line(-1, 0);
    mov_line(0, 1);
    mov_line(0, -1);

    mov_line(1, 1);
    mov_line(-1, -1);
    mov_line(1, -1);
    mov_line(-1, 1);
    mov_end(COST_Q);
}

uint8_t* moves_k(Board* board, uint8_t x, uint8_t y) {
    mov_st(COST_K);
    if (x >= 1 && y >= 1) mov_add_cap_mov(x - 1, y - 1);
    if (y >= 1) mov_add_cap_mov(x, y - 1);
    if (x <= 6 && y >= 1) mov_add_cap_mov(x + 1, y - 1);
    if (x >= 1) mov_add_cap_mov(x - 1, y);
    if (x <= 6) mov_add_cap_mov(x + 1, y);
    if (x >= 1 && y <= 6) mov_add_cap_mov(x - 1, y + 1);
    if (y <= 6) mov_add_cap_mov(x, y + 1);
    if (x <= 6 && y <= 6) mov_add_cap_mov(x + 1, y + 1);

    bool ks = color ? board->castle_wk : board->castle_bk;
    bool qs = color ? board->castle_wq : board->castle_bq;

    // King side castling
    bool can_kingside_castle = ks &&
                               _p(x + 1, y) == ' ' &&
                               _p(x + 2, y) == ' ';

    if (can_kingside_castle) {
        mov_add(x + 2, y);
    }

    // Queen side castling
    bool can_queenside_castle = qs &&
                                _p(x - 1, y) == ' ' &&
                                _p(x - 2, y) == ' ' &&
                                _p(x - 3, y) == ' ';

    if (can_queenside_castle) {
        mov_add(x - 2, y);
    }

    mov_end(COST_K);
}

uint8_t* moves_p(Board* board, uint8_t x, uint8_t y) {
    mov_st(COST_P);
    int pw_move = -2 * color + 1;
    int pw_move_2 = pw_move * 2;

    // checking if the pawn is in the starting position (white: y=6, black: y=1)
    if (1 + color * 5 == y && _p(x, y + pw_move_2) == ' ') {
        mov_add(x, y + pw_move_2);
    }

    // 1 square move
    if (_p(x, y + pw_move) == ' ') {
        mov_add(x, y + pw_move);
    }

    // captures and en passant
    char cpt_r = x == 7 ? 0 : _p(x + 1, y + pw_move);
    char cpt_l = x == 0 ? 0 : _p(x - 1, y + pw_move);
    uint8_t en_y = color ? 3 : 4;

    if (cpt_r) {
        mov_add_cap(x + 1, y + pw_move);
        if (board->en_passant == x + 1 && y == en_y) {
            mov_add_mov(x + 1, y + pw_move);
        }
    }

    if (cpt_l) {
        mov_add_cap(x - 1, y + pw_move);
        if (board->en_passant == x - 1 && y == en_y) {
            mov_add_mov(x - 1, y + pw_move);
        }
    }

    mov_end(COST_P);
}

void print_square_list(uint8_t* list) {
    printf("{ ");
    for (int i = 0; list[i] != ui8_max; i++) {
        printf("(%d, %d)", list[i] % 8, list[i] / 8);
        if (list[i + 1] != ui8_max) printf(", ");
    }
    printf(" }\n");
}

void print_move_list(uint16_t* list) {
    printf("{ ");
    for (int i = 0; list[i] != ui16_max; i++) {
        uint16_t n = list[i];
        uint8_t a = n % 8;
        n /= 8;
        uint8_t b = n % 8;
        n /= 8;
        uint8_t c = n % 8;
        n /= 8;
        uint8_t d = n % 8;
        printf("( (%d, %d), (%d, %d) )", a, b, c, d);
        if (list[i + 1] != ui16_max) printf(", ");
    }
    printf(" }\n");
}