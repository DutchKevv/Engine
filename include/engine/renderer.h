#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "logger.h"
#include "../../extern/stb_image.h"
#include "context.h"
#include "baseRenderObj.h"


class Renderer {

private:
    int counter = 0;
    bool dirty = true;
public:
    Context &context;

    vector<BaseRenderObj *> renderObjects;

    Renderer(Context &context);

    void tickLoop();

    void startLoop();

    void stopLoop();

    void gameLoop();

    void appLoop();

    int renderSingleObj(int id, int width, int height);

    BaseRenderObj *getRenderObjectById(int id);

    int initWindow();

    int setWindowSize(int width, int height);

    int attachCamera(Camera *camera);

    int attachRenderObj(BaseRenderObj *instance);

    void mouseCallback(GLFWwindow *window, double xpos, double ypos);

    void scrollCallback(double xoffset, double yoffset);

    void windowSizeCallback(int width, int height);

    void destroy();
};
