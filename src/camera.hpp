#pragma once
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include "ray.hpp"
#include "utils.hpp"

// Camera **with a film**
// TODO: 用fov形式表示film
class Camera
{
    using Vec3 = Eigen::Vector3f;

protected:
    // Camera intrinsics
    float _focal = 1.0f;
    float _aspectRatio = 1024.f / 768.f;
    float _hfov = degree2radian(100.0f);

    // Camera Extrinsics
    Vec3 _pos = Vec3{0.0f, 0.0f, 0.0f};
    Vec3 _up = Vec3{0.0f, 1.0f, 0.0f};
    Vec3 _lookAt = Vec3{0.0f, 0.0f, -1.0f};
    Vec3 _rightHand = Vec3{1.0f, 0.0f, 0.0f};

    // Viewport parameters
    // Should not be changed directly
    float _width = 0.0f;
    float _height = 0.0f;

    // Film parameters
    int _nHrozPix = 1024;
    int _nVertPix = 768;
    // Once we modify the camera or the film, the width/height per pixel will change.
    // We set a flag to ensure recompute when use.
    bool _recomputeFilmFlag = true;
    Vec3 _firstPixelCenter{0.0f, 0.0f, 0.0f};
    float _wPix; // width per pixel
    float _hPix; // height per pixel

protected:
    void _recomputeFilm()
    {
        _wPix = _width / _nHrozPix;
        _hPix = _height / _nVertPix;
        Vec3 filmCenter = _pos + _focal * _lookAt;
        float offLeft = _width / 2 - 0.5f * _wPix;
        float offUp = _height / 2 - 0.5f * _hPix;
        _firstPixelCenter = filmCenter - _rightHand * offLeft + _up * offUp;
    }

public:
    Camera(){};

public:
    virtual Ray rayThroughFilm(float row, float col) = 0;

    // parameter setters
public:
    inline void setFocal(float focal)
    {
        _focal = focal;
        _width = 2.0f * _focal * tan(_hfov / 2.0f);
        _height = _width / _aspectRatio;
        _recomputeFilmFlag = true;
    }
    inline void setAspectRatio(float aspectRatio)
    {
        _aspectRatio = aspectRatio;
        _height = _width / aspectRatio;
        _recomputeFilmFlag = true;
    }
    inline void setHFOV(float hfov)
    {
        _hfov = degree2radian(hfov);
        _width = 2.0f * _focal * tan(_hfov / 2.0f);
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
    using Vec3 = Eigen::Vector3f;

public:
    Ray rayThroughFilm(float row, float col)
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
    using Vec3 = Eigen::Vector3f;

public:
    Ray rayThroughFilm(float row, float col)
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