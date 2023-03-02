#pragma once
#include <eigen3/Eigen/Core>
#include "light.hpp"
#include "material.hpp"
#include "intersection.hpp"
#include "ray.hpp"
#include "callback_base.hpp"

class Scene;

class PhongShader
{
    using Vec3 = Eigen::Vector3f;
    using LightPtr = std::shared_ptr<Light>;
    using MtlPtr = std::shared_ptr<Material>;

private:
    SceneBase* _scene=nullptr;

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
    void bind(SceneBase* scene)
    {
        _scene=scene;
    }

    Vec3 getColor(const std::vector<LightPtr> &lights, const Intersection &intersection) const
    {
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
                Ray shadowRay(intersection.pos,L);
                if(_scene->intersect(shadowRay).happen)
                {
                    continue;
                }
                
                color += _diffuse(kd, I, N, L); // diffuse term
                color += _specular(ks, I, N, L, V, p);
            }
        }
        return color;
    }
};
