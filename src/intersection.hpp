#pragma once
#include <eigen3/Eigen/Core>
#include "utils.hpp"
#include "material.hpp"
#include "texture_payload.hpp"

class Intersection
{
    using Vec3 = Eigen::Vector3f;
    using MtlPtr = std::shared_ptr<Material>;
    using VecPtr = std::shared_ptr<Vec3>;
    using PlPtr = std::shared_ptr<Payload>;

public:
    bool happen = false;
    float t = INF;
    Vec3 viewDir = {0.0f, 0.0f, 1.0f};
    Vec3 pos = {0.0f, 0.0f, 0.0f};
    Vec3 normal = {1.0f, 0.0f, 0.0f};
    MtlPtr mtl = DEFAULT_MATERIAL;
    PlPtr payload = nullptr;  //if exists, contains bcCoord and texture pointer
};