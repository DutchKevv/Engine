//
// Created by Kewin Brandsma on 04/08/2017.
//

#include "world.h"
#include "context.h"

World::World() : BaseRenderObj() {

}

int World::init() {
    BaseRenderObj::init();
    return 0;
}

int World::update() {
    BaseRenderObj::update();
    return 0;
}

int World::draw() {
    BaseRenderObj::draw();
    return 0;
}

int World::renderScene(Shader &shader, bool isShadowRender) {
    BaseRenderObj::renderScene(shader, isShadowRender);
}

/**
 *
 * @param instance
 * @return
 */
int World::attachSkybox(BaseRenderObj *instance) {
    instance->id = idCounter++;
    instance->init();
    this->skybox = instance;
    return 0;
}

/**
 *
 * @param instance
 * @return
 */
int World::attachHud(BaseRenderObj *instance) {
    instance->id = idCounter++;
    instance->init();
    this->hud = instance;
    return 0;
}

/**
 *
 * @param instance
 * @return
 */
int World::attachPlayer(BaseRenderObj *instance) {
    instance->id = idCounter++;
    instance->init();
    this->player = instance;
    this->attachRenderObj(instance);
    return 0;
}

/**
 * Add object to render list
 * @param renderObj
 * @return
 */
int World::attachRenderObj(BaseRenderObj *renderObj) {
    renderObj->id = idCounter++;
    renderObjects.push_back(renderObj);
    renderObj->init();
    return renderObj->id;
}

/**
 * Attach camera
 * @param camera
 * @return
 */
int World::attachCamera(Camera *camera) {
    context->camera = camera;
    return 0;
}


/**
 *
 * @param id
 * @param width
 * @param height
 * @return
 */
int World::renderSingleObj(int id, int width, int height) {
    BaseRenderObj *renderObject = getRenderObjectById(id);

    if (!renderObject) {
        consoleLog("Error - RenderObject not found!");
        consoleLog(id);
        return -1;
    }

    context->renderer->setWindowSize(width, height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Shader shader = ResourceManager::GetShader("chart");
    renderObject->renderScene(shader, false);

    glfwSwapBuffers(context->window);
#ifdef __EMSCRIPTEN__
    EM_ASM_({
         window.Module.custom.updateClientCanvas($0);
    }, renderObject->id);
#endif
    return 0;
}

/**
 * Get RenderObject by id
 * @param id
 * @return
 */
BaseRenderObj *World::getRenderObjectById(int id) {
    for (BaseRenderObj *renderObject : renderObjects) {
        if (renderObject->id == id) {
            return renderObject;
        }
    }

    return NULL;
}

int World::toggleFocusedRenderObj(BaseRenderObj *renderObj) {
    for (BaseRenderObj *renderObject : renderObjects) {
        renderObject->focused = false;
    }

    renderObj->focused = true;
    return 0;
}

int World::initShadowMap() {
    // -----------------------



    return 0;
}


int World::destroy() {
    BaseRenderObj::destroy();
    return 0;
}