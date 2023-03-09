#pragma once
#include <eigen3/Eigen/Core>

#define MULTI_THREAD
#define NUM_THREADS 6

// const Eigen::Vector3f CAMERA_POS = {0.0f, 0.0f, 10.0f};
// const Eigen::Vector3f CAMERA_LOOKAT = {0.0f, 0.0f, -1.0f};

const Eigen::Vector3f CAMERA_POS = {0.0f, 0.0f, 5.0f};
const Eigen::Vector3f CAMERA_LOOKAT = {0.0f, 0.0f, -1.0f};
const Eigen::Vector3f CAMERA_UP = {0.0f, 1.0f, 0.0f};

// const Eigen::Vector3f CAMERA_POS = {0.0f, 0.0f, 5.0f};
// const Eigen::Vector3f CAMERA_LOOKAT = {0.0f, -0.5f, -1.0f};
// const Eigen::Vector3f CAMERA_UP = {0.0f, 1.0f, -0.5f};

const Eigen::Vector3f BG_COLOR = {0.27f, 0.4f, 0.65f};
const int FILM_HEIGHT = 768;
const int FILM_WIDTH = 1024;
const float CAMERA_FOCAL_LENGTH = 1.0f;
const float CAMERA_HFOV = 60.0f;
const float CAMERA_ASPECT_RATIO = (float)FILM_WIDTH / FILM_HEIGHT;

const int MAX_BOUNCE = 5;