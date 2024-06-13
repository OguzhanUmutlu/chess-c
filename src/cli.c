#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "legal_moves.h"
#include "utils.h"

void start_cli() {
    init_chess();

    Board* board = make_board();
    if (board == NULL) return;

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

    clear_console();

    print_board(board);

    uint8_t pieceX = ui8_max;
    uint8_t pieceY = ui8_max;
    char* test = (char*)malloc(9 * sizeof(char));

    while (1) {
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
            break;
        }

        char* line = (char*)malloc(9 * sizeof(char));
        if (line == NULL) {
            malloc_fail();
            break;
        }

        printf(F_GRN "> " C_RES);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            continue;
        }

        line[strlen(line) - 1] = '\0';  // Remove the \n at the end

        if (strlen(line) == 4 && strcmp(line, "exit")) {
            free(line);
            break;
        }

        if (strlen(line) != 2 || line[0] < 'a' || line[0] > 'h' || line[1] < '1' || line[1] > '9') {
            free(line);
            line = NULL;
            board_set_highlight(board, NULL);
            pieceX = ui8_max;
            pieceY = ui8_max;
        } else {
            uint8_t x = line[0] - 'a';
            uint8_t y = 8 - (line[1] - '0');

            char select = _p(x, y);

            if (select != ' ' && is_white(select) == board->turn) {
                pieceX = x;
                pieceY = y;
                board_highlight_moves(board, x, y);
            } else if (pieceX != ui8_max) {
                char piece = _p(pieceX, pieceY);
                char promotingTo = 'q';
                if ((piece == 'p' && y == 7) || (piece == 'P' && y == 0)) {
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
                }
                if (board->turn) {
                    promotingTo += 'A' - 'a';
                }
                if (move_piece(board, pieceX, pieceY, x, y, promotingTo)) {
                    board_set_highlight(board, NULL);
                    pieceX = ui8_max;
                    pieceY = ui8_max;
                }
            }
        }

        clear_console();
        print_board(board);
        free(line);
    }
    free_board();
}