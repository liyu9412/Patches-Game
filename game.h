#pragma once
#include "move.h"
class Game {
public:
    int W, H;
    int cellSize;
    std::vector<Rect> rects;
    Grid grid;
    std::mt19937 rng;
    // drag state
    bool dragging;
    POINT dragStart;
    POINT dragEnd;
    // colors
    std::vector<COLORREF> blockColors;
    HWND hwnd;
    Game(int w = 7, int h = 7, int cell = 64);
    void init(int targetRects = 8);
    void onPaint(HDC hdc);
    void onLButtonDown(int mx, int my);
    void onMouseMove(int mx, int my);
    void onLButtonUp(int mx, int my);
    void onRButtonDown(int mx, int my); // 右鍵清除
    void drawTextCentered(HDC hdc, const std::wstring& s, RECT rc, COLORREF color);
};