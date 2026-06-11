#pragma once
//Windows API 精簡設定
//防止windows.h定義 min/max 巨集，避免與std::min/std::max產生衝突
#ifndef NOMINMAX
#define NOMINMAX
#endif
//排除Windows API中不常用子系統
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// Windows API
#include <windows.h>
//GET_X_LPARAM / GET_Y_LPARAM
#include <windowsx.h>
//C++ 標準函式庫

#include <vector>//std::vector
#include <string>//std::wstring
#include <sstream>//std::wstringstream
#include <algorithm>//std::min/std::max/std::clamp
#include <chrono>//std::chrono::system_clock(系統時間)
#include <random>//std::mt19937
#include <cmath>//std::fmod/std::abs
#include <queue>//std::queue
//針對間接引入補強
#include <new>//operator new/operator delete（防止main.cpp的new Game和delete g_game失效）
#include <cstddef>//size_t