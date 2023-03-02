#pragma once
#include <eigen3/Eigen/Core>

class Light
{
    using Vec3 = Eigen::Vector3f;

public:
    // Compute intensity and direction at a specific point
    virtual void idAt(const Vec3 &, Vec3 &, Vec3 &) const = 0;
};

class PointLight : public Light
{
    using Vec3 = Eigen::Vector3f;

private:
    Vec3 _intensity = {2.0f, 2.0f, 2.0f};
    Vec3 _pos = {0.0f, 0.0f, 0.0f};

public:
    PointLight(const Vec3 &intensity, const Vec3 &pos) : _intensity(intensity), _pos(pos){};

public:
    void idAt(const Vec3 &pos, Vec3 &intensity, Vec3 &dir) const override
    {
        dir = _pos - pos;
        float r = dir.norm();
        dir /= r;
        intensity = _intensity / r / r;
    }
};

class AmbientLight : public Light
{
    using Vec3 = Eigen::Vector3f;

private:
    Vec3 _intensity = {2.0f, 2.0f, 2.0f};

public:
    AmbientLight(const Vec3& intensity):_intensity(intensity){};

public:
    void idAt(const Vec3 &pos, Vec3 &intensity, Vec3 &dir) const override
    {
        intensity = _intensity;
        dir = {0.0f, 0.0f, 0.0f};
    }
};

class ParallelLight : public Light
{
    using Vec3 = Eigen::Vector3f;

private:
    Vec3 _intensity = {2.0f, 2.0f, 2.0f};
    Vec3 _dir = {0.0f, 0.0f, -1.0f};

public:
    void idAt(const Vec3 &pos, Vec3 &intensity, Vec3 &dir) const override
    {
        intensity = _intensity;
        dir = _dir;
    }
};