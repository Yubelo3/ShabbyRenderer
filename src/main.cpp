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
#include "obj_loader.hpp"
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

void setTestScene(Scene &scene)
{
    // load material and texture
    MtlLoader mtlLoader("../res/model/model.mtl");
    std::shared_ptr<Texture> tex = std::make_shared<Texture>("../res/models/rock/rock.png");

    // load meshes, create primitives
    ObjLoader loader;
    ObjPtr rock = loader.load("../res/models/rock/rock.obj");
    rock->transform(Vec3::Ones(),Vec3::Zero(),Vec3{-4.0,0.0,0.0});
    ObjPtr bunny = loader.load("../res/models/bunny/bunny.obj");
    ObjPtr blackShpere = std::make_shared<Shpere>(Vec3{4.0, 1.0, 1.0}, 1.0);
    ObjPtr transparentShpere = std::make_shared<Shpere>(Vec3{2.5, 1.0, 3.0}, 1.0);
    ObjPtr mirrorSphere = std::make_shared<Shpere>(Vec3{-0.0, -46.0, -10.0}, 45.0);

    // Set ideal mirror reflection material
    mtlLoader.materials()[3]->setKm(mtlLoader.materials()[3]->ks()*1.5);
    mtlLoader.materials()[3]->setNe(600.0);
    mtlLoader.materials()[3]->setKd(Vec3{0.6,0.6,0.6});
    mtlLoader.materials()[3]->setKa(Vec3{0.6,0.6,0.6});
    mtlLoader.materials()[3]->setKs(Vec3{0.8,0.8,0.8});

    // Set transparent material
    mtlLoader.materials()[1]->setKf(1.6f);
    mtlLoader.materials()[1]->setAttenuateCoeff(Vec3{0.9f, 0.93, 0.9f});
    mtlLoader.materials()[1]->setKa(Vec3{0.1f, 0.1f, 0.1f});
    mtlLoader.materials()[1]->setKd(Vec3{0.1f, 0.1f, 0.1f});
    mtlLoader.materials()[1]->setKs(Vec3{0.2f, 0.2f, 0.2f});

    mtlLoader.materials()[2]->setKf(1.2f);
    mtlLoader.materials()[2]->setAttenuateCoeff(Vec3{0.9f, 0.9, 0.98f});
    mtlLoader.materials()[2]->setKa(Vec3{0.1f, 0.1f, 0.1f});
    mtlLoader.materials()[2]->setKd(Vec3{0.1f, 0.1f, 0.1f});
    mtlLoader.materials()[2]->setKs(Vec3{0.2f, 0.2f, 0.2f});

    // Set material for objects
    // blackShpere->setMaterial(mtlLoader.materials()[0]);
    transparentShpere->setMaterial(mtlLoader.materials()[1]);
    mirrorSphere->setMaterial(mtlLoader.materials()[3]);
    rock->setMaterial(mtlLoader.materials()[0]);
    rock->setTexture(tex);
    bunny->setMaterial(mtlLoader.materials()[1]);

    // Add objects to scene
    scene.addObject(blackShpere);
    scene.addObject(transparentShpere);
    scene.addObject(mirrorSphere);
    scene.addObject(rock);
    scene.addObject(bunny);

    // Set lights
    LightPtr light1 = std::make_shared<PointLight>(Vec3{50.0, 50.0, 50.0}, Vec3{-4.0, 10.0, 0.0});
    // for(int i=0;i<8;i++)
    // {
    //     LightPtr areaLight = std::make_shared<AreaLight>(Vec3{15.0,15.0,15.0},Vec3{0.0,10.0,0.0},Vec3{1.0,0.0,0.0},Vec3{0.0,1.0,0.0});
    //     scene.addLight(areaLight);
    // }
    // LightPtr areaLight = std::make_shared<AreaLight>(Vec3{180.0,180.0,180.0},Vec3{0.0,10.0,0.0},Vec3{1.0,0.0,0.0},Vec3{0.0,1.0,0.0});
    LightPtr light2 = std::make_shared<AmbientLight>(Vec3{0.28, 0.28, 0.28});
    LightPtr light3 = std::make_shared<ParallelLight>(BG_COLOR * 0.3, Vec3{0.0, 0.0, -1.0});
    LightPtr light4 = std::make_shared<PointLight>(Vec3{3.0, 3.0, 3.0}, Vec3{-10.0, -1.0, 3.0});

    // Add lights to scene
    scene.addLight(light1);
    // scene.addLight(areaLight);
    scene.addLight(light2);
    scene.addLight(light3);
    scene.addLight(light4);
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
    renderScene(setTestScene);

    return 0;
}