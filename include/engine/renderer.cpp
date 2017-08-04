//
// Created by Kewin Brandsma on 20/07/2017.
//

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include "GL/glew.h"
#include <vector>
#include "../../extern/stb_image.h"
#include "logger.h"
#include "text.h"
#include "Level.h"
#include "renderer.h"
#include "baseRenderObj.h"
#include "engine.h"
#include "context.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
unsigned int depthMapFBO;
unsigned int depthMap;
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

unsigned int width = 1920;
unsigned int height = 1080;

using namespace std;

void _keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

void _mouseCallback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void _windowSizeCallback(GLFWwindow *window, int width, int height);

void _windowCloseCallback(GLFWwindow *window);

void _errorCallback(int error, const char *description);

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void _renderLoop();

Renderer *renderer;

Renderer::Renderer() {
    renderer = this;
};

int Renderer::setWindowSize(int width, int height) {
    glfwSetWindowSize(context->window, width, height);
    return 0;
}

void Renderer::tickLoop() {
    if (context->type == ENGINE_TYPE_GAME)
        this->gameLoop();
    else
        this->appLoop();
}

/**
 * Game Loop
 */
void Renderer::gameLoop() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (context->camera != NULL) {
//        context->camera->processInput(context->window);
    }

    for (BaseRenderObj *obj : renderObjects) {
        obj->update();
    }

//    lightPos = glm::vec3(player->position.x, player->position.y + 1.0f, player->position.z);

    Shader worldShader = ResourceManager::GetShader("world");
    Shader depthQuadShader = ResourceManager::GetShader("depthQuad");
    Shader depthShader = ResourceManager::GetShader("shadowMappingDepth");

    // Shader config
    worldShader.Use();
    worldShader.SetVector3f("light.direction", -0.2f, -0.2f, -2.3f);
//    worldShader.SetVector3f("viewPos", context->camera->Position);

    worldShader.SetVector3f("light.ambient", 0.7, 0.7, 0.7);
    worldShader.SetVector3f("light.diffuse", 0.5, 0.5, 0.5);
    worldShader.SetVector3f("light.specular", 0.7, 0.7, 0.7);

    worldShader.SetFloat("material.shininess", 32.0f);

    lightPos.x = sin(glfwGetTime()) * 3.0f;
    lightPos.z = cos(glfwGetTime()) * 2.0f;
    lightPos.y = 4.0 + cos(glfwGetTime()) * 1.0f;

    // 1. render depth of scene to texture (from light's perspective)
    // --------------------------------------------------------------
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    float near_plane = 1.0f, far_plane = 7.5f;
//    lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane);
    lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;

    // render scene from light's point of view
    depthShader.Use();
    depthShader.SetMatrix4("lightSpaceMatrix", lightSpaceMatrix);

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ResourceManager::GetTexture("rock").ID);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    for (BaseRenderObj *obj : renderObjects) {
        obj->renderScene(depthShader, true);
    }
//    glCullFace(GL_BACK);
    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 2. render scene as normal using the generated depth/shadow map
    // --------------------------------------------------------------
    glViewport(0, 0, context->windowW * 2, context->windowH * 2);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    worldShader.Use();
    glm::mat4 projection = glm::perspective(context->camera->Zoom, (float)context->windowW / (float)context->windowH, 0.1f, 100.0f);
    glm::mat4 view = context->camera->GetViewMatrix();
    worldShader.SetMatrix4("projection", projection);
    worldShader.SetMatrix4("view", view);
    // set light uniforms
    worldShader.SetVector3f("viewPos", context->camera->Position);
    worldShader.SetVector3f("lightPos", lightPos);
    worldShader.SetMatrix4("lightSpaceMatrix", lightSpaceMatrix);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ResourceManager::GetTexture("rock").ID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    for (BaseRenderObj *obj : renderObjects) {
        obj->renderScene(worldShader, false);
    }



    depthQuadShader.Use();
    depthQuadShader.SetFloat("near_plane", near_plane);
    depthQuadShader.SetFloat("far_plane", far_plane);
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, depthMap);

    if (this->hud)
        this->hud->renderScene(worldShader, false);


//    depthQuadShader.Use();
//
//    depthQuadShader.SetFloat("near_plane", near_plane);
//    depthQuadShader.SetFloat("far_plane", far_plane);
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, depthMap);

    // render Depth map to quad for visual debugging
    // ---------------------------------------------
//    debugDepthQuad.use();
//    debugDepthQuad.setFloat("near_plane", near_plane);
//    debugDepthQuad.setFloat("far_plane", far_plane);
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, depthMap);
//
//    for (BaseRenderObj *obj : renderObjects) {
//        obj->renderScene(worldShader, false);
//    }

    if (this->skybox)
        this->skybox->renderScene(worldShader, false);

//    if (this->player)
//        this->player->renderScene(worldShader, false);



    // We are done
    glfwSwapBuffers(context->window);
    glfwPollEvents();
}

/**
 * App Loop
 */
void Renderer::appLoop() {
    for (auto &renderObject : renderObjects) {
        if (renderObject->isFocused == true) {
            if (renderObject->checkKeys() == 1) {
                renderSingleObj(renderObject->id, renderObject->width, renderObject->height);
            }
            break;
        }
    }

    glfwPollEvents();
}

/**
 * start loop
 */
void Renderer::startLoop() {
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(_renderLoop, 0, 0);
#else
    /* Draw the Image on rendering surface */
    while (!glfwWindowShouldClose(context->window)) {
        _renderLoop();
    }
#endif
}

/**
 * stop loop
 */
void Renderer::stopLoop() {}

/**
 *
 * @param id
 * @param width
 * @param height
 * @return
 */
int Renderer::renderSingleObj(int id, int width, int height) {
    BaseRenderObj *renderObject = getRenderObjectById(id);

    if (!renderObject) {
        consoleLog("Error - RenderObject not found!");
        consoleLog(id);
        return -1;
    }

    this->setWindowSize(width, height);

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
BaseRenderObj *Renderer::getRenderObjectById(int id) {
    for (auto &renderObject : renderObjects) {
        if (renderObject->id == id) {
            return renderObject;
        }
    }

    return NULL;
}

/**
 * Add object to render list
 * @param renderObj
 * @return
 */
int Renderer::attachRenderObj(BaseRenderObj *renderObj) {
    renderObj->id = counter++;
    renderObjects.push_back(renderObj);
    renderObj->init();
    return renderObj->id;
}

/**
 * Attach camera
 * @param camera
 * @return
 */
int Renderer::attachCamera(Camera *camera) {
    context->camera = camera;
    return 0;
}

/**
 *
 * @param instance
 * @return
 */
int Renderer::attachSkybox(BaseRenderObj *instance) {
    instance->id = counter++;
    instance->init();
    this->skybox = instance;
    return 0;
}

/**
 *
 * @param instance
 * @return
 */
int Renderer::attachHud(BaseRenderObj *instance) {
    instance->id = counter++;
    instance->init();
    this->hud = instance;
    return 0;
}

/**
 *
 * @param instance
 * @return
 */
int Renderer::attachPlayer(BaseRenderObj *instance) {
    instance->id = counter++;
    instance->init();
    this->player = instance;
    this->attachRenderObj(instance);
    return 0;
}

int Renderer::initShadowMap() {
    // -----------------------
    Shader depthQuadShader = ResourceManager::LoadShader("assets/shaders/depthQuad.v.glsl", "assets/shaders/depthQuad.f.glsl",
    nullptr, "depthQuad");

    Shader depthShader = ResourceManager::LoadShader("assets/shaders/shadowMappingDepth.v.glsl",
                                                     "assets/shaders/shadowMappingDepth.f.glsl", nullptr,
                                                     "shadowMappingDepth");

    Shader worldShader = ResourceManager::LoadShader("assets/shaders/shadowMapping.v.glsl",
                                                     "assets/shaders/shadowMapping.f.glsl",
                                                     nullptr, "world");

//    ResourceManager::LoadShader("assets/shaders/model.v.glsl", "assets/shaders/model.f.glsl", nullptr,
//                                "object");

    // load textures
    // -----------------------
    ResourceManager::LoadTexture("assets/textures/grass.jpg", GL_FALSE, "grass");
    ResourceManager::LoadTexture("assets/textures/boulder.jpg", GL_FALSE, "rock");

    // configure depth map FBO
    // -----------------------
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // shader configuration
    // --------------------
    worldShader.Use();
    worldShader.SetInteger("diffuseTexture", 0);
    worldShader.SetInteger("shadowMap", 1);
    depthQuadShader.Use();
    depthQuadShader.SetInteger("depthMap", 0);

    return 0;
}

/**
 *
 * @return
 */
int Renderer::initSpriteHandler() {
    Shader shader = ResourceManager::LoadShader("assets/shaders/sprite.v.glsl", "assets/shaders/sprite.f.glsl", nullptr,
                                                "sprite");

    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(1920), static_cast<GLfloat>(1080), 0.0f, -1.0f, 1.0f);
    shader.Use().SetInteger("sprite", 0);
    shader.SetMatrix4("projection", projection);

    this->sprite = new SpriteRenderer(shader);

    return 0;
}

int Renderer::initWindow() {

    // Error callback is needed first
    glfwSetErrorCallback(_errorCallback);

    // Initialise GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    // glfwGetPrimaryMonitor()
    context->window = glfwCreateWindow(width, height, "Engine", NULL, NULL);

    if (context->window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. \n");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(context->window);

    glfwSetInputMode(context->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(context->window, _mouseCallback);
    glfwSetKeyCallback(context->window, _keyCallback);
    glfwSetScrollCallback(context->window, scroll_callback);
    glfwSetWindowSizeCallback(context->window, _windowSizeCallback);
    glfwSetWindowCloseCallback(context->window, _windowCloseCallback);
    glfwSetFramebufferSizeCallback(context->window, framebuffer_size_callback);

    glfwGetWindowSize(context->window, &context->windowW, &context->windowH);
    context->mouseLastX = context->windowW / 2.0f;
    context->mouseLastY = context->windowW / 2.0f;

    // Glew
    glewExperimental = true; // Needed for core profile on OS X
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0f, 0.0f, 0.0f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return 0;
};


void Renderer::mouseCallback(GLFWwindow *window, double xpos, double ypos) {
    if (context->firstMouse) {
        context->mouseLastX = xpos;
        context->mouseLastY = ypos;
        context->firstMouse = false;
    }

    double xOffset = xpos - context->mouseLastX;
    double yOffset = context->mouseLastY - ypos;
    context->mouseLastX = xpos;
    context->mouseLastY = ypos;

    if (context->camera == NULL)
        return;

    context->camera->ProcessMouseMovement(xOffset, yOffset);
}

void Renderer::scrollCallback(double xoffset, double yoffset) {
    if (context->camera == NULL)
        return;

    context->camera->ProcessMouseScroll(xoffset, yoffset);
}

void Renderer::windowSizeCallback(int width, int height) {
    context->windowW = width;
    context->windowH = height;
}

/*
 *
 *
 *
 * EVENT CALLBACKS
 *
 *
 * EVENT CALLBACKS
 *
 *
 * EVENT CALLBACKS
 *
 *
 * EVENT CALLBACKS
 *
 *
 *
 */
void _keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
//    renderer->keyCallback(window, key, scancode, action, mods);
}

void _mouseCallback(GLFWwindow *window, double xpos, double ypos) {
    renderer->mouseCallback(window, xpos, ypos);
}

void scroll_callback(GLFWwindow *window, double xOffset, double yOffet) {
    renderer->scrollCallback(xOffset, yOffet);
}

void _windowSizeCallback(GLFWwindow *window, int width, int height) {
    renderer->windowSizeCallback(width, height);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
//    glViewport(0, 0, width, height);
}


// glf

void _errorCallback(int error, const char *description) {
    consoleLog(error);
    consoleLog(description);
//    renderer->errorCallback(error, description);
}

void _windowCloseCallback(GLFWwindow *window) {
//    renderer->windowCloseCallback(window);
}

void _renderLoop() {
    renderer->tickLoop();
}