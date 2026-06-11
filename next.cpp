//下一關
#include "Next.h"
#include "game.h"
void next_level(Game* game) {
    if (!game) return;
    game->init(8);
    // 強制視窗完全重繪以顯示新關卡
    if (game->hwnd) {
        InvalidateRect(game->hwnd, NULL, TRUE);
        UpdateWindow(game->hwnd);
    }
}