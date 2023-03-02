#pragma once
#include "camera.hpp"
#include "renderable.hpp"
#include <vector>
#include <eigen3/Eigen/Core>
#include "config.h"
#include <memory>
#include <omp.h>
#include "light.hpp"
#include "shader.hpp"

class Scene
{
    using Vec3 = Eigen::Vector3f;
    using ObjPtr=std::shared_ptr<Renderable>;
    using LightPtr=std::shared_ptr<Light>;
    using CameraPtr=std::shared_ptr<Camera>;

private:
    CameraPtr _camera=nullptr;
    std::vector<ObjPtr> _objs;
    std::vector<LightPtr> _lights;
    Vec3 *_frameBuffer = nullptr;
    //Ray-scene intersection callback
    //Can be implemented more efficient
    Intersection (Scene::*_intersect)(const Ray &ray) const = &Scene::trivialIntersect;
    PhongShader shader;

public:
    Scene(){};
    ~Scene()
    {
        delete[] _frameBuffer;
    }

public:
    // TODO: 引用修正
    void setCamera(CameraPtr camera)
    {
        _camera = camera;
        delete[] _frameBuffer;
        int bufferSize = (camera->nHorzPix()) * (camera->nVertPix());
        _frameBuffer = new Vec3[bufferSize];
    }
    void addObject(ObjPtr renderable)
    {
        _objs.push_back(renderable);
    }
    void addLight(LightPtr light)
    {
        _lights.push_back(light);
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
                    _frameBuffer[bufferOffset] = shader.getColor(_lights,intersection); // TODO: 颜色计算
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