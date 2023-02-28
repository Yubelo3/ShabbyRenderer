#include <string>
#include <assert.h>
#include <iostream>
#include <fstream>
#include "exception.hpp"
#include <eigen3/Eigen/Core>

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
    void write(const void *const data) const
    {
        if (!data)
            RAISE_ERROR("No data to write.");
        std::ofstream ofs(_filepath);
        if (!ofs.is_open())
            RAISE_ERROR("Failed to open output file.");

        ofs << "P6\n"
            << _width << " " << _height << '\n'
            << 255 << '\n'; // Binary representation

        unsigned long long offset = 0;
        for (int i = 0; i < _height; i++)
            for (int j = 0; j < _width; j++)
                for (int k = 0; k < 3; k++)
                {
                    unsigned char value = *(float *)((data + offset)) * 255.999f;
                    ofs << value;
                    offset += 4;
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