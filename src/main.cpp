#include <iostream>
#include <eigen3/Eigen/Core>

using vec3 = Eigen::Vector3f;

int main()
{
    vec3 a;
    a<<1.,2.,3.;
    std::cout<<a<<std::endl;
    return 0;
}