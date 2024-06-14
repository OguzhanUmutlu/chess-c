#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "chess.h"
#include "legal_moves.h"
#include "utils.h"

static Board* board;
static uint8_t pieceX = ui8_max;
static uint8_t pieceY = ui8_max;

// M1 preset:
// read_fen(board, "6k1/5ppp/8/8/8/8/8/QK6 w QKqk - 0");

// stalemate preset:
// read_fen(board, "k7/P7/1K6/8/8/8/8/8 w QKqk - 0");

// castling preset:
// read_fen(board, "r3k2r/8/8/8/8/8/8/R3K2R w QKqk - 0");

// promotion preset:
// read_fen(board, "k1K5/7P/8/8/8/8/8/8 w QKqk - 0");

// insufficient material preset:
// read_fen(board, "k6K/1P6/8/8/8/8/8/8 b QKqk - 0");

// todo: castling preset does not work for some reason now, munmap_chunk(): invalid pointer

void start_mouse() {
    printf("\e[?25l");         // hide blinking cursor
    disable_canonical_mode();  // hide cursor text
    printf("\033[?1000h");     // enable mouse input
    fflush(stdout);
}

void reset_mouse() {
    printf("\e[?25h");        // show blinking cursor
    enable_canonical_mode();  // resets cursor text
    printf("\033[?1000l");    // disables mouse input
    fflush(stdout);
}

void on_quit(int q) {
    reset_mouse();
    free_board();
    exit(0);
}

void check_end() {
    chess_status status = get_game_status(board);
    if (status != PLAYING) {
        switch (status) {
            case CHECKMATE: {
                printf("\nCheckmate! %s wins!\n", board->turn ? "Black" : "White");
                break;
            }
            case STALEMATE: {
                printf("\nStalemate! It's a draw!\n");
                break;
            }
            case INSUFFICIENT_MATERIAL: {
                printf("\nInsufficient mating material! It's a draw!\n");
                break;
            }
            case FIFTY_MOVE_RULE: {
                printf("\n50 move rule! It's a draw!\n");
                break;
            }
        }
        on_quit(0);
    }
}

void on_board_click(int rx, int ry) {
    if (rx < 0 || ry < 0 || rx > 7 || ry > 7) {
        board_set_highlight(board, NULL);
        pieceX = ui8_max;
        pieceY = ui8_max;
    } else {
        char select = _p(rx, ry);

        if (select != ' ' && is_white(select) == board->turn) {
            pieceX = rx;
            pieceY = ry;
            board_highlight_moves(board, rx, ry);
        } else if (pieceX != ui8_max) {
            char piece = _p(pieceX, pieceY);
            char promotingTo = 'q';
            /*if ((piece == 'p' && ry == 7) || (piece == 'P' && ry == 0)) {
                while (1) {
                    printf(F_GRN "(q, r, b, n)? " C_RES);
                    char* promText = (char*)malloc(8 * sizeof(char));
                    if (fgets(promText, sizeof(promText), stdin) == NULL) {
                        continue;
                    }
                    char prom = promText[0];
                    if (prom < 'a') prom += 'a' - 'A';
                    if (prom != 'q' && prom != 'r' && prom != 'b' && prom != 'n') {
                        continue;
                    }
                    promotingTo = prom;
                    break;
                }
            }*/
            if (board->turn) {
                promotingTo += 'A' - 'a';
            }
            move_piece(board, pieceX, pieceY, rx, ry, promotingTo);

            board_set_highlight(board, NULL);
            pieceX = ui8_max;
            pieceY = ui8_max;
        }
    }

    clear_console();
    print_board(board);
    check_end();
}

int main() {
    init_chess();

    board = make_board();
    if (board == NULL) return 1;

    signal(SIGINT, on_quit);

    start_mouse();

    clear_console();

    print_board(board);

    char input[10];
    int bytes_read;
#ifdef _WIN32
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin == INVALID_HANDLE_VALUE) {
        perror("Error getting standard input handle");
        return 1;
    }
#endif

    while (1) {
#ifdef _WIN32
        if (!ReadFile(hStdin, input, sizeof(input) - 1, &bytes_read, NULL)) continue;
#else
        bytes_read = read(STDIN_FILENO, input, sizeof(input) - 1);
#endif
        if (bytes_read <= 0) continue;
        input[bytes_read] = '\0';

        if (bytes_read == 6 && input[0] == 27 && input[1] == 91 && input[2] == 77 && input[3] == 32) {
            on_board_click((input[4] - 33) / 2, input[5] - 33);
        }
    }

    on_quit(0);
}