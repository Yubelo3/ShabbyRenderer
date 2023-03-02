#pragma once
#include "camera.hpp"
#include "renderable.hpp"
#include <vector>
#include <eigen3/Eigen/Core>
#include "config.h"
#include <memory>
#include <omp.h>

class Scene
{
    using Vec3 = Eigen::Vector3f;

private:
    Camera *_camera;
    std::vector<std::shared_ptr<Renderable>> _objs;
    Vec3 *_frameBuffer = nullptr;

    //Ray-scene intersection callback
    //Can be implemented more efficient
    Intersection (Scene::*_intersect)(const Ray &ray) const = &Scene::trivialIntersect;

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
    void addObject(std::shared_ptr<Renderable> renderable)
    {
        _objs.push_back(renderable);
    }

    // Intersection that can be optimized
    Intersection trivialIntersect(const Ray &ray) const
    {
        Intersection ret;
        for (const auto &obj : _objs)
        {
            Intersection intersection = obj->intersect(ray);
            if (intersection.t < ret.t)
                ret = intersection;
        }
        return ret;
    }

    void render() const
    {
        int w = _camera->nHorzPix(), h = _camera->nVertPix();
        for (int i = 0; i < h; i++)
        {
            int rowOffset = i * w;
#pragma omp parallel for num_threads(4)
            for (int j = 0; j < w; j++)
            {
                int bufferOffset = rowOffset + j;
                Ray ray = _camera->rayThroughFilm(i, j);
                Intersection intersection = (this->*_intersect)(ray);
                if (intersection.happen)
                    _frameBuffer[bufferOffset] = intersection.normal.cwiseAbs(); // TODO: 颜色计算
                else
                    _frameBuffer[bufferOffset] = BG_COLOR;
            }
        }
    }
    Vec3 *frameBuffer() const
    {
        return _frameBuffer;
    }
};