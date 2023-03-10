#pragma once
#include <iostream>

#define RAISE_ERROR(x)                                 \
    {                                                  \
        printf("%s\n", x);                             \
        printf("%s:line %d: x\n", __FILE__, __LINE__); \
        exit(-1);                                      \
    }

const double INF = std::numeric_limits<double>::infinity();

const double PI = 3.1415926535897;

const double EPS = 1e-5;

inline double degree2radian(double d)
{
    return d * PI / 180.0;
}