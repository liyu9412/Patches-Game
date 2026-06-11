//HSV to RGB
//Game建構子
//關卡初始化
//滑鼠拖曳、放置、右鍵清除）
//GDI繪圖
#include "game.h"
#include "Next.h"
#include "Exit.h"
// 將HSV轉換為RGB
static COLORREF hsv2rgb(float h, float s, float v) {
    float c = v * s;
    float x = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = v - c;
    float r = 0, g = 0, b = 0;
    if (h >= 0 && h < 60) { r = c; g = x; b = 0; }
    else if (h >= 60 && h < 120) { r = x; g = c; b = 0; }
    else if (h >= 120 && h < 180) { r = 0; g = c; b = x; }
    else if (h >= 180 && h < 240) { r = 0; g = x; b = c; }
    else if (h >= 240 && h < 300) { r = x; g = 0; b = c; }
    else { r = c; g = 0; b = x; }
    return RGB(
        static_cast<int>((r + m) * 255),
        static_cast<int>((g + m) * 255),
        static_cast<int>((b + m) * 255)
    );
}
//使用當下系統時間為初始化亂數來源
Game::Game(int w, int h, int cell) :
    W(w), H(h), cellSize(cell),
    dragging(false), dragStart{0, 0}, dragEnd{0, 0},
    hwnd(nullptr)
{
    rng.seed((unsigned)std::chrono::system_clock::now().time_since_epoch().count());
}
//關卡初始化
void Game::init(int targetRects) {
    //使用BSP切割
    rects = bsp_split(W, H, targetRects, rng);
    //分別設定提示位置與顏色
    blockColors.assign(rects.size(), RGB(0, 0, 0));
    for (size_t i = 0; i < rects.size(); ++i) {
        Rect& r = rects[i];
        std::uniform_int_distribution<int> rx(0, r.w - 1);
        std::uniform_int_distribution<int> ry(0, r.h - 1);
        r.numX = r.x + rx(rng);
        r.numY = r.y + ry(rng);
        r.hintValue = r.area();
        // 均勻分配色相確保顏色不易碰撞
        float hue = (static_cast<float>(i) / rects.size()) * 360.0f;
        blockColors[i] = hsv2rgb(hue, 0.7f, 0.8f);
    }
    init_grid(grid, W, H, rects);//初始化網格
}
void Game::drawTextCentered(HDC hdc, const std::wstring& s, RECT rc, COLORREF color) {
    SetTextColor(hdc, color);
    SetBkMode(hdc, TRANSPARENT);
    DrawTextW(hdc, s.c_str(), -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}
//滑鼠左鍵拖曳
void Game::onLButtonDown(int mx, int my) {
    dragging = true;
    dragStart = { mx, my };
    dragEnd = dragStart;
    if (hwnd) InvalidateRect(hwnd, NULL, FALSE);
}
//滑鼠移動更新預覽
void Game::onMouseMove(int mx, int my) {
    if (dragging) {
        dragEnd = { mx, my };
        if (hwnd) InvalidateRect(hwnd, NULL, FALSE);
    }
}
//滑鼠左鍵放開
void Game::onLButtonUp(int mx, int my) {
    if (!dragging) return;
    dragging = false;
    dragEnd = { mx, my };
    //像素座標格數座標
    int x1 = dragStart.x / cellSize;
    int y1 = dragStart.y / cellSize;
    int x2 = dragEnd.x / cellSize;
    int y2 = dragEnd.y / cellSize;
    //將座標限制在網格內
    x1 = std::max(0, std::min(W - 1, x1));
    x2 = std::max(0, std::min(W - 1, x2));
    y1 = std::max(0, std::min(H - 1, y1));
    y2 = std::max(0, std::min(H - 1, y2));

    int placedId = -1;
    if (attempt_place(grid, x1, y1, x2, y2, rects, placedId)) {
        if (hwnd) InvalidateRect(hwnd, NULL, FALSE);
        if (check_victory(grid, rects)) {
            //對話框內容
            int response = MessageBoxW(hwnd,
                L"ALL Cleared\n\nY to enter next\n\nN to exit",
                L"Cleared",
                MB_YESNO | MB_ICONINFORMATION);

            if (response == IDYES) {
                next_level(this);
            }
            else {
                exit_game(hwnd);
            }
        }
    }
    // 於背景執行重繪
    if (hwnd) InvalidateRect(hwnd, NULL, FALSE);
}
//滑鼠右鍵按下
void Game::onRButtonDown(int mx, int my) {
    int gx = mx / cellSize;
    int gy = my / cellSize;
    remove_placement(grid, gx, gy);
    if (hwnd) InvalidateRect(hwnd, NULL, FALSE);
}
//來自"windef.h"
void Game::onPaint(HDC hdc) {
    RECT client;
    GetClientRect(hwnd, &client);
    int width = client.right - client.left;
    int height = client.bottom - client.top;
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBmp = CreateCompatibleBitmap(hdc, width, height);
    HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, memBmp);
    // 繪製背景
    HBRUSH bg = CreateSolidBrush(RGB(30, 30, 30));
    FillRect(memDC, &client, bg);
    DeleteObject(bg);
    // 繪製填色網格
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            RECT cellRect = { x * cellSize, y * cellSize, (x + 1) * cellSize, (y + 1) * cellSize };
            int idx = grid.idx(x, y);
            int owner = grid.owner[idx];

            if (owner != -1) {
                //被佔用填入該矩形原色
                HBRUSH br = CreateSolidBrush(blockColors[owner]);
                FillRect(memDC, &cellRect, br);
                DeleteObject(br);
            }
            else {
                //未佔用填入暗灰色
                HBRUSH br = CreateSolidBrush(RGB(50, 50, 50));
                FillRect(memDC, &cellRect, br);
                DeleteObject(br);
            }
        }
    }
    // 繪製即時拖曳預覽
    if (dragging) {
        int x1 = dragStart.x / cellSize;
        int y1 = dragStart.y / cellSize;
        int x2 = dragEnd.x / cellSize;
        int y2 = dragEnd.y / cellSize;
        x1 = std::max(0, std::min(W - 1, x1));
        x2 = std::max(0, std::min(W - 1, x2));
        y1 = std::max(0, std::min(H - 1, y1));
        y2 = std::max(0, std::min(H - 1, y2));
        int lx = std::min(x1, x2), rx = std::max(x1, x2);
        int ty = std::min(y1, y2), by = std::max(y1, y2);
        // 檢查選取範圍內的提示數字
        int hintCount = 0;
        int hintRectId = -1;
        for (int y = ty; y <= by; ++y) {
            for (int x = lx; x <= rx; ++x) {
                int ho = grid.hintOwner[grid.idx(x, y)];
                if (ho != -1) { ++hintCount; hintRectId = ho; }
            }
        }
        // 根據包含提示數字數量決定預覽色彩
        COLORREF dragColor = RGB(120, 120, 120); // 預設灰色
        if (hintCount == 1) {
            //包含1個提示則由該矩形原色增亮
            COLORREF base = blockColors[hintRectId];
            dragColor = RGB(
                std::min(255, GetRValue(base) + 30),
                std::min(255, GetGValue(base) + 30),
                std::min(255, GetBValue(base) + 30)
            );
        }
        else if (hintCount > 1) {
            //覆蓋多個提示時變紅警告
            dragColor = RGB(200, 50, 50);
        }
        RECT sel = { lx * cellSize, ty * cellSize, (rx + 1) * cellSize, (by + 1) * cellSize };
        // 填入預覽色塊
        HBRUSH brDrag = CreateSolidBrush(dragColor);
        FillRect(memDC, &sel, brDrag);
        DeleteObject(brDrag);
        // 繪製白色虛線外框
        HPEN pen = CreatePen(PS_DASH, 2, RGB(255, 255, 255));
        HPEN oldPen = (HPEN)SelectObject(memDC, pen);
        HBRUSH oldBr = (HBRUSH)SelectObject(memDC, GetStockObject(NULL_BRUSH));
        Rectangle(memDC, sel.left, sel.top, sel.right, sel.bottom);
        SelectObject(memDC, oldBr);
        SelectObject(memDC, oldPen);
        DeleteObject(pen);
        // 繪製目前拖曳面積
        int currentArea = (rx - lx + 1) * (by - ty + 1);
        std::wstringstream dragText;
        dragText << currentArea;
        if (hintCount == 1) {
            dragText << L" / " << grid.hintValue[hintRectId];
        }
        HFONT hFontDrag = CreateFontW(cellSize / 2, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");
        HFONT oldFontD = (HFONT)SelectObject(memDC, hFontDrag);
        // 繪製黑底
        RECT shadowSel = sel;
        shadowSel.top += 2; shadowSel.left += 2; shadowSel.bottom += 2; shadowSel.right += 2;
        drawTextCentered(memDC, dragText.str(), shadowSel, RGB(0, 0, 0));
        // 繪製白字
        drawTextCentered(memDC, dragText.str(), sel, RGB(255, 255, 255));
        SelectObject(memDC, oldFontD);
        DeleteObject(hFontDrag);
    }
    //繪製格線 (疊加在區塊上)
    HPEN penLine = CreatePen(PS_SOLID, 1, RGB(80, 80, 80));
    HPEN oldPenLine = (HPEN)SelectObject(memDC, penLine);
    for (int y = 0; y <= H; ++y) {
        MoveToEx(memDC, 0, y * cellSize, NULL);
        LineTo(memDC, W * cellSize, y * cellSize);
    }
    for (int x = 0; x <= W; ++x) {
        MoveToEx(memDC, x * cellSize, 0, NULL);
        LineTo(memDC, x * cellSize, H * cellSize);
    }
    SelectObject(memDC, oldPenLine);
    DeleteObject(penLine);
    //  於最上層繪製提示數字
    HFONT hFont = CreateFontW(cellSize / 2, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");
    HFONT oldFont = (HFONT)SelectObject(memDC, hFont);
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            int hint = grid.hintOwner[grid.idx(x, y)];
            if (hint != -1) {
                RECT cellRect = { x * cellSize, y * cellSize, (x + 1) * cellSize, (y + 1) * cellSize };
                std::wstringstream ss;
                ss << grid.hintValue[hint];
                // 深色圓底
                int cx = (cellRect.left + cellRect.right) / 2;
                int cy = (cellRect.top + cellRect.bottom) / 2;
                int r = cellSize / 3;
                HBRUSH textBgBr = CreateSolidBrush(RGB(20, 20, 20));
                HPEN textBgPen = CreatePen(PS_SOLID, 1, RGB(20, 20, 20));
                HGDIOBJ oldBr = SelectObject(memDC, textBgBr);
                HGDIOBJ oldPen = SelectObject(memDC, textBgPen);
                Ellipse(memDC, cx - r, cy - r, cx + r, cy + r);
                SelectObject(memDC, oldBr);
                SelectObject(memDC, oldPen);
                DeleteObject(textBgBr);
                DeleteObject(textBgPen);
                drawTextCentered(memDC, ss.str(), cellRect, RGB(255, 255, 255));//圓內白色數字
            }
        }
    }
    SelectObject(memDC, oldFont);
    DeleteObject(hFont);
    // 輸出至視窗
    BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);
    // 清理GDI資源
    SelectObject(memDC, oldBmp);
    DeleteObject(memBmp);
    DeleteDC(memDC);
}