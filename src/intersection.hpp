#pragma once
#include <eigen3/Eigen/Core>
#include "constant.h"

class Intersection
{
    using Vec3 = Eigen::Vector3f;

public:
    bool happen = false;
    float t = INF;
    Vec3 normal = {1.0f, 0.0f, 0.0f};
    Vec3 color = {0.0f, 0.0f, 0.0f};

};