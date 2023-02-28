#include <iostream>
#include <eigen3/Eigen/Core>
#include "image_encoder.hpp"
#include <cmath>

using Vec3 = Eigen::Vector3f;

int main()
{
    int h = 1024, w = 1024;
    ImageEncoder writer(h, w);
    writer.writeTest();
    return 0;
}