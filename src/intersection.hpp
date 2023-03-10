#pragma once
#include <eigen3/Eigen/Core>
#include "utils.hpp"
#include "material.hpp"

class Intersection
{
    using Vec3 = Eigen::Vector3d;
    using Vec3Ptr = std::shared_ptr<Vec3>;
    using MtlPtr = std::shared_ptr<Material>;
    using VecPtr = std::shared_ptr<Vec3>;

public:
    bool happen = false;
    double t = INF;
    Vec3 viewDir = {0.0, 0.0, 1.0};
    Vec3 pos = {0.0, 0.0, 0.0};
    Vec3 normal = {1.0, 0.0, 0.0};
    MtlPtr mtl = DEFAULT_MATERIAL;
    Vec3Ptr texColor = nullptr;
};