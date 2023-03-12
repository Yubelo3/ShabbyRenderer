#pragma once
#include <eigen3/Eigen/Core>
#include <string>
#include "texture.hpp"
#include <memory>

class Material
{
    using Vec3 = Eigen::Vector3d;

private:
    std::string _name;
    Vec3 _ka = {1.0, 1.0, 1.0};
    Vec3 _kd = {1.0, 1.0, 1.0};
    Vec3 _ks = {1.0, 1.0, 1.0};
    double _ne = 100.0;
    Vec3 _ke = Vec3::Zero();                // emitting
    Vec3 _km = Vec3::Zero();                // ideal mirror reflection
    Vec3 _kg = Vec3::Zero();                // "matte" mirror reflection
    double _g = 0.05;                        // Defines how "matte" the material is
    double _kf = 0.0;                       // index of refraction(only for transparent material)
    Vec3 _attenuateCoeff = {0.1, 0.1, 0.1}; // only for transparent material
    std::shared_ptr<Texture> _texture = nullptr;

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
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
    inline const Vec3 &km() const
    {
        return _km;
    }
    inline const Vec3 &kg() const
    {
        return _kg;
    }
    inline const Vec3 &attenuateCoeff() const
    {
        return _attenuateCoeff;
    }
    inline double kf() const
    {
        return _kf;
    }
    inline double ne() const
    {
        return _ne;
    }
    inline double g() const
    {
        return _g;
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
    void setKm(const Vec3 &km)
    {
        _km = km;
    }
    void setKg(const Vec3 &kg)
    {
        _kg = kg;
    }
    void setAttenuateCoeff(const Vec3 &att)
    {
        _attenuateCoeff = att;
    }
    void setKf(double kf)
    {
        _kf = kf;
    }
    void setNe(double ne)
    {
        _ne = ne;
    }
    void setG(double g)
    {
        _g = g;
    }
    void setTexture(std::shared_ptr<Texture> texture)
    {
        _texture = texture;
    }
};

std::shared_ptr<Material> DEFAULT_MATERIAL = std::make_shared<Material>();