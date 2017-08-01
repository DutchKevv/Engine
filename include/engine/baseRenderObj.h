//
// Created by Kewin Brandsma on 31/07/2017.
//

#pragma once

#include <vector>
#include "context.h"
#include "shader.h"
#include "resourceManager.h"

class BaseRenderObj {
public:

    int id;

    vector<Shader> shaders;

    Context context;

    BaseRenderObj();

    virtual int init(Context &context);

    virtual int update();

    virtual int draw();

    virtual int destroy();

    virtual Shader addShader(const char *vertex, const char *fragment) {
        Shader shader = ResourceManager::LoadShader(vertex, fragment, NULL, "");
        return shader;
    }

private:


};
