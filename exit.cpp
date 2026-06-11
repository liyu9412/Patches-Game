//結束遊戲
#include "Exit.h"
void exit_game(HWND hwnd) {
    if (hwnd) {
        //發送 WM_DESTROY
        DestroyWindow(hwnd);
    }
    else {
        PostQuitMessage(0);
    }
}