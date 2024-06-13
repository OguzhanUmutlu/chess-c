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

// rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq e3 0
// position, whom to play, en passant target square, half moves with no captures/pawn moves(=0)
char* generate_fen(Board* board) {
    // pppppppp/pppppppp/pppppppp/pppppppp/pppppppp/pppppppp/pppppppp/pppppppp w KQkq e3 99
    // so max(even impossible) length = 84
    char* fen = (char*)malloc(84 * sizeof(char));
    int empty_counter = 0;
    int j = 0;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            char piece = _p(x, y);
            if (piece == ' ') {
                empty_counter++;
                if (x == 7) {
                    fen[j++] = '0' + empty_counter;
                    empty_counter = 0;
                }
                continue;
            }
            if (empty_counter > 0) {
                fen[j++] = '0' + empty_counter;
                empty_counter = 0;
            }
            fen[j++] = piece;
        }
        fen[j++] = y != 7 ? '/' : ' ';
    }
    fen[j++] = board->turn ? 'w' : 'b';
    fen[j++] = ' ';
    if (board->castle_wk) fen[j++] = 'K';
    if (board->castle_wq) fen[j++] = 'Q';
    if (board->castle_bk) fen[j++] = 'k';
    if (board->castle_bq) fen[j++] = 'q';
    if (!board->castle_wk && !board->castle_wq && !board->castle_bk && !board->castle_bq) {
        fen[j++] = '-';
    }
    fen[j++] = ' ';
    fen[j++] = board->en_passant == 8 ? '-' : ('a' + board->en_passant);
    if (board->en_passant != 8) {
        fen[j++] = board->turn ? '6' : '3';
    }
    fen[j++] = '\0';
    return fen;
}

void read_fen(Board* board, char* str) {
    board->turn = 1;
    board->castle_bk = 0;
    board->castle_bq = 0;
    board->castle_wk = 0;
    board->castle_wq = 0;
    board->en_passant = 8;
    board->render_highlight = NULL;
    board->fifty_move_rule = 0;

    size_t len = strlen(str);

    int j = 0, i = 0;

    for (; i < len; i++) {
        char c = str[i];
        if (c == ' ') break;
        if (c == '/') {
            continue;
        }
        if (j > 63) break;
        if (c > '0' && c < '9') {  // 1-8
            c -= '0';
            for (int k = 0; k < c; k++) {
                board->pieces[j++] = ' ';
            }
        } else {
            board->pieces[j++] = c;
        }
    }

    for (; j < 64; j++) {
        board->pieces[j] = ' ';
    }

    i++;
    board->turn = str[i++] == 'w';
    i++;

    if (str[i] != '-') {
        for (; i < len; i++) {
            if (str[i] == ' ') break;
            if (str[i] == 'K') board->castle_wk = 1;
            if (str[i] == 'Q') board->castle_wq = 1;
            if (str[i] == 'k') board->castle_bk = 1;
            if (str[i] == 'q') board->castle_bq = 1;
        }
    } else {
        i += 2;
    }

    if (str[i] != '-') {
        board->en_passant = str[i] - 'a';
        i += 3;
    } else {
        i += 2;
    }

    if (str[i + 1] >= '0' && str[i + 1] <= '9') {
        board->fifty_move_rule = str[i + 1] - '0' + 10 * (str[i] - '0');
    } else {
        board->fifty_move_rule = str[i] - '0';
    }

    update_castling_rights(board);
}

Board* make_board() {
    Board* board = (Board*)malloc(sizeof(Board));
    if (board == NULL) {
        malloc_fail();
        return NULL;
    }

    board->fens = NULL;
    board->fens_size = 0;

    read_fen(board, DEFAULT_FEN);

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
    // free(board); this causes free(): invalid pointer ?
}

bool can_move_base(Board* board, uint8_t x2, uint8_t y2, uint8_t* moves) {
    uint8_t move = ch_pos(x2, y2);
    int i = 0;

    for (int i = 0; moves[i] != ui8_max; i++) {
        if (moves[i] == move) {
            return true;
        }
    }

    return false;
}

bool is_square_visitable(Board* board, uint8_t X, uint8_t Y, bool white) {
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            char piece = _p(x, y);
            if (piece == ' ' || is_white(piece) != white) continue;
            if (can_basic_move(board, x, y, X, Y)) return true;
        }
    }
    return false;
}

bool is_under_check(Board* board, bool white) {
    char search = white ? 'K' : 'k';
    uint8_t kx = 10;
    uint8_t ky = 0;
    for (uint8_t x = 0; x < 8; x++) {
        if (kx != 10) break;
        for (uint8_t y = 0; y < 8; y++) {
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
    bool v = can_move_base(board, x2, y2, moves);
    free(moves);
    return v;
}

bool can_move(Board* board, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
    uint8_t* moves = moves_piece_check(board, x1, y1);
    bool v = can_move_base(board, x2, y2, moves);
    free(moves);
    return v;
}

bool move_notation(Board* board, char* notation, char promotion) {
    return move_piece(board, notation[0] - 'a', 8 - (notation[1] - '0'), notation[3] - 'a', 8 - (notation[4] - '0'), promotion);
}

void update_castling_rights(Board* board) {
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
}

bool move_piece(Board* board, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, char promotion) {
    char piece = _p(x1, y1);
    if (piece == ' ') return false;

    // check if its' turn
    bool color = is_white(piece);
    if (board->turn != color) return false;

    if (!can_basic_move(board, x1, y1, x2, y2)) return false;

    char target = _p(x2, y2);
    _p(x1, y1) = ' ';
    _p(x2, y2) = piece;

    if (is_under_check(board, color) || __invalid_castling(board, piece, x1, y1, x2, y2)) {
        _p(x1, y1) = piece;
        _p(x2, y2) = target;
        return false;
    }

    board->turn = 1 - board->turn;
    if (target == ' ' && piece != 'p' && piece != 'P') {
        board->fifty_move_rule++;
    } else {
        board->fifty_move_rule = 0;
    }

    if ((piece == 'p' && y2 - y1 == 2) || (piece == 'P' && y1 - y2 == 2)) {
        board->en_passant = x1;
    } else {
        board->en_passant = 8;
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

    update_castling_rights(board);

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

    for (int i = 0; board->render_highlight[i] != ui8_max; i++) {
        if (pos == board->render_highlight[i]) return true;
    }

    return false;
}

chess_status get_game_status(Board* board) {
    if (board->fifty_move_rule >= 100) return FIFTY_MOVE_RULE;
    bool is_check = is_under_check(board, board->turn);
    bool has_moves = moves_all_check(board)[0] != ui16_max;
    if (!has_moves) {
        return is_check ? CHECKMATE : STALEMATE;
    }
    bool minorW = false;
    bool minorB = false;
    for (int i = 0; i < 64; i++) {
        char piece = board->pieces[i];
        if (piece == 'k' || piece == 'K' || piece == ' ') continue;
        if (piece == 'q' || piece == 'Q' || piece == 'r' || piece == 'R' || piece == 'p' || piece == 'P') {
            return PLAYING;
        }
        if (piece == 'n' || piece == 'b') {
            if (minorB) return PLAYING;
            minorB = true;
            continue;
        }
        if (piece == 'N' || piece == 'B') {
            if (minorW) return PLAYING;
            minorW = true;
            continue;
        }
    }
    return INSUFFICIENT_MATERIAL;
}

void print_board(Board* board) {
    printf("  a b c d e f g h");
    for (int i = 0; i < 64; i++) {
        if (i % 8 == 0) printf(C_RES "\n%d ", 8 - i / 8);
        char piece = board->pieces[i];
        bool sq_diff = (i + i / 8) % 2 == 0;
        char* cb = sq_diff ? B_BLU : B_B_WHT;
        if (is_highlighting(board, i)) {
            cb = sq_diff ? B_RED : B_B_RED;
        }
        char* cf = F_WHT;
        if (is_white(piece)) cf = F_BLK;
        printf("%s%s%s ", cb, cf, piece_char(piece));
    }
    printf(C_RES "\n");
}