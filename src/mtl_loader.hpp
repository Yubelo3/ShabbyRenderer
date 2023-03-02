#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "utils.hpp"
#include "material.hpp"
#include <sstream>

class MtlLoader
{
    using Vec3 = Eigen::Vector3f;
    using MtlPtr = std::shared_ptr<Material>;

private:
    std::string _filepath;
    std::vector<MtlPtr> _materials;
    MtlPtr _curMtl = nullptr;

private:
    void _parseName(const std::string &line)
    {
        int nameBegin = 0;
        while (line[nameBegin++] != ' ')
            ;
        _curMtl->setName(line.substr(nameBegin));
    }
    void _parseK(const std::string &line, void (Material::*setter)(const Vec3 &))
    {
        std::string buf;
        Vec3 color;
        std::stringstream ss(line);
        ss >> buf >> color[0] >> color[1] >> color[2];
        (_curMtl.get()->*setter)(color);
    }

public:
    MtlLoader(){};
    MtlLoader(const std::string &filepath)
    {
        load(filepath);
    }

public:
    void load(const std::string &filepath)
    {
        _filepath = filepath;
        std::ifstream ifs(filepath);
        if (!ifs.is_open())
            RAISE_ERROR("Failed to open file");

        std::string lineBuf;
        while (std::getline(ifs, lineBuf))
        {
            switch (lineBuf[0])
            {
            case 'K': // Ka Kd Ks Ke
                switch (lineBuf[1])
                {
                case 'a':
                    _parseK(lineBuf, &Material::setKa);
                    break;
                case 'd':
                    _parseK(lineBuf, &Material::setKd);
                    break;
                case 's':
                    _parseK(lineBuf, &Material::setKs);
                    break;
                case 'e':
                    _parseK(lineBuf, &Material::setKe);
                    break;
                default:
                    break;
                }
                break;
            case 'n': // newmtl
                if (_curMtl)
                    _materials.push_back(_curMtl);
                _curMtl = std::make_shared<Material>();
                _parseName(lineBuf);
                break;
            case 'i': // illum
                break;
            case 'd': // d
                break;
            case 'T': // Tr Tf
                break;
            case 'm': // map_Ka map_Kd
                break;
            default:
                break;
            }
        }
        ifs.close();
        _curMtl = nullptr;
    }

    const std::vector<MtlPtr> &materials() const
    {
        return _materials;
    }
};