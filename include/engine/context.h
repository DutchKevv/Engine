#pragma once

#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include "constants.h"
#include "camera.h"
#include "renderer.h"

struct Context {

    Context();

    int type;
    int windowW;
    int windowH;
    int dayCycle;
    int dayCycleMax = 5000;

    float mouseLastX;
    float mouseLastY;
    bool firstMouse;

    GLFWwindow *window;
    Renderer *renderer;
    Camera *camera;
};

extern Context *context;