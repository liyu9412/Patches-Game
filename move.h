#pragma once
#include "split.h"
struct Grid {
    int W, H;
    std::vector<int> owner;
    std::vector<int> hintOwner;
    std::vector<int> hintValue;
    Grid(int w = 0, int h = 0);
    int idx(int x, int y) const { return y * W + x; }
    bool inBounds(int x, int y) const { return x >= 0 && y >= 0 && x < W && y < H; }
};
void init_grid(Grid& g, int W, int H, const std::vector<Rect>& rects);
bool attempt_place(Grid& g, int x1, int y1, int x2, int y2, const std::vector<Rect>& rects, int& placedRectId);
void remove_placement(Grid& g, int gx, int gy); // 清除無效佔用區塊
bool check_victory(const Grid& g, const std::vector<Rect>& rects);