#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "logger.h"
#include "../../extern/stb_image.h"
#include "baseRenderObj.h"
#include "spriteRenderer.h"
#include "camera.h"

class Renderer {

private:
    int counter = 0;
    bool dirty = true;
public:
    SpriteRenderer *sprite;
    BaseRenderObj *player;
    BaseRenderObj *skybox;
    BaseRenderObj *hud;

    vector<BaseRenderObj *> renderObjects;

    Renderer();

    void tickLoop();

    void startLoop();

    void stopLoop();

    void gameLoop();

    void appLoop();

    int renderSingleObj(int id, int width, int height);

    BaseRenderObj *getRenderObjectById(int id);

    int initWindow();

    int setWindowSize(int width, int height);

    int initSpriteHandler();

    int attachCamera(Camera *camera);

    int attachSkybox(BaseRenderObj *instance);

    int attachHud(BaseRenderObj *instance);

    int attachPlayer(BaseRenderObj *instance);

    int attachRenderObj(BaseRenderObj *instance);

    void mouseCallback(GLFWwindow *window, double xpos, double ypos);

    void scrollCallback(double xoffset, double yoffset);

    void windowSizeCallback(int width, int height);

    int initShadowMap();

    void destroy();
};
