#include <eigen3/Eigen/Core>

class Camera
{
    using Vec3 = Eigen::Vector3f;

private:
    // Camera intrinsics
    float _focal = 1.0f;
    float _aspectRatio = 16.0f / 9.0f;
    float _width = 16.0f;
    float _height = 9.0f;

    // Camera Extrinsics
    Vec3 _pos = Vec3{0.0f, 0.0f, 0.0f};
    Vec3 _up = Vec3{0.0f, 1.0f, 0.0f};
    Vec3 _lookAt = Vec3{0.0f, 0.0f, -1.0f};

public:
    Camera();

public:
    //    

    // parameter setters
public:
    inline void setFocal(float focal)
    {
        _focal = focal;
    }
    inline void setAspectRation(float aspectRatio)
    {
        _aspectRatio = aspectRatio;
        _height = _width / aspectRatio;
    }
    inline void setWidth(float width)
    {
        _width = width;
        _aspectRatio = _width / _height;
    }
    inline void setHeight(float height)
    {
        _height = height;
        _aspectRatio = _width / _height;
    }
    inline void setPose(const Vec3 &up, const Vec3 &lookAt)
    {
        _up = up;
        _lookAt = lookAt;
    }
    inline void setPosition(const Vec3 &pos)
    {
        _pos = pos;
    }
};