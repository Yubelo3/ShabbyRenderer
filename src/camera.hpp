#pragma once
#include <eigen3/Eigen/Core>
#include "ray.hpp"

// Camera **with a film**
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
    Vec3 _rightHand = Vec3{1.0f, 0.0f, 0.0f};

    // Film parameters
    int _nHrozPix = 1024;
    int _nVertPix = 768;
    // Once we modify the camera or the film, the width/height per pixel will change.
    // We set a flag to ensure recompute when use.
    bool _recomputeFilmFlag = true;
    Vec3 _firstPixelCenter{0.0f, 0.0f, 0.0f};
    float _wPix; // width per pixel
    float _hPix; // height per pixel

private:
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
    Camera();

public:
    Ray rayThroughFilm(int row, int col)
    {
        if (_recomputeFilmFlag)
            _recomputeFilm();
        Vec3 pixCenter = _firstPixelCenter + col * _rightHand - row * _up;
        return Ray(_pos, pixCenter - _pos);
    }

    // parameter setters
public:
    inline void setFocal(float focal)
    {
        _focal = focal;
        _recomputeFilmFlag = true;
    }
    inline void setAspectRatio(float aspectRatio)
    {
        _aspectRatio = aspectRatio;
        _height = _width / aspectRatio;
        _recomputeFilmFlag = true;
    }
    inline void setWidth(float width)
    {
        _width = width;
        _aspectRatio = _width / _height;
        _recomputeFilmFlag = true;
    }
    inline void setHeight(float height)
    {
        _height = height;
        _aspectRatio = _width / _height;
        _recomputeFilmFlag = true;
    }
    inline void setPose(const Vec3 &up, const Vec3 &lookAt)
    {
        _up = up.normalized();
        _lookAt = lookAt.normalized();
        _rightHand = _lookAt.cross3(_up);
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
};