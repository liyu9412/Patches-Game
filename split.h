#pragma once
#include "common.h"
struct Rect {
    int x, y, w, h;
    int id;
    int area() const { return w * h; }
    wchar_t colorChar;
    int numX, numY;
    int hintValue;
};
std::vector<Rect> bsp_split(int gridW, int gridH, int targetCount, std::mt19937& rng);//參考https://www.w3schools.com/cpp/cpp_ref_vector.asp