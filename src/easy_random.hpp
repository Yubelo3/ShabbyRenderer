#pragma once
#include <random>

static std::random_device _rd;
static std::mt19937_64 _gen(_rd());
static std::uniform_real_distribution<double> _distrib(-1.0, 1.0);

inline double easyUniform()
{
    return _distrib(_gen);
}