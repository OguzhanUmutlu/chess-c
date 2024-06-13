#ifndef CHESS_UTILS_H
#define CHESS_UTILS_H

#include <stdlib.h>

#define malloc_fail() perror("Memory allocation failed")

#define ui8_max 255
#define ui16_max 65535

#define F_BLK "\x1b[99;30m"
#define F_RED "\x1b[99;31m"
#define F_GRN "\x1b[99;32m"
#define F_YLW "\x1b[99;33m"
#define F_BLU "\x1b[99;34m"
#define F_MGN "\x1b[99;35m"
#define F_CYN "\x1b[99;36m"
#define F_WHT "\x1b[99;37m"

#define B_BLK "\x1b[99;40m"
#define B_RED "\x1b[99;41m"
#define B_GRN "\x1b[99;42m"
#define B_YLW "\x1b[99;43m"
#define B_BLU "\x1b[99;44m"
#define B_MGN "\x1b[99;45m"
#define B_CYN "\x1b[99;46m"
#define B_WHT "\x1b[99;47m"

#define B_B_BLK "\x1b[99;100m"
#define B_B_RED "\x1b[99;101m"
#define B_B_GRN "\x1b[99;102m"
#define B_B_YLW "\x1b[99;103m"
#define B_B_BLU "\x1b[99;104m"
#define B_B_MGN "\x1b[99;105m"
#define B_B_CYN "\x1b[99;106m"
#define B_B_WHT "\x1b[99;107m"

#define C_RES "\x1b[99;0m"

#ifdef _WIN32
#define clear_console() system("cls")
#define USE_LETTERS_FOR_PIECES
#else
#define clear_console() system("clear")
#endif

#ifdef USE_LETTERS_FOR_PIECES
#define _WK "K"
#define _WQ "Q"
#define _WR "R"
#define _WB "B"
#define _WN "N"
#define _WP "P"
#define _BK "k"
#define _BQ "q"
#define _BR "r"
#define _BB "b"
#define _BN "n"
#define _BP "p"

#else

#define _WK "♔"
#define _WQ "♕"
#define _WR "♖"
#define _WB "♗"
#define _WN "♘"
#define _WP "♙"
#define _BK "♚"
#define _BQ "♛"
#define _BR "♜"
#define _BB "♝"
#define _BN "♞"
#define _BP "♟"

#endif

#endif