//遊戲規則
#include "move.h"
Grid::Grid(int w, int h) : W(w), H(h) {
    owner.assign(W * H, -1);
    hintOwner.assign(W * H, -1);
}
void init_grid(Grid& g, int W, int H, const std::vector<Rect>& rects) {
    g = Grid(W, H);
    g.hintValue.assign(rects.size(), 0);
    for (const Rect& r : rects) {
        if (r.numX >= 0 && r.numY >= 0 && g.inBounds(r.numX, r.numY)) {
            g.hintOwner[g.idx(r.numX, r.numY)] = r.id;
            g.hintValue[r.id] = r.hintValue;
        }
    }
}
bool attempt_place(Grid& g, int x1, int y1, int x2, int y2, const std::vector<Rect>& rects, int& placedRectId) {
    if (!g.inBounds(x1, y1) || !g.inBounds(x2, y2)) return false;
    int lx = std::min(x1, x2), rx = std::max(x1, x2);
    int ty = std::min(y1, y2), by = std::max(y1, y2);
    int w = rx - lx + 1;
    int h = by - ty + 1;
    int area = w * h;
    int hintCount = 0;
    int hintRectId = -1;
    for (int y = ty; y <= by; ++y) for (int x = lx; x <= rx; ++x) {
        int ho = g.hintOwner[g.idx(x, y)];
        if (ho != -1) { ++hintCount; hintRectId = ho; }
    }
    if (hintCount != 1) return false;
    int expected = g.hintValue[hintRectId];
    if (expected != area) return false;
    for (int y = ty; y <= by; ++y) for (int x = lx; x <= rx; ++x) {
        if (g.owner[g.idx(x, y)] != -1) return false;
    }
    for (int y = ty; y <= by; ++y) for (int x = lx; x <= rx; ++x) {
        g.owner[g.idx(x, y)] = hintRectId;
    }
    placedRectId = hintRectId;
    return true;
}
void remove_placement(Grid& g, int gx, int gy) {
    if (!g.inBounds(gx, gy)) return;
    int ownerId = g.owner[g.idx(gx, gy)];
    if (ownerId != -1) {
        for (int i = 0; i < g.W * g.H; ++i) {
            if (g.owner[i] == ownerId) {
                g.owner[i] = -1;
            }
        }
    }
}
bool check_victory(const Grid& g, const std::vector<Rect>& rects) {
    for (int i = 0; i < g.W * g.H; ++i) if (g.owner[i] == -1) return false;
    std::vector<int> counts(rects.size(), 0);
    for (int i = 0; i < g.W * g.H; ++i) {
        int id = g.owner[i];
        if (id >= 0 && id < (int)rects.size()) counts[id]++;
    }
    for (size_t i = 0; i < rects.size(); ++i) {
        if (counts[i] != rects[i].area()) return false;
    }
    return true;
}