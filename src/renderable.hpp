#pragma once
#include "intersection.hpp"
#include "ray.hpp"
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include "material.hpp"
#include <cmath>
#include "utils.hpp"

class AABB
{
    using Vec3 = Eigen::Vector3f;

private:
    Vec3 _min = {INF, INF, INF};
    Vec3 _max = {-INF, -INF, -INF};

public:
    AABB(){};
    AABB(const Vec3 &min, const Vec3 &max) : _min(min), _max(max){};

public:
    bool intersect(const Ray &ray) const
    {
        float tMin = 0.0f, tMax = INF;
        Vec3 O = ray.orig();
        Vec3 D = ray.dir();
        for (int i : {0, 1, 2})
        {
            float minBound = _min[i], maxBound = _max[i];
            float tMinBound = (minBound - O[i]) / D[i], tMaxBound = (maxBound - O[i]) / D[i];
            if (tMinBound > tMaxBound)
                std::swap(tMinBound, tMaxBound);
            tMin = std::max(tMin, tMinBound);
            tMax = std::min(tMax, tMaxBound);
            if (tMin > tMax)
                return false;
        }
        return true;
    }

public:
    void set(const Vec3 &min, const Vec3 &max)
    {
        _min = min;
        _max = max;
    }
    inline Vec3 centroid() const
    {
        return (_min + _max) / 2.0f;
    }
    inline Vec3 len() const
    {
        return _max - _min;
    }
    inline void expand(const AABB &aabb)
    {
        _min = _min.cwiseMin(aabb.min());
        _max = _max.cwiseMax(aabb.max());
    }
    inline const Vec3 &min() const
    {
        return _min;
    }
    inline const Vec3 &max() const
    {
        return _max;
    }
};

class Renderable
{
    using MtlPtr = std::shared_ptr<Material>;
    using TexPtr = std::shared_ptr<Texture>;

protected:
    MtlPtr _material = nullptr;
    TexPtr _texture = nullptr;
    AABB _aabb;

public:
    Renderable(){};

public:
    void setMaterial(MtlPtr mtl)
    {
        _material = mtl;
    }
    inline const AABB &aabb() const
    {
        return _aabb;
    }
    virtual Intersection intersect(const Ray &) const = 0;
    virtual void transform() = 0;
};

class Shpere : public Renderable
{
    using Vec3 = Eigen::Vector3f;
    using MtlPtr = std::shared_ptr<Material>;

private:
    Vec3 _c = {0.0f, 0.0f, -5.0f};
    float _r = 2.0f;

public:
    Shpere(const Vec3 &c, float r) : _c(c), _r(r)
    {
        _aabb.set(c - Vec3{r, r, r}, c + Vec3{r, r, r});
    }

public:
    // override functions
    Intersection intersect(const Ray &ray) const override
    {
        if (!_aabb.intersect(ray))
            return Intersection();
        Vec3 o = ray.orig() - _c, d = ray.dir();
        float A = d.dot(d), B = 2 * o.dot(d), C = o.dot(o) - _r * _r;
        float delta = B * B - 4 * A * C;
        if (delta < 0.0f)
            return Intersection();
        float tMin = (-B - sqrt(delta)) / 2.0f / A;
        float tMax = (-B + sqrt(delta)) / 2.0f / A;
        float t;
        if (tMin > tMax)
            std::swap(tMin, tMax);
        if (tMax < EPS)
            return Intersection();
        if (tMin < EPS)
            t = tMax;
        else
            t = tMin;
        Intersection inter;
        inter.happen = true;
        inter.t = t;
        inter.viewDir = -ray.dir();
        inter.pos = ray.orig() + t * d;
        inter.normal = (inter.pos - _c).normalized();
        if (_material)
            inter.mtl = _material;
        else
            inter.mtl = DEFAULT_MATERIAL;
        return inter;
    }

    void transform() override
    {
    }
};

class Triangle : public Renderable
{
    // By default, triangle use (v1-v0).cross(v2-v0) as face normal
    // This can be changed via setFaceNormal()
    using Vec3 = Eigen::Vector3f;
    using Vec2 = Eigen::Vector2f;
    using Mat3 = Eigen::Matrix3f;
    using MtlPtr = std::shared_ptr<Material>;

private:
    Vec3 _v[3];                  // counter-clockwise vertices
    Vec2 _vt[3];                 // vertex uv
    Vec3 _n[3] = {Vec3::Zero()}; // vertex normal
    Vec3 _normal;                // face normal

public:
    Triangle(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2)
    {
        _v[0] = v0, _v[1] = v1, _v[2] = v2;
        _normal = (v1 - v0).cross(v2 - v0).normalized();
        Mat3 V;
        Vec3 min = _v[0].cwiseMin(_v[1].cwiseMin(_v[2]));
        Vec3 max = _v[0].cwiseMax(_v[1].cwiseMax(_v[2]));
        _aabb.set(min, max);
    };

public: // override functions
    // Why so slow?
    Intersection intersect(const Ray &ray) const override
    {
        Intersection inter;
        if (!_aabb.intersect(ray))
            return inter;

        // SOLVE: o+td=a+beta*e1+gamma*e2
        // That is, [e1,e2,-d][beta,gamma,t]^T=o-a

        const Vec3 &a_b = _v[0] - _v[1], a_c = _v[0] - _v[2], d = ray.dir(), b = _v[0] - ray.orig();
        Mat3 A, Beta, Gamma, T;
        A << a_b, a_c, d;
        float detA = A.determinant();
        if (std::abs(detA) < EPS)
            return inter;

        Beta << b, a_c, d;
        float beta = Beta.determinant() / detA;
        if (beta < 0.0 || beta > 1.0f)
            return inter;

        Gamma << a_b, b, d;
        float gamma = Gamma.determinant() / detA;
        if (gamma < 0.0f || gamma + beta > 1.0f)
            return inter;

        T << a_b, a_c, b;
        float t = T.determinant() / detA;
        if (t < EPS)
            return inter;
        float alpha = 1.0f - beta - gamma;

        float hitDir = (ray.dir().dot(_normal) > 0.0f ? -1.0f : 1.0f);
        inter.happen = true;
        inter.pos = ray(t);
        if (_material)
            inter.mtl = _material;
        else
            inter.mtl = DEFAULT_MATERIAL;
        if (_vt[0].isZero())
            inter.normal = _normal * hitDir;
        else
            inter.normal = alpha * _n[0] + beta * _n[1] + gamma * _n[2];
        inter.t = t;
        inter.viewDir = ray.dir();

        if (_texture)
        {
            inter.payload = std::make_shared<Payload>();
            inter.payload->bcCoord = Vec3{alpha, beta, gamma};
            inter.payload->texture = _texture;
            inter.payload->uv = alpha * _vt[0] + beta * _vt[1] + gamma * _vt[2];
        }

        return inter;
    }

    void transform() override
    {
    }

public:
    // parameter setters
    void setVertexNormal(const Vec3 &n0, const Vec3 &n1, const Vec3 &n2)
    {
        _n[0] = n0.normalized();
        _n[1] = n1.normalized();
        _n[2] = n2.normalized();
    }
    void setVertexUV(const Vec2 &uv0, const Vec2 &uv1, const Vec2 &uv2)
    {
        _vt[0] = uv0;
        _vt[1] = uv1;
        _vt[2] = uv2;
    }
    void setFaceNormal(const Vec3 &n)
    {
        _normal = n.normalized();
    }
};

class BVHNode
{
    using ObjPtr = std::shared_ptr<Renderable>;
    using NodePtr = std::shared_ptr<BVHNode>;
    using Vec3 = Eigen::Vector3f;

private:
    NodePtr _left = nullptr;
    NodePtr _right = nullptr;
    AABB _aabb;
    ObjPtr _obj = nullptr;

public:
    BVHNode(){};

public:
    Intersection intersect(const Ray &ray) const
    {
        if (!_aabb.intersect(ray))
            return Intersection();
        if (!_left)
            return _obj->intersect(ray);
        Intersection i1 = _left->intersect(ray);
        Intersection i2 = _right->intersect(ray);
        return i1.t < i2.t ? i1 : i2;
    }

public:
    static NodePtr build(std::vector<ObjPtr> objs)
    {
        NodePtr root = std::make_shared<BVHNode>();
        assert(objs.size() > 0);
        std::vector<ObjPtr> leftObj, rightObj;
        if (objs.size() == 1)
        {
            root->_aabb = objs[0]->aabb();
            root->_obj = objs[0];
            return root;
        }

        root->_aabb.set(Vec3{INF, INF, INF}, Vec3{-INF, -INF, -INF});
        for (const ObjPtr &obj : objs)
            root->_aabb.expand(obj->aabb());

        int maxDim;
        root->_aabb.len().maxCoeff(&maxDim);
        float divisionx2 = root->_aabb.min()[maxDim] + root->_aabb.max()[maxDim];

        for (const ObjPtr &obj : objs)
        {
            if (obj->aabb().min()[maxDim] + obj->aabb().max()[maxDim] > divisionx2)
                rightObj.push_back(obj);
            else
                leftObj.push_back(obj);
        }
        if (leftObj.size() == 0)
        {
            leftObj.push_back(rightObj.back());
            rightObj.pop_back();
        }
        else if (rightObj.size() == 0)
        {
            rightObj.push_back(leftObj.back());
            leftObj.pop_back();
        }
        root->_left = build(std::move(leftObj));
        root->_right = build(std::move(rightObj));
        return root;
    }
};

class Mesh : public Renderable
{
    using Vec3 = Eigen::Vector3f;
    using MtlPtr = std::shared_ptr<Material>;
    using ObjPtr = std::shared_ptr<Renderable>;
    using NodePtr = std::shared_ptr<BVHNode>;

private:
    std::vector<ObjPtr> _primitives;
    NodePtr _bvh = nullptr;

public:
    Mesh()
    {
        _aabb.set(Vec3{INF, INF, INF}, Vec3{-INF, -INF, -INF});
    };

public: // override functions
    Intersection intersect(const Ray &ray) const override
    {
        Intersection inter = _bvh->intersect(ray);
        inter.mtl = _material;
        return inter;
    }
    void transform() override
    {
    }

public: // parameter setters
    void appendTriangle(ObjPtr triangle)
    {
        _primitives.push_back(triangle);
        _aabb.expand(triangle->aabb());
        _bvh = nullptr;
    }
    void buildBVH()
    {
        _bvh = BVHNode::build(_primitives);
    }
    inline std::size_t numTriangles() const
    {
        return _primitives.size();
    }
};