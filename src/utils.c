#include "utils.h"

#ifdef _WIN32

#include <windows.h>

void disable_canonical_mode() {
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);
    mode &= ~ENABLE_LINE_INPUT;
    mode &= ~ENABLE_ECHO_INPUT;
    SetConsoleMode(hStdin, mode);
}

void enable_canonical_mode() {
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);
    mode |= ENABLE_LINE_INPUT;
    mode |= ENABLE_ECHO_INPUT;
    SetConsoleMode(hStdin, mode);
}
#else

void disable_canonical_mode() {
    system("stty -icanon -echo");
}

void enable_canonical_mode() {
    system("stty icanon echo");
}
#endif