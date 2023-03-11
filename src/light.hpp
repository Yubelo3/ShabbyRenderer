#pragma once
#include <eigen3/Eigen/Core>
#include "easy_random.hpp"

class Light
{
    using Vec3 = Eigen::Vector3d;

public:
    // Compute intensity and direction at a specific point
    virtual void idAt(const Vec3 &, Vec3 &, Vec3 &) const = 0;
};

class PointLight : public Light
{
    using Vec3 = Eigen::Vector3d;

private:
    Vec3 _intensity = {2.0, 2.0, 2.0};
    Vec3 _pos = {0.0, 0.0, 0.0};

public:
    PointLight(const Vec3 &intensity, const Vec3 &pos) : _intensity(intensity), _pos(pos){};

public:
    void idAt(const Vec3 &pos, Vec3 &intensity, Vec3 &dir) const override
    {
        dir = _pos - pos;
        double r = dir.norm();
        dir /= r;
        intensity = _intensity / r / r;
    }
};

class AmbientLight : public Light
{
    using Vec3 = Eigen::Vector3d;

private:
    Vec3 _intensity = {2.0, 2.0, 2.0};

public:
    AmbientLight(const Vec3 &intensity) : _intensity(intensity){};

public:
    void idAt(const Vec3 &pos, Vec3 &intensity, Vec3 &dir) const override
    {
        intensity = _intensity;
        dir = {0.0, 0.0, 0.0};
    }
};

class ParallelLight : public Light
{
    using Vec3 = Eigen::Vector3d;

private:
    Vec3 _intensity = {2.0, 2.0, 2.0};
    Vec3 _dir = {0.0, 0.0, -1.0};

public:
    ParallelLight(const Vec3 &intensity, const Vec3 &dir) : _intensity(intensity), _dir(dir.normalized()){};

public:
    void idAt(const Vec3 &pos, Vec3 &intensity, Vec3 &dir) const override
    {
        intensity = _intensity;
        dir = -_dir;
    }
};

class AreaLight : public Light
{
    using Vec3 = Eigen::Vector3d;

private:
    Vec3 _intensity = {2.0, 2.0, 2.0};
    Vec3 _center = Vec3::Zero();
    Vec3 _a;
    Vec3 _b;

public:
    AreaLight(const Vec3 &intensity, const Vec3 &center, const Vec3 &a, const Vec3 &b) : _intensity(intensity), _center(center), _a(a), _b(b){};

public:
    void idAt(const Vec3 &pos, Vec3 &intensity, Vec3 &dir) const override
    {
        Vec3 samplePos = _center + easyUniform() * _a + easyUniform() * _b;
        dir = samplePos - pos;
        double r = dir.norm();
        dir /= r;
        intensity = _intensity / r / r;
    }
};