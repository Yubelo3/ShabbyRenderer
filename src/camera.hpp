#pragma once
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include "ray.hpp"
#include "utils.hpp"

// Camera **with a film**lele
// TODO: 用fov形式表示filmle
class Camera
{
    using Vec3 = Eigen::Vector3d;

protected:
    // Camera intrinsics
    double _focal = 1.0;
    double _aspectRatio = 1024.0 / 768.0;
    double _hfov = degree2radian(100.0);

    // Camera Extrinsics
    Vec3 _pos = Vec3{0.0, 0.0, 0.0};
    Vec3 _up = Vec3{0.0, 1.0, 0.0};
    Vec3 _lookAt = Vec3{0.0, 0.0, -1.0};
    Vec3 _rightHand = Vec3{1.0, 0.0, 0.0};

    // Viewport parameters
    // Should not be changed directly
    double _width = 0.0;
    double _height = 0.0;

    // Film parameters
    int _nHrozPix = 1024;
    int _nVertPix = 768;
    // Once we modify the camera or the film, the width/height per pixel will change.
    // We set a flag to ensure recompute when use.
    bool _recomputeFilmFlag = true;
    Vec3 _firstPixelCenter{0.0, 0.0, 0.0};
    double _wPix; // width per pixel
    double _hPix; // height per pixel

protected:
    void _recomputeFilm()
    {
        _wPix = _width / _nHrozPix;
        _hPix = _height / _nVertPix;
        Vec3 filmCenter = _pos + _focal * _lookAt;
        double offLeft = _width / 2 - 0.5 * _wPix;
        double offUp = _height / 2 - 0.5 * _hPix;
        _firstPixelCenter = filmCenter - _rightHand * offLeft + _up * offUp;
    }

public:
    Camera(){};

public:
    virtual Ray rayThroughFilm(double row, double col) = 0;

    // parameter setters
public:
    inline void setFocal(double focal)
    {
        _focal = focal;
        _width = 2.0 * _focal * tan(_hfov / 2.0);
        _height = _width / _aspectRatio;
        _recomputeFilmFlag = true;
    }
    inline void setAspectRatio(double aspectRatio)
    {
        _aspectRatio = aspectRatio;
        _height = _width / aspectRatio;
        _recomputeFilmFlag = true;
    }
    inline void setHFOV(double hfov)
    {
        _hfov = degree2radian(hfov);
        _width = 2.0 * _focal * tan(_hfov / 2.0);
        _height = _width / _aspectRatio;
        _recomputeFilmFlag = true;
    }
    inline void setPose(const Vec3 &up, const Vec3 &lookAt)
    {
        if(std::abs(up.dot(lookAt))>EPS)
            RAISE_ERROR("Illegal pose");
        _up = up.normalized();
        _lookAt = lookAt.normalized();
        _rightHand = _lookAt.cross(_up);
        _recomputeFilmFlag = true;
    }
    inline void setPosition(const Vec3 &pos)
    {
        _pos = pos;
        _recomputeFilmFlag = true;
    }
    inline void setFilm(int nHorz, int nVert)
    {
        _nHrozPix = nHorz;
        _nVertPix = nVert;
        _recomputeFilmFlag = true;
    }

    // parameter getters
    inline int nHorzPix() const
    {
        return _nHrozPix;
    }
    inline int nVertPix() const
    {
        return _nVertPix;
    }
};

class PerspectiveCamera : public Camera
{
    using Vec3 = Eigen::Vector3d;

public:
    Ray rayThroughFilm(double row, double col)
    {
        if (_recomputeFilmFlag)
        {
            _recomputeFilm();
            _recomputeFilmFlag = false;
        }
        Vec3 pixCenter = _firstPixelCenter + col * _wPix * _rightHand - row * _hPix * _up;
        return Ray(_pos, pixCenter - _pos);
    }
};

class OrthogonalCamera : public Camera
{
    using Vec3 = Eigen::Vector3d;

public:
    Ray rayThroughFilm(double row, double col)
    {
        if (_recomputeFilmFlag)
        {
            _recomputeFilm();
            _recomputeFilmFlag = false;
        }
        Vec3 pixCenter = _firstPixelCenter + col * _wPix * _rightHand - row * _hPix * _up;
        return Ray(pixCenter - _focal * _lookAt, _lookAt);
    }
};