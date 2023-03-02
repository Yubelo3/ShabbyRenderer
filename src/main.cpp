#include <iostream>
#include <eigen3/Eigen/Core>
#include "image_encoder.hpp"
#include <cmath>
#include "intersection.hpp"
#include "renderable.hpp"
#include "camera.hpp"
#include "scene.hpp"
#include <memory>
#include "config.h"

using Vec3 = Eigen::Vector3f;

Camera initCamera()
{
    Camera camera;
    camera.setFilm(FILM_WIDTH, FILM_HEIGHT);
    camera.setFocal(CAMERA_FOCAL_LENGTH);
    camera.setPosition(CAMERA_POS);
    camera.setPose(CAMERA_UP, CAMERA_LOOKAT);
    camera.setHFOV(CAMERA_HFOV);
    camera.setAspectRatio(CAMERA_ASPECT_RATIO);
    return camera;
}

int main()
{
    ImageEncoder writer(FILM_WIDTH, FILM_HEIGHT, "../imout.ppm");
    Camera camera = initCamera();
    Scene scene;
    scene.setCamera(&camera);
    scene.addObject(std::make_shared<Shpere>(Vec3{0.0f, 0.0f, -10.0f}, 1.0f));
    scene.addObject(std::make_shared<Shpere>(Vec3{2.0f, 0.0f, -5.0f}, 1.0f));
    scene.render();
    writer.write(scene.frameBuffer());
    return 0;
}