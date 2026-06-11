#include "split.h"
static bool try_split_rect(const Rect& r, Rect& a, Rect& b, std::mt19937& rng) {
    std::uniform_int_distribution<int> coin(0, 1);
    bool vertical = coin(rng) == 0;
    if (vertical) {
        if (r.w <= 1) return false;
        std::uniform_int_distribution<int> cut(1, r.w - 1);
        int cw = cut(rng);
        a = { r.x, r.y, cw, r.h, -1, L'A', -1, -1, 0 };
        b = { r.x + cw, r.y, r.w - cw, r.h, -1, L'A', -1, -1, 0 };
        return true;
    }
    else {
        if (r.h <= 1) return false;
        std::uniform_int_distribution<int> cut(1, r.h - 1);
        int ch = cut(rng);
        a = { r.x, r.y, r.w, ch, -1, L'A', -1, -1, 0 };
        b = { r.x, r.y + ch, r.w, r.h - ch, -1, L'A', -1, -1, 0 };
        return true;
    }
}
std::vector<Rect> bsp_split(int gridW, int gridH, int targetCount, std::mt19937& rng) {
    std::vector<Rect> result;
    std::queue<Rect> q;
    q.push({ 0,0,gridW,gridH,-1,L'A',-1,-1,0 });
    while (!q.empty() && (int)q.size() < targetCount) {
        Rect cur = q.front(); q.pop();
        Rect a, b;
        bool splitted = false;
        for (int tries = 0; tries < 20; ++tries) {
            if (try_split_rect(cur, a, b, rng)) {
                if (a.area() >= 1 && b.area() >= 1) {
                    q.push(a); q.push(b);
                    splitted = true;
                    break;
                }
            }
        }
        if (!splitted) {
            q.push(cur);
            break;
        }
    }
    while (!q.empty()) { result.push_back(q.front()); q.pop(); }
    wchar_t base = L'A';
    for (size_t i = 0; i < result.size(); ++i) {
        result[i].id = (int)i;
        result[i].colorChar = base + (wchar_t)(i % 26);
    }
    return result;
}