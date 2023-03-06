#pragma once
#include "intersection.hpp"
#include "ray.hpp"
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/LU>
#include "material.hpp"
#include <cmath>
#include "utils.hpp"

class AABB
{
    using Vec3 = Eigen::Vector3f;

private:
    Vec3 _min;
    Vec3 _max;

public:
    AABB(){};
    AABB(const Vec3 &min, const Vec3 &max) : _min(min), _max(max){};

public:
    bool intersect(const Ray &ray) const
    {
        float tMin = 0.0f, tMax = INF;
        Vec3 O = ray.orig(), D = ray.dir();
        for (int i : {0, 1, 2})
        {
            float minBound = _min[i], maxBound = _max[i];
            float tMinBound = (minBound - O[i]) / D[i], tMaxBound = (maxBound - O[i]) / D[i];
            if (tMinBound > tMaxBound)
                std::swap(tMinBound, tMaxBound);
            tMin = std::max(tMin, tMinBound);
            tMax = std::min(tMax, tMaxBound);
            if (tMin > tMax)
                return false;
        }
        return true;
    }

public:
    void set(const Vec3 &min, const Vec3 &max)
    {
        _min = min;
        _max = max;
    }
};

class Renderable
{
    using MtlPtr = std::shared_ptr<Material>;

protected:
    MtlPtr _material = nullptr;
    AABB _aabb;

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
    Shpere(Vec3 c, float r) : _c(c), _r(r)
    {
        _aabb.set(c - Vec3{r, r, r}, c + Vec3{r, r, r});
    }

public:
    // override functions
    Intersection intersect(Ray ray) const override
    {
        if (!_aabb.intersect(ray))
            return Intersection();
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

    void transform() override
    {
    }
};

class Triangle : public Renderable
{
    // By default, triangle use (v1-v0).cross(v2-v0) as face normal
    // This can be changed via setFaceNormal()
    using Vec3 = Eigen::Vector3f;
    using Mat3 = Eigen::Matrix3f;
    using MtlPtr = std::shared_ptr<Material>;

private:
    Vec3 _v[3];   // counter-clockwise vertices
    Vec3 _n[3];   // vertex normal
    Vec3 _normal; // face normal

public:
    Triangle(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2)
    {
        _v[0] = v0, _v[1] = v1, _v[2] = v2;
        _normal = (v1 - v0).cross(v2 - v0).normalized();
        Mat3 V;
        Vec3 min = _v[0].cwiseMin(_v[1].cwiseMin(_v[2]));
        Vec3 max = _v[0].cwiseMax(_v[1].cwiseMax(_v[2]));
        _aabb.set(min, max);
    };

public:
    // override functions
    Intersection intersect(Ray ray) const override
    {
        Intersection inter;
        if (!_aabb.intersect(ray))
            return inter;
        // SOLVE: o+td=a+beta*e1+gamma*e2
        // That is, [e1,e2,-d][beta,gamma,t]^T=o-a
        Mat3 A;
        A << _v[1] - _v[0], _v[2] - _v[0], -ray.dir();
        Vec3 b = ray.orig() - _v[0];
        Vec3 solution = A.lu().solve(b);
        auto [beta, gamma, t] = std::array{solution[0], solution[1], solution[2]};
        float alpha = 1.0f - beta - gamma;

        if (t > EPS && alpha >= 0.0f && beta >= 0.0f && gamma >= 0.0f)
        {
            float hitDir = (ray.dir().dot(_normal) > 0.0f ? -1.0f : 1.0f);
            inter.happen = true;
            inter.pos = ray(t);
            if (_material)
                inter.mtl = _material;
            else
                inter.mtl = DEFAULT_MATERIAL;
            inter.normal = _normal * hitDir;
            inter.t = t;
            inter.viewDir = ray.dir();
            inter.bcCoord = std::make_shared<Vec3>(alpha, beta, gamma);
        }
        return inter;
    }

    void transform() override
    {
    }

public:
    // parameter setters
    void setVertexNormal(const Vec3 &n0, const Vec3 &n1, const Vec3 &n2)
    {
        _n[0] = n0.normalized();
        _n[1] = n1.normalized();
        _n[2] = n2.normalized();
    }
    void setFaceNormal(const Vec3 &n)
    {
        _normal = n.normalized();
    }
};