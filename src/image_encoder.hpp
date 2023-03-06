#pragma once
#include <string>
#include <assert.h>
#include <iostream>
#include <fstream>
#include "utils.hpp"
#include <eigen3/Eigen/Core>
#include <omp.h>

class ImageEncoder
{
    using Vec3 = Eigen::Vector3f;

private:
    std::string _filepath = "imout.ppm";
    int _width = 512;
    int _height = 512;

public:
    ImageEncoder(int w, int h, const std::string &filepath = "imout.ppm") : _width(w), _height(h), _filepath(filepath){};

public:
    void write(const void *const vData) const
    {
        if (!vData)
            RAISE_ERROR("No data to write.");
        std::ofstream ofs(_filepath);
        if (!ofs.is_open())
            RAISE_ERROR("Failed to open output file.");

        ofs << "P6\n"
            << _width << " " << _height << '\n'
            << 255 << '\n'; // Binary representation

        float *data = (float *)vData;
        int offset = 0;
        for (int i = 0; i < _height; i++)
            for (int j = 0; j < _width; j++)
                for (int k = 0; k < 3; k++)
                {
                    float value = std::clamp(*(data + offset), 0.0f, 1.0f);
                    // gamma矫正
                    value = pow(value, 1.0f / 2.2f);

                    unsigned char pValue = value * 255.999f;
                    ofs << pValue;
                    offset++;
                }
        ofs.close();
    }

    // Only for test
    void writeTest() const
    {
        Vec3 *image = new Vec3[_height * _width];
        int offset = 0;
        for (int i = 0; i < _height * _width; i++)
        {
            for (int k = 0; k < 3; k++)
                image[offset][k] = 0.3 * (sin(i) + cos(offset) + sin(k));
            offset++;
        }
        this->write(image);
        delete[] image;
    }
};