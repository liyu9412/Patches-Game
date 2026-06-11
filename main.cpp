#include "game.h"
static Game* g_game = nullptr;
static const wchar_t CLASS_NAME[] = L"Patches";
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
    {
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)g_game);
        if (g_game) g_game->hwnd = hwnd;
    }
    return 0;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        if (g_game) g_game->onPaint(hdc);
        EndPaint(hwnd, &ps);
    }
    return 0;
    case WM_LBUTTONDOWN:
    {
        int mx = GET_X_LPARAM(lParam);
        int my = GET_Y_LPARAM(lParam);
        if (g_game) g_game->onLButtonDown(mx, my);
    }
    return 0;
    case WM_MOUSEMOVE:
    {
        int mx = GET_X_LPARAM(lParam);
        int my = GET_Y_LPARAM(lParam);
        if (g_game) g_game->onMouseMove(mx, my);
    }
    return 0;
    case WM_LBUTTONUP:
    {
        int mx = GET_X_LPARAM(lParam);
        int my = GET_Y_LPARAM(lParam);
        if (g_game) g_game->onLButtonUp(mx, my);
    }
    return 0;
    case WM_RBUTTONDOWN:
    {
        int mx = GET_X_LPARAM(lParam);
        int my = GET_Y_LPARAM(lParam);
        if (g_game) g_game->onRButtonDown(mx, my);
    }
    return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}
// 修正 C28251: wWinMain 的 SAL 註釋不一致警告
// 參考 winbase.h 的原型，補齊 _In_/_In_opt_ 標註
int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ PWSTR pCmdLine,
    _In_ int nCmdShow
) {
    // 建立遊戲
	g_game = new Game(7, 7, 100);//將cellSize預設值64改為增加至100
    g_game->init(8);
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClassW(&wc)) {
        delete g_game;
        return 0;
    }
    int winW = g_game->W * g_game->cellSize;
    int winH = g_game->H * g_game->cellSize;
    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Patches",
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT,
        winW + 16, winH + 39,
        NULL,
        NULL,
        hInstance,
        NULL
    );
    if (!hwnd) {
        delete g_game;
        return 0;
    }
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    g_game->hwnd = hwnd;
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    delete g_game;
    return 0;
}
#ifdef _CONSOLE
extern "C" int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ PWSTR pCmdLine,
    _In_ int nCmdShow
);
int main()
{
    return wWinMain(GetModuleHandleW(NULL), NULL, GetCommandLineW(), SW_SHOWDEFAULT);
}
#endif