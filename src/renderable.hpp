#pragma once
#include "intersection.hpp"
#include "ray.hpp"
#include <eigen3/Eigen/Core>
#include "material.hpp"
#include <cmath>
#include "utils.hpp"

class Renderable
{
    using MtlPtr = std::shared_ptr<Material>;

protected:
    MtlPtr _material = nullptr;

public:
    Renderable(){};

public:
    void setMaterial(MtlPtr mtl)
    {
        _material = mtl;
    }
    virtual Intersection intersect(Ray) const = 0;
    virtual void transform() = 0;
};

class Shpere : public Renderable
{
    using Vec3 = Eigen::Vector3f;
    using MtlPtr = std::shared_ptr<Material>;

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
        if (tMax < EPS)
            return Intersection();
        if (tMin < EPS)
            t = tMax;
        else
            t = tMin;
        Intersection inter;
        inter.happen = true;
        inter.t = t;
        inter.viewDir = -ray.dir();
        inter.pos = ray.orig() + t * d;
        inter.normal = (inter.pos - _c).normalized();
        if (_material)
            inter.mtl = _material;
        else
            inter.mtl = DEFAULT_MATERIAL;
        return inter;
    }

    void transform()
    {
    }
};

class Triangle
{
};