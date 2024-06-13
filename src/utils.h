#ifndef CHESS_UTILS_H
#define CHESS_UTILS_H

#include <stdlib.h>

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

#ifdef _WIN32
#define clear_console() system("cls")
#else
#define clear_console() system("clear")
#endif

#endif