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
#include "callback_base.hpp"
#include "easy_random.hpp"

class Scene : public SceneBase
{
    using Vec3 = Eigen::Vector3d;
    using ObjPtr = std::shared_ptr<Renderable>;
    using LightPtr = std::shared_ptr<Light>;
    using CameraPtr = std::shared_ptr<Camera>;

private:
    CameraPtr _camera = nullptr;
    std::vector<ObjPtr> _objs;
    std::vector<LightPtr> _lights;
    Vec3 *_frameBuffer = nullptr;
    // Ray-scene intersection callback
    // Can be implemented more efficient
    Intersection (Scene::*_intersect)(const Ray &ray) const = &Scene::trivialIntersect;
    PhongShader shader;

public:
    Scene()
    {
        shader.bind(this);
    };
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

    // Intersection callback
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

    Intersection intersect(const Ray &ray) const
    {
        return (this->*_intersect)(ray);
    }

    void render() const
    {
        double pixXOffset[] = {-0.25, -0.25, 0.25, 0.25};  //perform anti-aliasing
        double pixYOffset[] = {-0.25, 0.25, -0.25, 0.25};
        int w = _camera->nHorzPix(), h = _camera->nVertPix();
        std::uniform_real_distribution<double> distrib(-0.25, 0.25);
        for (int i = 0; i < h; i++)
        {
            printf("%d/%d\n", i, h);
            int rowOffset = i * w;
#ifdef MULTI_THREAD
#pragma omp parallel for schedule(dynamic, NUM_THREADS)
#endif
            for (int j = 0; j < w; j++)
            {
                int bufferOffset = rowOffset + j;
                Vec3 color = Vec3::Zero();
                for (int s = 0; s < 4; s++)
                {
                    double a = i + pixXOffset[s] + easyUniform() * 0.25;
                    double b = j + pixYOffset[s] + easyUniform() * 0.25;
                    Ray ray = _camera->rayThroughFilm(a, b);
                    Intersection intersection = (this->*_intersect)(ray);
                    if (intersection.happen)
                        color += shader.getColor(intersection);
                    else
                        color += BG_COLOR;
                }
                _frameBuffer[bufferOffset] = color / 4.0;
            }
        }
    }
    Vec3 *frameBuffer() const
    {
        return _frameBuffer;
    }

public:
    const std::vector<LightPtr> &lights() const
    {
        return _lights;
    }
};