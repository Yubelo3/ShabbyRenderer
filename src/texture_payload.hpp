#include <eigen3/Eigen/Core>
#include <memory>
#include "texture.hpp"

class Payload
{
    using Vec3 = Eigen::Vector3d;
    using Vec2 = Eigen::Vector2d;
    using TexPtr = std::shared_ptr<Texture>;

public:
    TexPtr texture = nullptr;
    Vec3 bcCoord;
    Vec2 uv;
};