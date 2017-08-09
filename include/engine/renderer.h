#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "logger.h"
#include "../../extern/stb_image.h"
#include "baseRenderObj.h"
#include "spriteRenderer.h"
#include "camera.h"
#include "world.h"

class Renderer {

public:
    vector<World*> worlds;
    SpriteRenderer *sprite;

    Renderer();

    void tickLoop();

    void startLoop();

    void stopLoop();

    void gameLoop();

    void appLoop();

    int initWindow();

    int setWindowSize(int width, int height);

    int initSpriteHandler();

    int attachWorld(World *world);

    void mouseCallback(GLFWwindow *window, double xpos, double ypos);

    void scrollCallback(double xoffset, double yoffset);

    void windowSizeCallback(int width, int height);

    void destroy();
};
