#pragma once
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include "light.hpp"
#include "material.hpp"
#include "intersection.hpp"
#include "ray.hpp"
#include "callback_base.hpp"
#include "config.h"
#include <cmath>

class Scene;

class Shader
{
    using Vec3 = Eigen::Vector3d;
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
    using Vec3 = Eigen::Vector3d;
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
        double cosTheta = std::max(0.0, N.dot(L));
        return kd.cwiseProduct(I) * cosTheta;
    }
    inline Vec3 _specular(const Vec3 &ks, const Vec3 &I, const Vec3 &N, const Vec3 &L, const Vec3 &V, double p) const
    {
        Vec3 H = (L + V).normalized();
        double cosAlpha = pow(std::max(0.0, N.dot(H)), p);
        return ks.cwiseProduct(I) * cosAlpha;
    }

    inline static double _schlickApproxim(const Vec3 &L, const Vec3 &N, double n)
    // use index of refraction to compute the proportion of reflection
    // L is always the dir in air
    {
        double cosTheta = N.dot(L);
        double R0 = pow(((n - 1.0) / (n + 1.0)), 2.0);
        return R0 + (1.0 - R0) * pow(1 - cosTheta, 5.0);
    }
    inline static Vec3 _reflectDir(const Vec3 &L, const Vec3 &N)
    {
        double cosTheta = N.dot(L);
        return 2.0 * cosTheta * N - L;
    }
    inline static Vec3 _refractDir(const Vec3 &L, const Vec3 &N, double n1, double n2)
    // if total internal reflection happens, return Vec3::Zero
    {
        double cosTheta1 = N.dot(L);
        double ifracRatio = n1 / n2;
        double cos2Theta2 = 1.0 - ifracRatio * ifracRatio * (1 - cosTheta1 * cosTheta1);
        if (cos2Theta2 < 0.0) // total internal reflection
            return Vec3::Zero();
        return -sqrt(cos2Theta2) * N - ifracRatio * (L - N * cosTheta1);
    }
    inline static Vec3 _attenuate(const Vec3& attenCoeff,double t)
    {
        auto [kr, kg, kb] = std::array{attenCoeff[0], attenCoeff[1], attenCoeff[2]};
        double r = exp(t * log(kr));
        double g = exp(t * log(kg));
        double b = exp(t * log(kb));
        return Vec3{r, g, b};
    }

    Vec3 _getInternalReflection(const Ray &ray, int depth = 1) const
    {
        if (depth > MAX_BOUNCE)
            return Vec3::Zero();
        Intersection inter = _scene->intersect(ray);
        if (!inter.happen)
            return BG_COLOR;
        double n = inter.mtl->kf();
        Vec3 attenuate=_attenuate(inter.mtl->attenuateCoeff(),inter.t);
        if (attenuate.maxCoeff() < 0.05)
            return Vec3::Zero();
        Vec3 refractDir = _refractDir(-ray.dir(), inter.normal, n, 1.0);

        // refraction+reflection
        double nReflect = _schlickApproxim(refractDir, -inter.normal, n);
        double nRefract = 1.0 - nReflect;
        Vec3 reflectDir = _reflectDir(-ray.dir(), inter.normal);
        Ray reflectRay(inter.pos, reflectDir);
        Vec3 reflection = _getInternalReflection(reflectRay, depth + 1).cwiseProduct(attenuate);
        if (refractDir.isZero()) // total internal reflection
            return reflection.cwiseProduct(attenuate);  

        Ray refractRay(inter.pos, refractDir);
        Intersection outInter = _scene->intersect(refractRay);
        if (outInter.happen)
            return (nRefract * getColor(outInter, depth + 1) + nReflect * reflection).cwiseProduct(attenuate);
        else
            return (BG_COLOR + nReflect * reflection).cwiseProduct(attenuate);
    }

public:
    Vec3 getColor(const Intersection &intersection, int depth = 0) const override
    {
        const std::vector<LightPtr> &lights = _scene->lights();
        MtlPtr mtl = intersection.mtl;
        Vec3 color = mtl->ke();
        Vec3 N = intersection.normal, V = intersection.viewDir;
        Vec3 ka = mtl->ka(), kd = mtl->kd(), ks = mtl->ks();
        if(intersection.texColor)
            kd=*intersection.texColor;
        double p = mtl->ne();

        //local illumination model
        for (const LightPtr &light : lights)
        {
            Vec3 I, L;
            light->idAt(intersection.pos, I, L);

            if (L.norm() < 0.01) // ambient
                color += _ambient(ka, I);
            else
                for(int i=0;i<MULTI_SHADOW_RAY;i++)
                {
                    double at=1.0;
                    Ray shadowRay(intersection.pos, L);
                    Intersection shadowInter=_scene->intersect(shadowRay);
                    if (shadowInter.happen)
                    {
                        if(shadowInter.mtl->kf()>1.0)
                            at/=pow(shadowInter.mtl->kf(),0.8);
                        else
                            continue;
                    }

                    color += at*_diffuse(kd, I, N, L)/MULTI_SHADOW_RAY; // diffuse term
                    color += at*_specular(ks, I, N, L, V, p)/MULTI_SHADOW_RAY;
                }

        }

        //manage reflection and refraction
        if (depth < MAX_BOUNCE)
        {
            if (mtl->km().maxCoeff() > 0.01) // ideal mirror reflection material
            {
                Vec3 ref=_reflectDir(V, N);
                Vec3 dst=intersection.pos+ref;
                if(mtl->g()>0.0)
                {
                    dst[0]+=easyUniform()*mtl->g();
                    dst[1]+=easyUniform()*mtl->g();
                    dst[2]+=easyUniform()*mtl->g();
                }
                Ray reflectRay(intersection.pos, dst-intersection.pos);
                Intersection reflectIntersection = _scene->intersect(reflectRay);
                if (reflectIntersection.happen)
                    color += mtl->km().cwiseProduct(getColor(reflectIntersection, depth + 1));
            }
            else if (mtl->kf() > 0.01) // transparent material
            {
                double nReflect = _schlickApproxim(V, N, mtl->kf());
                double nRefract = 1.0 - nReflect;

                // compute reflection
                Ray reflectRay(intersection.pos, _reflectDir(V, N));
                Intersection reflectIntersection = _scene->intersect(reflectRay);
                if (reflectIntersection.happen)
                    color += nReflect * getColor(reflectIntersection, depth + 1);
                else
                    color += nReflect * BG_COLOR;

                // compute refraction
                Vec3 refractDir = _refractDir(V, N, 1.0, mtl->kf());
                Ray refractRay(intersection.pos, refractDir);
                color += nRefract * _getInternalReflection(refractRay, depth + 1);
            }
        }

        return color;
    }
};
