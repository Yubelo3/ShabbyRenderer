#include <iostream>
#include <eigen3/Eigen/Core>
#include "image_encoder.hpp"
#include <cmath>
#include "intersection.hpp"
#include "renderable.hpp"
#include "camera.hpp"
#include "scene.hpp"

using Vec3 = Eigen::Vector3f;

Camera initCamera(int w, int h)
{
    Camera camera;
    camera.setFilm(w, h);
    camera.setFocal(1.0f);
    camera.setPosition({0.0f, 0.0f, 0.0f});
    camera.setPose({0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f});
    camera.setWidth(2.0f);
    camera.setHeight(1.5f);
    return camera;
}

int main()
{
    int w = 1024, h = 768;
    ImageEncoder writer(w, h);
    Camera camera = initCamera(w, h);
    Scene scene;
    scene.setCamera(&camera);
    scene.addObject(new Shpere({0.0f, 0.0f, -5.0f}, 1.0f));
    scene.render();
    writer.write(scene.frameBuffer());
    return 0;
}