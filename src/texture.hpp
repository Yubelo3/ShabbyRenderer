#pragma once
#include <eigen3/Eigen/Core>
#include <string>
#include "image_encoder.hpp"
#include "../dep/lodepng/lodepng.h"

class Texture
{
    using Vec3 = Eigen::Vector3d;

private:
    unsigned char *_buffer = nullptr;
    Vec3 *_image = nullptr;
    double _w;
    double _h;

public:
    Texture(const std::string &filename)
    {
        unsigned w, h;
        if (lodepng_decode24_file(&_buffer, &w, &h, filename.c_str()))
            RAISE_ERROR("Failed to open png file");
        _w = w;
        _h = h;
        _image = new Vec3[w * h];
        unsigned long offset = 0;
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++)
                for (int k = 0; k < 3; k++)
                    ((double *)_image)[offset++] = _buffer[offset] / 255.0;

        delete[] _buffer;
    };
    ~Texture()
    {
        delete[] _image;
    }

private:
    inline double _arrCoordX(double u) const
    {
        //[0,1] => [0,w]
        return u * (_w - 1.0);
    }
    inline double _arrCoordY(double v) const
    {
        //[0,1] =>[h,0]
        return (1.0 - v) * (_h - 1.0);
    }

public:
    const Vec3 &getColor(double u, double v) const
    {
        int row = std::round(_arrCoordY(v));
        int col = std::round(_arrCoordY(u));
        return _image[row * int(_w) + col];
    }

public:
    // void write()
    // {
    //     ImageEncoder writer(_w, _h, "../imout.ppm");
    //     writer.write(_image);
    // }
};

// unsigned char* image;
//     unsigned w,h;
//     if(lodepng_decode24_file(&image,&w,&h,"../res/models/rock/rock.png"))
//         RAISE_ERROR("failed to open png");

//     double* frameBuffer=new double[w*h*3];
//     long offset=0;
//     for(int i=0;i<h;i++)
//         for(int j=0;j<w;j++)
//             for(int k=0;k<3;k++)
//             {
//                 frameBuffer[offset]=double(image[offset])/255.0;
//                 offset++;
//             }

//     ImageEncoder writer(w, h, "../imout.ppm");
//     writer.write(frameBuffer);