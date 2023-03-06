#pragma once
#include <iostream>

#define RAISE_ERROR(x)                                 \
    {                                                  \
        printf("%s\n", x);                             \
        printf("%s:line %d: x\n", __FILE__, __LINE__); \
        exit(-1);                                      \
    }

const float INF = std::numeric_limits<float>::infinity();

const float PI = 3.1415926;

const float EPS = 1e-2f;

inline float degree2radian(float d)
{
    return d * PI / 180.0f;
}