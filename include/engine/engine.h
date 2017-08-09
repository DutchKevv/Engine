#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "renderer.h"
#include "constants.h"
#include "baseDataObj.h"

#include "../../extern/json/json.hpp"

using json = nlohmann::json;

class Engine {
private:
    int counter = 0;
public:
    Renderer *renderer;

    vector<BaseDataObj *> dataObjects;

    Engine(int type = 0);

    int init();

    int addDataObj(BaseDataObj *dataObj, int id = -1);

    int updateDataObj(int id, json data);

    BaseDataObj *getDataObjById(int id);

    int update() {
        return 0;
    }

    int destroy() {
        return 0;
    }

    int loadModel() {
        return 0;
    }
};