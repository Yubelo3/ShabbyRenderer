#pragma once
#include "ray.hpp"
#include "intersection.hpp"
#include "light.hpp"

class SceneBase
{
    using LightPtr = std::shared_ptr<Light>;

public:
    virtual Intersection intersect(const Ray &ray) const = 0;
    virtual const std::vector<LightPtr> &lights() const = 0;
};