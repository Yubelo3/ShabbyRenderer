#pragma once
#include <eigen3/Eigen/Core>

class Ray
{
    using Vec3 = Eigen::Vector3d;

private:
    Vec3 _orig;
    Vec3 _dir;

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    Ray();
    Ray(const Vec3 &orig, const Vec3 &direction) : _orig(orig)
    {
        _dir = direction.normalized();
    }

public:
    inline Vec3 operator()(double t) const
    {
        return _orig + t * _dir;
    }

public:
    inline const Vec3 &orig() const
    {
        return _orig;
    }
    inline const Vec3 &dir() const
    {
        return _dir;
    }
};