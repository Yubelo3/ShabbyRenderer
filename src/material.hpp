#pragma once
#include <eigen3/Eigen/Core>
#include <string>
#include "texture.hpp"
#include <memory>

class Material
{
    using Vec3 = Eigen::Vector3f;

private:
    std::string _name;
    Vec3 _ka = {1.0f, 1.0f, 1.0f};
    Vec3 _kd = {1.0f, 1.0f, 1.0f};
    Vec3 _ks = {1.0f, 1.0f, 1.0f};
    Vec3 _ke = {0.0f, 0.0f, 0.0f};
    float _ne = 100.0f;
    std::shared_ptr<Texture> _texture = nullptr;

public:
    Material(){};
    Material(const std::string &name, const Vec3 &ka, const Vec3 &kd, const Vec3 &ks) : _name(name), _ka(ka), _kd(kd), _ks(ks){};

public:
    inline const std::string &name() const
    {
        return _name;
    }
    inline const Vec3 &ka() const
    {
        return _ka;
    }
    inline const Vec3 &kd() const
    {
        return _kd;
    }
    inline const Vec3 &ks() const
    {
        return _ks;
    }
    inline const Vec3 &ke() const
    {
        return _ke;
    }
    inline float ne() const
    {
        return _ne;
    }
    void setName(const std::string &name)
    {
        _name = name;
    }
    void setKa(const Vec3 &ka)
    {
        _ka = ka;
    }
    void setKd(const Vec3 &kd)
    {
        _kd = kd;
    }
    void setKs(const Vec3 &ks)
    {
        _ks = ks;
    }
    void setKe(const Vec3 &ke)
    {
        _ke = ke;
    }
    void setNe(float ne)
    {
        _ne = ne;
    }
    void setTexture(std::shared_ptr<Texture> texture)
    {
        _texture = texture;
    }
};

std::shared_ptr<Material> DEFAULT_MATERIAL=std::make_shared<Material>();