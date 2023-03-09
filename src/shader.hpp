#pragma once
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include "light.hpp"
#include "material.hpp"
#include "intersection.hpp"
#include "ray.hpp"
#include "callback_base.hpp"
#include "config.h"

class Scene;

class Shader
{
    using Vec3 = Eigen::Vector3f;
    using LightPtr = std::shared_ptr<Light>;
    using MtlPtr = std::shared_ptr<Material>;

protected:
    SceneBase *_scene = nullptr;

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
    void bind(SceneBase *scene)
    {
        _scene = scene;
    }

    virtual Vec3 getColor(const Intersection &, int depth = 0) const = 0;
};

class PhongShader : public Shader
{
    using Vec3 = Eigen::Vector3f;
    using LightPtr = std::shared_ptr<Light>;
    using MtlPtr = std::shared_ptr<Material>;

public:
    PhongShader(){};

private:
    inline Vec3 _ambient(const Vec3 &ka, const Vec3 &I) const
    {
        return ka.cwiseProduct(I);
    }
    inline Vec3 _diffuse(const Vec3 &kd, const Vec3 &I, const Vec3 &N, const Vec3 &L) const
    {
        float cosTheta = std::max(0.0f, N.dot(L));
        return kd.cwiseProduct(I) * cosTheta;
    }
    inline Vec3 _specular(const Vec3 &ks, const Vec3 &I, const Vec3 &N, const Vec3 &L, const Vec3 &V, const float p) const
    {
        Vec3 H = (L + V).normalized();
        float cosAlpha = pow(std::max(0.0f, N.dot(H)), p);
        return ks.cwiseProduct(I) * cosAlpha;
    }

public:
    Vec3 getColor(const Intersection &intersection, int depth = 0) const override
    {
        const std::vector<LightPtr> &lights = _scene->lights();
        MtlPtr mtl = intersection.mtl;
        Vec3 color = mtl->ke();
        Vec3 N = intersection.normal, V = intersection.viewDir;
        Vec3 ka = mtl->ka(), kd = mtl->kd(), ks = mtl->ks();
        float p = mtl->ne();
        for (const LightPtr &light : lights)
        {
            Vec3 I, L;
            light->idAt(intersection.pos, I, L);

            if (L.norm() < 0.01f) // ambient
                color += _ambient(ka, I);
            else
            {
                Ray shadowRay(intersection.pos, L);
                if (_scene->intersect(shadowRay).happen)
                    continue;

                color += _diffuse(kd, I, N, L); // diffuse term
                color += _specular(ks, I, N, L, V, p);
            }
        }

        if (depth < MAX_BOUNCE && mtl->km().maxCoeff() > 0.01f)
        {
            float cosTheta = N.dot(V);
            Vec3 R = 2.0f * cosTheta * N - V;
            Ray reflectRay(intersection.pos, R);
            Intersection reflectIntersection = _scene->intersect(reflectRay);
            if (reflectIntersection.happen)
                color += mtl->km().cwiseProduct(getColor(reflectIntersection, depth + 1));
        }

        return color;
    }
};

// class BounceShader : public Shader
// {
//     using Vec3 = Eigen::Vector3f;
//     using LightPtr = std::shared_ptr<Light>;
//     using MtlPtr = std::shared_ptr<Material>;

// public:
//     BounceShader(){};

// private:
//     inline Vec3 _sampleUnitShpere() const
//     {
//         Vec3 ret = Vec3::Random();
//         while (ret[0] * ret[0] + ret[1] * ret[1] + ret[2] * ret[2] > 1.0f)
//             ret = Vec3::Random();
//         return ret;
//     }
//     Ray _constructBounce(const Vec3 &pos, const Vec3 &N) const
//     {
//         Vec3 dst = pos + N + _sampleUnitShpere();
//         return Ray(pos, dst - pos);
//     }
//     Vec3 _getColor(const std::vector<LightPtr> &lights, const Intersection &intersection, int depth) const
//     {
//         if (depth <= 0)
//             return Vec3::Zero();
//         float r = intersection.pos.norm();
//         Ray bounceRay = _constructBounce(intersection.pos, intersection.normal);
//         Intersection bounceIntersection = _scene->intersect(bounceRay);
//         if (bounceIntersection.happen)
//             return 0.5 * _getColor(lights, bounceIntersection, depth - 1);

//         float t = 0.5 * (bounceRay.dir()[1] + 1.0);
//         return (1.0f - t) * Vec3::Ones() + t * Vec3{0.3f, 0.5f, 0.5f};
//     }

// public:
//     Vec3 getColor(const Intersection &intersection) const override
//     {
//         return _getColor(_scene->lights(), intersection, 50);
//     }
// };