#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "utils.hpp"
#include "renderable.hpp"
#include <sstream>

class ObjLoader
{
    using Vec3 = Eigen::Vector3f;
    using Vec2 = Eigen::Vector2f;
    using IVec3 = Eigen::Vector3i;
    using ObjPtr = std::shared_ptr<Renderable>;
    using TriPtr = std::shared_ptr<Triangle>;

private:
    std::stringstream _ss;
    std::vector<Vec3> _v;  // vertex position
    std::vector<Vec2> _vt; // vertex uv
    std::vector<Vec3> _vn; // vertex normal
    std::vector<IVec3> _vi; // faces index
    std::vector<IVec3> _vti; // uv index
    std::vector<IVec3> _vni; // normal index

public:
    ObjLoader(){};

private:
    void _parseVertexPosition(const std::string &line)
    {
        _ss.clear();
        _ss.str("");
        std::string buf;
        Vec3 v;
        _ss<<line;
        _ss >> buf >> v[0] >> v[1] >> v[2];
        _v.push_back(v);
    }
    void _parseVertexUV(const std::string &line)
    {
        _ss.clear();
        _ss.str("");
        std::string buf;
        Vec2 vt;
        _ss<<line;
        _ss >> buf >> vt[0] >> vt[1];
        _vt.push_back(vt);
    }
    void _parseVertexNormal(const std::string &line)
    {
        _ss.clear();
        _ss.str("");
        std::string buf;
        Vec3 vn;
        _ss<<line;
        _ss >> buf >> vn[0] >> vn[1] >> vn[2];
        _vn.push_back(vn);
    }
    void _parseFace(const std::string& line)
    {
        IVec3 f[3]={IVec3::Zero(),IVec3::Zero(),IVec3::Zero()};  //face index, uv index
        int ptr=1,flag=0,value=0,curVertex=0;
        while(ptr<line.length()-1)
        {
            ptr++;
            char c=line[ptr];
            if(c=='/')
            {
                f[flag][curVertex]=value;
                value=0;
                flag=(flag+1)%3;
                continue;
            }
            if(c==' ')
            {
                f[flag][curVertex]=value;
                flag=0;
                value=0;
                curVertex++;
                continue;
            }
            assert(c>='0'&&c<='9');
            value=value*10+(c-'0');
        }
        assert(curVertex==2);
        f[flag][2]=value;
        _vi.push_back(f[0]);
        if(!f[1].isZero())
            _vti.push_back(f[1]);
        if(!f[2].isZero())
            _vni.push_back(f[2]);
    }
    void _initialize()
    {
        _ss.clear();
        _ss.str("");
        _vi.clear();
        _vti.clear();
        _vni.clear();
        _v.clear();
        _v.push_back(Vec3());
        _vt.clear();
        _vt.push_back(Vec2());
        _vn.clear();
        _vn.push_back(Vec3());
    }


public:
    std::shared_ptr<Mesh> load(const std::string &filepath)
    {
        _initialize();
        std::shared_ptr<Mesh> ret=std::make_shared<Mesh>();

        std::ifstream ifs(filepath);
        if (!ifs.is_open())
            RAISE_ERROR("Failed to open file");

        std::string lineBuf;
        while (std::getline(ifs, lineBuf))
        {
            switch (lineBuf[0])
            {
            case 'v': // v,vt,vn
                switch (lineBuf[1])
                {
                case ' ': // v
                    _parseVertexPosition(lineBuf);
                    break;
                case 't': // vt
                    _parseVertexUV(lineBuf);
                    break;
                case 'n': // vn
                    _parseVertexNormal(lineBuf);
                    break;
                default:
                    break;
                }
                break;
            case 'f': // f
                _parseFace(lineBuf);
                break;
            case 'm': // mtllib
                break;
            default:
                break;
            }
        }

        for(int i=0;i<_vi.size();i++)
        {
            TriPtr tri=std::make_shared<Triangle>(
                _v[_vi[i][0]],_v[_vi[i][1]],_v[_vi[i][2]]
            );
            if(_vti.size()>0)
                tri->setVertexUV(
                    _vt[_vti[i][0]],_vt[_vti[i][1]],_vt[_vti[i][2]]
                );
            if(_vni.size()>0)
                tri->setVertexNormal(
                    _vn[_vni[i][0]],_vn[_vni[i][1]],_vn[_vni[i][2]]
                );
            ret->appendTriangle(tri);
        }

        ret->buildBVH();

        return ret;
    }
};