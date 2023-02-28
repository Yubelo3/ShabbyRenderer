#include <iostream>
#include <eigen3/Eigen/Core>
#include "image_encoder.hpp"
#include <cmath>
#include "intersection.hpp"
#include "renderable.hpp"

using Vec3 = Eigen::Vector3f;

int main()
{
    // int h = 1024, w = 1024;
    // ImageEncoder writer(h, w);
    // writer.writeTest();

    Shpere s;
    Ray ray({0.0f,0.0f,0.0f},{0.0f,0.0f,-1.0f});
    std::cout<<s.intersect(ray).t<<std::endl;

    return 0;
}