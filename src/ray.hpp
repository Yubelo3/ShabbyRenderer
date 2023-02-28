#include <eigen3/Eigen/Core>


class Ray
{
    using Vec3=Eigen::Vector3f;
private:
    Vec3 _orig;
    Vec3 _dir;

public:
    Ray();
    Ray(const Vec3& orig, const Vec3& direction):_orig(orig)
    {
        _dir=direction.normalized();
    }

public:
    Vec3 operator()(float t) const
    {
        return _orig+t*_dir;
    }
};