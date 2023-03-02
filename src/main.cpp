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
#include "mtl_loader.hpp"

using Vec3 = Eigen::Vector3f;
using ObjPtr = std::shared_ptr<Renderable>;
using CameraPtr = std::shared_ptr<Camera>;
using LightPtr = std::shared_ptr<Light>;
using MtlPtr = std::shared_ptr<Material>;

CameraPtr initCamera()
{
    CameraPtr camera = std::make_shared<Camera>();
    camera->setFilm(FILM_WIDTH, FILM_HEIGHT);
    camera->setFocal(CAMERA_FOCAL_LENGTH);
    camera->setPosition(CAMERA_POS);
    camera->setPose(CAMERA_UP, CAMERA_LOOKAT);
    camera->setHFOV(CAMERA_HFOV);
    camera->setAspectRatio(CAMERA_ASPECT_RATIO);
    return camera;
}

int main()
{
    ImageEncoder writer(FILM_WIDTH, FILM_HEIGHT, "../imout.ppm");
    CameraPtr camera = initCamera();
    Scene scene;
    scene.setCamera(camera);

    MtlLoader mtlLoader("../res/model/model.mtl");
    ObjPtr sphere1 = std::make_shared<Shpere>(Vec3{0.0f, 2.0f, -10.0f}, 1.0f);
    ObjPtr sphere2 = std::make_shared<Shpere>(Vec3{-2.0f, 2.0f, -10.0f}, 2.0f);
    ObjPtr sphere3 = std::make_shared<Shpere>(Vec3{2.0f, -2.0f, -10.0f}, 1.0f);
    ObjPtr sphere4 = std::make_shared<Shpere>(Vec3{-2.0f, -2.0f, -10.0f}, 1.0f);
    ObjPtr sphere5 = std::make_shared<Shpere>(Vec3{-0.0f, -25.0f, -10.0f}, 20.0f);
    sphere1->setMaterial(mtlLoader.materials()[0]);
    sphere2->setMaterial(mtlLoader.materials()[1]);
    sphere3->setMaterial(mtlLoader.materials()[2]);
    sphere4->setMaterial(mtlLoader.materials()[3]);
    sphere5->setMaterial(mtlLoader.materials()[3]);
    // scene.addObject(sphere1);
    scene.addObject(sphere2);
    scene.addObject(sphere3);
    scene.addObject(sphere4);
    scene.addObject(sphere5);

    LightPtr light1 = std::make_shared<PointLight>(Vec3{30.0f, 30.0f, 30.0f}, Vec3{0.0f, 0.0f, 0.0f});
    LightPtr light2 = std::make_shared<AmbientLight>(Vec3{0.5f, 0.5f, 0.5f});
    LightPtr light3 = std::make_shared<ParallelLight>(Vec3{0.5f, 0.8f, 0.8f}, Vec3{0.0f, -1.0f, 0.0f});
    scene.addLight(light1);
    scene.addLight(light2);
    scene.addLight(light3);

    scene.render();
    writer.write(scene.frameBuffer());

    return 0;
}