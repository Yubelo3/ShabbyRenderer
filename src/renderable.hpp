#pragma once
#include "intersection.hpp"
#include "ray.hpp"
#include <eigen3/Eigen/Core>
#include "material.hpp"
#include <cmath>

class Renderable
{
private:
    Material *_material = nullptr;

public:
    Renderable(){};

public:
    virtual Intersection intersect(Ray) const = 0;
    virtual void transform() = 0;
};

class Shpere : public Renderable
{
    using Vec3 = Eigen::Vector3f;

private:
    Vec3 _c = {0.0f, 0.0f, -5.0f};
    float _r = 2.0f;

public:
    Shpere(){};
    Shpere(Vec3 c, float r) : _c(c), _r(r){};

public:
    Intersection intersect(Ray ray) const override
    {
        Vec3 o = ray.orig() - _c, d = ray.dir();
        float A = d.dot(d), B = 2 * o.dot(d), C = o.dot(o) - _r * _r;
        float delta = B * B - 4 * A * C;
        if (delta < 0.0f)
            return Intersection();
        float tMin = (-B - sqrt(delta)) / 2.0f / A;
        float tMax = (-B + sqrt(delta)) / 2.0f / A;
        float t;
        if (tMin > tMax)
            std::swap(tMin, tMax);
        if (tMax < 0.0f)
            return Intersection();
        if (tMin < 0.0f)
            t = tMax;
        else
            t = tMin;
        Intersection inter;
        inter.happen = true;
        inter.t = t;
        inter.color = {0.5f, 0.6f, 0.7f};
        return inter;
    }

    void transform()
    {
    }
};

class Triangle
{
};