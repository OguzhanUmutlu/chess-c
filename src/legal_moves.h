#ifndef LEGAL_MOVES_H
#define LEGAL_MOVES_H

#include "chess.h"

void init_piece_costs();
uint16_t* moves_all_check(Board* board);
uint8_t* moves_piece_check(Board* board, uint8_t x, uint8_t y);
uint8_t* moves_piece(Board* board, uint8_t x, uint8_t y);
uint8_t* moves_r(Board* board, uint8_t x, uint8_t y);
uint8_t* moves_n(Board* board, uint8_t x, uint8_t y);
uint8_t* moves_b(Board* board, uint8_t x, uint8_t y);
uint8_t* moves_q(Board* board, uint8_t x, uint8_t y);
uint8_t* moves_k(Board* board, uint8_t x, uint8_t y);
uint8_t* moves_p(Board* board, uint8_t x, uint8_t y);
void print_square_list(uint8_t* list);
void print_move_list(uint16_t* list);

#endif