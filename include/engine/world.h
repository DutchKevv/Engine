//
// Created by Kewin Brandsma on 04/08/2017.
//

#pragma once


#include "baseRenderObj.h"
#include "camera.h"

class World : public BaseRenderObj {
    unsigned int idCounter = 0;
public:
    BaseRenderObj *player;
    BaseRenderObj *skybox;
    BaseRenderObj *hud;

    vector<BaseRenderObj *> renderObjects;

    World();

    int init();

    int update();

    int draw();

    int renderScene(Shader &shader, bool isShadowRender);

    int renderShadowScene(Shader &shader);

    int attachSkybox(BaseRenderObj *instance);

    int attachHud(BaseRenderObj *instance);

    int attachPlayer(BaseRenderObj *instance);

    int attachRenderObj(BaseRenderObj *renderObj);

    int attachCamera(Camera *camera);

    int renderSingleObj(int id, int width, int height);

    BaseRenderObj *getRenderObjectById(int id);

    int initShadowMap();

    int toggleFocusedRenderObj(BaseRenderObj *renderObj);

    int destroy();
};
