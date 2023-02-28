#pragma once
#include "camera.hpp"
#include "renderable.hpp"
#include <vector>

class Scene
{
private:
    Camera *_camera;
    std::vector<Renderable*> _meshes;

public:
    Scene()
    {
        _camera=new Camera();
    }

public:

};