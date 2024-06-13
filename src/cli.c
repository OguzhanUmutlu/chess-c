#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chess.h"
#include "legal_moves.h"
#include "utils.h"

void start_cli() {
    init_chess();

    Board* board = make_board();

    clear_console();

    print_board(board);

    uint8_t pieceX = 255;
    uint8_t pieceY = 255;

    while (1) {
        char* line = (char*)malloc(9 * sizeof(char));

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
            pieceX = 255;
            pieceY = 255;
        } else {
            uint8_t x = line[0] - 'a';
            uint8_t y = 8 - (line[1] - '0');

            char select = _p(x, y);

            if (select != ' ' && is_white(select) == board->turn) {
                pieceX = x;
                pieceY = y;
                board_highlight_moves(board, x, y);
            } else if (pieceX != 255) {
                if (move_piece(board, pieceX, pieceY, x, y, 'Q')) {
                    board_set_highlight(board, NULL);
                    pieceX = 255;
                    pieceY = 255;
                }
            }
        }

        clear_console();
        print_board(board);
        free(line);
    }
    free_board();
}