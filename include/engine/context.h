#pragma once

#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include "constants.h"
#include "camera.h"

struct Context {

    int type;
    int windowW;
    int windowH;

    float mouseLastX;
    float mouseLastY;
    bool firstMouse;

    GLFWwindow *window;
    Camera *camera;
};