#ifndef CHESS_CHESS_H
#define CHESS_CHESS_H

#include <stdbool.h>
#include <stdint.h>

// 50 move rule, max 255
#define FMR 50

#define ch_pos(x, y) ((x) + (y) * 8)
#define ch_move(x1, y1, x2, y2) ((x1) + (y1) * 8 + (x2) * 64 + (y2) * 512)

#define _p(x, y) board->pieces[ch_pos(x, y)]
#define _pb(board, x, y) board->pieces[ch_pos(x, y)]

typedef struct {
    char pieces[64];  // ' ' for empty

    bool turn;

    bool castle_wk;
    bool castle_wq;
    bool castle_bk;
    bool castle_bq;

    // 8 if there is no en passant, 0-8
    uint8_t en_passant;

    // todo: fifty rule move, draw by repetition

    uint8_t* render_highlight;
} Board;

void init_chess();
Board* make_board();
void free_board();
void print_board(Board* board);
void board_set_highlight(Board* board, uint8_t* highlight);
void board_highlight_moves(Board* board, uint8_t x, uint8_t y);
bool is_white(char piece);
const char* piece_char(char piece);
void read_fen(Board* board, char* str);
bool can_move(Board* board, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
bool can_basic_move(Board* board, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
bool is_square_visitable(Board* board, uint8_t X, uint8_t Y, bool white);
bool can_king_captured(Board* board, bool white);
bool move_piece(Board* board, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, char promotion);
bool move_notation(Board* board, char* notation, char promotion);

#endif