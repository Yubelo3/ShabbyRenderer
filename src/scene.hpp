#pragma once
#include "camera.hpp"
#include "renderable.hpp"
#include <vector>
#include <eigen3/Eigen/Core>
#include "config.h"

class Scene
{
    using Vec3 = Eigen::Vector3f;

private:
    Camera *_camera;
    std::vector<Renderable *> _objs;
    Vec3 *_frameBuffer = nullptr;

public:
    Scene()
    {
        _camera = new Camera();
    }
    ~Scene()
    {
        delete[] _frameBuffer;
    }

public:
    // TODO: 引用修正
    void setCamera(Camera *camera)
    {
        _camera = camera;
        delete[] _frameBuffer;
        int bufferSize = (camera->nHorzPix()) * (camera->nVertPix());
        _frameBuffer = new Vec3[bufferSize];
    }
    void addObject(Renderable *renderable)
    {
        _objs.push_back(renderable);
    }

    void render() const
    {
        int w = _camera->nHorzPix(), h = _camera->nVertPix();
        int bufferOffset = 0;
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++)
            {
                Ray ray = _camera->rayThroughFilm(i, j);
                for (const Renderable *obj : _objs)
                {
                    Intersection itsct = obj->intersect(ray);
                    if (itsct.happen)
                        _frameBuffer[bufferOffset] = itsct.normal.cwiseAbs(); // TODO: 颜色计算
                    else
                        _frameBuffer[bufferOffset] = BG_COLOR;
                }
                bufferOffset++;
            }
    }
    Vec3 *frameBuffer() const
    {
        return _frameBuffer;
    }
};