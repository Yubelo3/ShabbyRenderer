#pragma once
#include "ray.hpp"
#include "intersection.hpp"

class SceneBase
{
public:
    virtual Intersection intersect(const Ray &ray) const = 0;
};