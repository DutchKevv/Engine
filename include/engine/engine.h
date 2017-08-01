#pragma once

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "renderer.h"
#include "context.h"
#include "constants.h"
#include "baseDataObj.h"

#include "../../extern/json/json.hpp"

using json = nlohmann::json;

class Engine {
private:
    int counter = 0;
public:
    Context *context;
    Renderer *renderer;

    vector<BaseDataObj *> dataObjects;

    Engine(int type = 0) {
        consoleLog("Engine init");

        this->init(type);
    };

    int init(int &type) {
        this->context = new Context();
        this->context->type = type;

        this->renderer = new Renderer(*this->context);

        this->renderer->initWindow();

        return 0;
    }

    int addDataObj(BaseDataObj *dataObj, int id = -1) {
        if (id == -1)
            id = counter++;

        dataObj->id = id;
        dataObjects.push_back(dataObj);
        return dataObj->id;
    }

    int
    updateDataObj(int id, json data) {
        BaseDataObj* dataObject = getDataObjById(id);

        if (!dataObject) {
            consoleLog("Error - DataObject not found");
            return -1;
        }

        dataObject->update(data);

        return 0;
    }

    BaseDataObj* getDataObjById(int id) {

        for (auto &dataObject : dataObjects) {
            if (dataObject->id == id) {
                return dataObject;
            }
        }

        return NULL;
    }

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