#include <eigen3/Eigen/Core>
#include <memory>
#include "texture.hpp"

class Payload
{
    using Vec3 = Eigen::Vector3f;
    using Vec2 = Eigen::Vector2f;
    using TexPtr = std::shared_ptr<Texture>;

public:
    TexPtr texture = nullptr;
    Vec3 bcCoord;
    Vec2 uv;
};