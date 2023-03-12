#pragma once
#include <eigen3/Eigen/Core>

#define MULTI_THREAD
#define NUM_THREADS 16

// const Eigen::Vector3d CAMERA_POS = {0.0f, 0.0f, 10.0f};
// const Eigen::Vector3d CAMERA_LOOKAT = {0.0f, 0.0f, -1.0f};

// const Eigen::Vector3d CAMERA_POS = {0.0f, 0.0f, 3.0f};
// const Eigen::Vector3d CAMERA_LOOKAT = {0.0f, 0.0f, -1.0f};
// const Eigen::Vector3d CAMERA_UP = {0.0f, 1.0f, 0.0f};

const Eigen::Vector3d CAMERA_POS = {0.0, 5.0, 15.0};
const Eigen::Vector3d CAMERA_LOOKAT = {0.0, -0.5, -1.0};
const Eigen::Vector3d CAMERA_UP = {0.0, 1.0, -0.5};

const Eigen::Vector3d BG_COLOR = {0.27, 0.4, 0.65};
const int FILM_HEIGHT = 192;
const int FILM_WIDTH = 256;
const double CAMERA_FOCAL_LENGTH = 1.0f;
const double CAMERA_HFOV = 60.0f;
const double CAMERA_ASPECT_RATIO = (double)FILM_WIDTH / FILM_HEIGHT;

int MULTI_SHADOW_RAY = 1;

const int MAX_BOUNCE = 10;