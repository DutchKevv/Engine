//
// Created by Kewin Brandsma on 31/07/2017.
//

#pragma once

#include <vector>
#include "shader.h"
#include "resourceManager.h"

class BaseRenderObj {
public:

    int id;
    int width;
    int height;

    float speed;
    float xRadius = 0.0f;
    float yRadius = 0.0f;
    bool focused = false;

    glm::vec3 position;
    glm::vec3 rotation;

    BaseRenderObj();

    virtual int init();

    virtual int update();

    virtual int draw();

    virtual int checkKeys();

    virtual int destroy();

    virtual int renderScene(Shader &shader, bool isShadowRender);
};
