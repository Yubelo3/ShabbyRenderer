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
#include <ctime>
#include "obj_loader.hpp"
#include "easy_random.hpp"
#include "../dep/lodepng/lodepng.h"

using Vec3 = Eigen::Vector3d;
using Mat3 = Eigen::Matrix3d;
using ObjPtr = std::shared_ptr<Renderable>;
using CameraPtr = std::shared_ptr<Camera>;
using LightPtr = std::shared_ptr<Light>;
using MtlPtr = std::shared_ptr<Material>;
using MeshPtr = std::shared_ptr<Mesh>;

CameraPtr initCamera()
{
    CameraPtr camera = std::make_shared<PerspectiveCamera>();
    camera->setFilm(FILM_WIDTH, FILM_HEIGHT);
    camera->setFocal(CAMERA_FOCAL_LENGTH);
    camera->setPosition(CAMERA_POS);
    camera->setPose(CAMERA_UP, CAMERA_LOOKAT);
    camera->setHFOV(CAMERA_HFOV);
    camera->setAspectRatio(CAMERA_ASPECT_RATIO);
    return camera;
}

void setSceneSphere(Scene &scene)
{
    MtlLoader mtlLoader("../res/model/model.mtl");
    mtlLoader.materials()[3]->setKm(mtlLoader.materials()[3]->ks() * 2.0);

    ObjPtr sphere1 = std::make_shared<Shpere>(Vec3{2.0, -2.0, -8.0}, 1.0);
    ObjPtr sphere2 = std::make_shared<Shpere>(Vec3{-3.0, 2.0, -10.0}, 2.0);
    ObjPtr sphere3 = std::make_shared<Shpere>(Vec3{2.0, 2.0, -10.0}, 1.0);
    ObjPtr sphere4 = std::make_shared<Shpere>(Vec3{-2.0, -2.0, -10.0}, 2.0);
    ObjPtr sphere5 = std::make_shared<Shpere>(Vec3{-0.0, -51.0, -10.0}, 45.0);
    sphere1->setMaterial(mtlLoader.materials()[0]);
    sphere2->setMaterial(mtlLoader.materials()[1]);
    sphere3->setMaterial(mtlLoader.materials()[2]);
    sphere4->setMaterial(mtlLoader.materials()[3]);
    sphere5->setMaterial(mtlLoader.materials()[3]);
    scene.addObject(sphere1);
    scene.addObject(sphere2);
    scene.addObject(sphere3);
    scene.addObject(sphere4);
    scene.addObject(sphere5);

    LightPtr light1 = std::make_shared<PointLight>(Vec3{23.0, 23.0, 23.0}, Vec3{0.0, 0.0, 0.0});
    LightPtr light2 = std::make_shared<AmbientLight>(Vec3{0.25, 0.25, 0.25});
    LightPtr light3 = std::make_shared<ParallelLight>(BG_COLOR * 0.5, Vec3{0.0, -1.0, 0.0});
    scene.addLight(light1);
    scene.addLight(light2);
    scene.addLight(light3);
}

void setSceneTriangle(Scene &scene)
{
    MtlLoader mtlLoader("../res/model/model.mtl");
    mtlLoader.materials()[3]->setKm(mtlLoader.materials()[3]->ks() * 2.0);

    ObjPtr triangle1 = std::make_shared<Triangle>(Vec3{-2.0, 0.0, -10.0}, Vec3{2.0, 0.0, -10.0}, Vec3{0.0, 2.0, -8.0});
    ObjPtr triangle2 = std::make_shared<Triangle>(Vec3{-2.0, -3.0, -8.0}, Vec3{1.0, -5.0, -10.0}, Vec3{0.0, 2.0, -4.0});
    ObjPtr sphere1 = std::make_shared<Shpere>(Vec3{2.0, -2.0, -8.0}, 1.0);
    ObjPtr sphere5 = std::make_shared<Shpere>(Vec3{-0.0, -51.0, -10.0}, 45.0);

    triangle1->setMaterial(mtlLoader.materials()[3]);
    triangle2->setMaterial(mtlLoader.materials()[3]);
    sphere1->setMaterial(mtlLoader.materials()[0]);
    sphere5->setMaterial(mtlLoader.materials()[3]);

    scene.addObject(triangle1);
    scene.addObject(triangle2);
    scene.addObject(sphere1);
    scene.addObject(sphere5);

    LightPtr light1 = std::make_shared<PointLight>(Vec3{23.0, 23.0, 23.0}, Vec3{0.0, 0.0, 0.0});
    LightPtr light2 = std::make_shared<AmbientLight>(Vec3{0.25, 0.25, 0.25});
    LightPtr light3 = std::make_shared<ParallelLight>(BG_COLOR * 0.5, Vec3{0.0, -1.0, 0.0});
    scene.addLight(light1);
    scene.addLight(light2);
    scene.addLight(light3);
}

void setSceneMesh(Scene &scene)
{
    MtlLoader mtlLoader("../res/model/model.mtl");
    // mtlLoader.materials()[3]->setKm(mtlLoader.materials()[3]->ks() * 1.0);
    mtlLoader.materials()[1]->setKf(1.1);
    mtlLoader.materials()[1]->setAttenuateCoeff(Vec3{0.9, 0.95, 0.9});
    mtlLoader.materials()[1]->setKa(Vec3{0.1, 0.1, 0.1});
    mtlLoader.materials()[1]->setKd(Vec3{0.1, 0.1, 0.1});
    mtlLoader.materials()[1]->setKs(Vec3{0.2, 0.2, 0.2});

    ObjLoader loader;
    // ObjPtr tri = loader.load("../res/model/teapot.obj");
    ObjPtr tri = loader.load("../res/models/bunny/bunny.obj");
    ObjPtr sphere1 = std::make_shared<Shpere>(Vec3{1.0, 2.0, -4.0}, 1.0);
    ObjPtr sphere5 = std::make_shared<Shpere>(Vec3{-0.0, -48.0, -10.0}, 45.0);

    tri->setMaterial(mtlLoader.materials()[1]);
    tri->transform(Vec3::Ones(), Vec3::Zero(), Vec3{0.0,1.0,0.0});
    sphere1->setMaterial(mtlLoader.materials()[3]);
    sphere5->setMaterial(mtlLoader.materials()[3]);

    scene.addObject(tri);
    scene.addObject(sphere1);
    scene.addObject(sphere5);

    LightPtr light1 = std::make_shared<PointLight>(Vec3{50.0, 50.0, 50.0}, Vec3{-10.0, 10.0, 2.0});
    LightPtr light2 = std::make_shared<AmbientLight>(Vec3{0.25, 0.25, 0.25});
    LightPtr light3 = std::make_shared<ParallelLight>(BG_COLOR * 0.2f, Vec3{0.0, -1.0, 0.0});
    scene.addLight(light1);
    scene.addLight(light2);
    scene.addLight(light3);
}

void setSceneTransparent(Scene &scene)
{
    MtlLoader mtlLoader("../res/model/model.mtl");
    mtlLoader.materials()[3]->setKm(mtlLoader.materials()[3]->ks() * 2.0);
    mtlLoader.materials()[1]->setKf(1.6f);
    mtlLoader.materials()[1]->setAttenuateCoeff(Vec3{0.9f, 1.0, 0.9f});
    mtlLoader.materials()[1]->setKa(Vec3{0.1f, 0.1f, 0.1f});
    mtlLoader.materials()[1]->setKd(Vec3{0.1f, 0.1f, 0.1f});
    mtlLoader.materials()[1]->setKs(Vec3{0.2f, 0.2f, 0.2f});

    ObjPtr sphere1 = std::make_shared<Shpere>(Vec3{4.0, 1.0, 3.0}, 1.0);
    ObjPtr sphere5 = std::make_shared<Shpere>(Vec3{-0.0, -48.0, -10.0}, 45.0);

    sphere1->setMaterial(mtlLoader.materials()[1]);
    sphere5->setMaterial(mtlLoader.materials()[3]);

    scene.addObject(sphere1);
    scene.addObject(sphere5);

    LightPtr light1 = std::make_shared<PointLight>(Vec3{23.0, 23.0, 23.0}, Vec3{0.0, 0.0, 10.0});
    LightPtr light2 = std::make_shared<AmbientLight>(Vec3{0.25, 0.25, 0.25});
    LightPtr light3 = std::make_shared<ParallelLight>(BG_COLOR * 0.5, Vec3{0.0, -1.0, 0.0});
    scene.addLight(light1);
    scene.addLight(light2);
    scene.addLight(light3);
}

void setSceneTexture(Scene &scene)
{
    MtlLoader mtlLoader("../res/model/model.mtl");
    std::shared_ptr<Texture> tex=std::make_shared<Texture>("../res/models/rock/rock.png");
    ObjLoader loader;
    ObjPtr rock = loader.load("../res/models/rock/rock.obj");

    mtlLoader.materials()[3]->setKm(mtlLoader.materials()[3]->ks() * 2.0);

    ObjPtr sphere1 = std::make_shared<Shpere>(Vec3{4.0, 1.0, 3.0}, 1.0);
    ObjPtr sphere5 = std::make_shared<Shpere>(Vec3{-0.0, -48.0, -10.0}, 45.0);

    sphere1->setMaterial(mtlLoader.materials()[1]);
    sphere5->setMaterial(mtlLoader.materials()[3]);
    rock->setMaterial(mtlLoader.materials()[1]);
    rock->setTexture(tex);

    scene.addObject(sphere1);
    scene.addObject(sphere5);
    scene.addObject(rock);

    LightPtr light1 = std::make_shared<PointLight>(Vec3{23.0, 23.0, 23.0}, Vec3{0.0, 0.0, 10.0});
    LightPtr light2 = std::make_shared<AmbientLight>(Vec3{0.25, 0.25, 0.25});
    LightPtr light3 = std::make_shared<ParallelLight>(BG_COLOR * 0.5, Vec3{0.0, -1.0, 0.0});
    scene.addLight(light1);
    scene.addLight(light2);
    scene.addLight(light3);
}

void renderScene(void (*setter)(Scene &))
{
    ImageEncoder writer(FILM_WIDTH, FILM_HEIGHT, "../imout.ppm");
    CameraPtr camera = initCamera();
    Scene scene;
    scene.setCamera(camera);

    setter(scene);

    scene.render();
    writer.write(scene.frameBuffer());
}

int main()
{
    // renderScene(setSceneMesh);
    // renderScene(setSceneSphere);
    // renderScene(setSceneTransparent);
    renderScene(setSceneTexture);

    // Texture tex("../res/models/rock/rock.png");
    // std::cout<<tex.getColor(0.5,0.5).transpose()<<"\n";

    return 0;
}