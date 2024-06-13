#include "chess.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "legal_moves.h"
#include "utils.h"

void init_chess() {
    init_piece_costs();
}

// rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR
void read_fen(Board* board, char* str) {
    size_t len = strlen(str);
    int j = 0;
    for (int i = 0; i < len; i++) {
        char c = str[i];
        if (c == '/') {
            continue;
        }
        if (j > 63) return;
        if (c > '0' && c < '9') {  // 1-8
            c -= '0';
            for (int k = 0; k < c; k++) {
                board->pieces[j++] = ' ';
            }
        } else {
            board->pieces[j++] = c;
        }
    }
}

Board* make_board() {
    Board* board = (Board*)malloc(sizeof(Board));
    read_fen(board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    board->turn = 1;
    board->castle_bk = 1;
    board->castle_bq = 1;
    board->castle_wk = 1;
    board->castle_wq = 1;
    board->en_passant = 0;
    board->render_highlight = NULL;
    return board;
}

void board_set_highlight(Board* board, uint8_t* highlight) {
    if (board->render_highlight != NULL) free(board->render_highlight);
    board->render_highlight = highlight;
}

void board_highlight_moves(Board* board, uint8_t x, uint8_t y) {
    board_set_highlight(board, moves_piece_check(board, x, y));
}

void free_board(Board* board) {
    if (board->render_highlight != NULL) free(board->render_highlight);
    free(board);
}

bool can_move_base(Board* board, uint8_t x2, uint8_t y2, uint8_t* moves) {
    uint8_t move = ch_pos(x2, y2);
    int i = 0;

    for (int i = 0; moves[i] != 255; i++) {
        if (moves[i] == move) {
            free(moves);
            return true;
        }
    }

    free(moves);
    return false;
}

bool is_square_visitable(Board* board, uint8_t X, uint8_t Y, bool white) {
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            char piece = _p(x, y);
            if (is_white(piece) != white) continue;
            if (can_basic_move(board, x, y, X, Y)) return true;
        }
    }
    return false;
}

bool can_king_captured(Board* board, bool white) {
    char search = 'k';
    if (white) search = 'K';
    uint8_t kx = 10;
    uint8_t ky = 0;
    for (int x = 0; x < 8; x++) {
        if (kx != 10) break;
        for (int y = 0; y < 8; y++) {
            char piece = _p(x, y);
            if (piece == search) {
                kx = x;
                ky = y;
                break;
            }
        }
    }
    if (kx == 10) return false;
    return is_square_visitable(board, kx, ky, 1 - white);
}

bool can_basic_move(Board* board, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
    uint8_t* moves = moves_piece(board, x1, y1);
    return can_move_base(board, x2, y2, moves);
}

bool can_move(Board* board, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
    uint8_t* moves = moves_piece_check(board, x1, y1);
    return can_move_base(board, x2, y2, moves);
}

bool move_notation(Board* board, char* notation, char promotion) {
    return move_piece(board, notation[0] - 'a', 8 - (notation[1] - '0'), notation[3] - 'a', 8 - (notation[4] - '0'), promotion);
}

// move = [x1, y1, x2, y2]
bool move_piece(Board* board, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, char promotion) {
    char piece = _p(x1, y1);
    if (piece == ' ') return false;

    // check if its turn
    bool color = is_white(piece);
    if (board->turn != color) return false;

    if (!can_basic_move(board, x1, y1, x2, y2)) return false;

    char target = _p(x2, y2);
    _p(x1, y1) = ' ';
    _p(x2, y2) = piece;

    if (can_king_captured(board, color)) {
        _p(x1, y1) = piece;
        _p(x2, y2) = target;
        return false;
    }

    board->turn = 1 - board->turn;

    if ((piece == 'p' && y2 - y1 == 2) || (piece == 'P' && y1 - y2 == 2)) {
        board->en_passant = x1;
    } else {
        board->en_passant = 0;
    }

    if (piece == 'k' && x2 - x1 == 2) {
        _p(7, y2) = ' ';
        _p(x2 - 1, y2) = 'r';
    }

    if (piece == 'k' && x1 - x2 == 2) {
        _p(0, y2) = ' ';
        _p(x2 + 1, y2) = 'r';
    }

    if (piece == 'K' && x2 - x1 == 2) {
        _p(7, y2) = ' ';
        _p(x2 - 1, y2) = 'R';
    }

    if (piece == 'K' && x1 - x2 == 2) {
        _p(0, y2) = ' ';
        _p(x2 + 1, y2) = 'R';
    }

    if (_p(4, 0) != 'k') {
        board->castle_bk = 0;
        board->castle_bq = 0;
    } else {
        if (_p(0, 0) != 'r') board->castle_bq = 0;
        if (_p(7, 0) != 'r') board->castle_bk = 0;
    }

    if (_p(4, 7) != 'K') {
        board->castle_wk = 0;
        board->castle_wq = 0;
    } else {
        if (_p(0, 0) != 'R') board->castle_wq = 0;
        if (_p(7, 7) != 'R') board->castle_wk = 0;
    }

    if ((piece == 'p' && y2 == 7) || (piece == 'P' && y2 == 0)) {
        _p(x2, y2) = promotion;
    }

    return true;
}

bool is_white(char piece) {
    return piece < 'a';
}

const char* piece_char(char piece) {
    switch (piece) {
        case ' ':
            return " ";
        case 'k':
            return _BK;
        case 'q':
            return _BQ;
        case 'r':
            return _BR;
        case 'b':
            return _BB;
        case 'n':
            return _BN;
        case 'p':
            return _BP;
        case 'K':
            return _WK;
        case 'Q':
            return _WQ;
        case 'R':
            return _WR;
        case 'B':
            return _WB;
        case 'N':
            return _WN;
        case 'P':
            return _WP;
        default:
            return NULL;
    }
}

bool is_highlighting(Board* board, uint8_t pos) {
    if (board->render_highlight == NULL) return false;

    for (int i = 0; board->render_highlight[i] != 255; i++) {
        if (pos == board->render_highlight[i]) return true;
    }

    return false;
}

void print_board(Board* board) {
    printf("  a b c d e f g h");
    for (int i = 0; i < 64; i++) {
        if (i % 8 == 0) printf(C_RES "\n%d ", 8 - i / 8);
        char piece = board->pieces[i];
        char* cb = (i + i / 8) % 2 == 0 ? B_BLU : B_B_WHT;
        if (is_highlighting(board, i)) {
            cb = B_RED;
        }
        char* cf = F_WHT;
        if (is_white(piece)) cf = F_BLK;
        printf("%s%s%s ", cb, cf, piece_char(piece));
    }
    printf(C_RES "\n");
}