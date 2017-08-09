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
#include "renderer.h"
#include "baseRenderObj.h"
#include "engine.h"
#include "context.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

unsigned int width = 1920;
unsigned int height = 1080;

using namespace std;

void _keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

void _mouseCallback(GLFWwindow *window, double xpos, double ypos);

void _scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void _windowSizeCallback(GLFWwindow *window, int width, int height);

void _windowCloseCallback(GLFWwindow *window);

void _errorCallback(int error, const char *description);

void _framebuffer_size_callback(GLFWwindow *window, int width, int height);

void _renderLoop();

Renderer::Renderer() {

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

int Renderer::attachWorld(World *world) {
    world->init();
    this->worlds.push_back(world);
    return 0;
}

/**
 * Game Loop
 */
void Renderer::gameLoop() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    World *world = worlds[0];

    for (BaseRenderObj *obj : world->renderObjects) {
        obj->update();
    }

    world->draw();

    // We are done
    glfwSwapBuffers(context->window);
    glfwPollEvents();
}

/**
 * App Loop
 */
void Renderer::appLoop() {
    World *world = worlds[0];
    for (auto &renderObject : world->renderObjects) {
        if (renderObject->focused == true) {
            if (renderObject->checkKeys() == 1) {
                world->renderSingleObj(renderObject->id, renderObject->width, renderObject->height);
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
 * @return
 */
int Renderer::initSpriteHandler() {
    Shader shader = ResourceManager::LoadShader("assets/shaders/sprite.v.glsl", "assets/shaders/sprite.f.glsl", nullptr,
                                                "sprite");

    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(context->windowW), static_cast<GLfloat>(context->windowW), 0.0f, -1.0f, 1.0f);
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
    context->window = glfwCreateWindow(width, height, "Engine", NULL, NULL);
//    context->window = glfwCreateWindow(width, height, "Engine", glfwGetPrimaryMonitor(), NULL);

    if (context->window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. \n");
        getchar();
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(context->window);

    if (context->type == ENGINE_TYPE_GAME) {
        glfwSetInputMode(context->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(context->window, _mouseCallback);
        glfwSetKeyCallback(context->window, _keyCallback);
        glfwSetScrollCallback(context->window, _scroll_callback);
    }

    glfwSetWindowSizeCallback(context->window, _windowSizeCallback);
    glfwSetWindowCloseCallback(context->window, _windowCloseCallback);
    glfwSetFramebufferSizeCallback(context->window, _framebuffer_size_callback);

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
    context->renderer->mouseCallback(window, xpos, ypos);
}

void _scroll_callback(GLFWwindow *window, double xOffset, double yOffet) {
    context->renderer->scrollCallback(xOffset, yOffet);
}

void _windowSizeCallback(GLFWwindow *window, int width, int height) {
    context->renderer->windowSizeCallback(width, height);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void _framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
//    glViewport(0, 0, width, height);
}

void _errorCallback(int error, const char *description) {
    consoleLog(error);
    consoleLog(description);
//    renderer->errorCallback(error, description);
}

void _windowCloseCallback(GLFWwindow *window) {
//    renderer->windowCloseCallback(window);
}

void _renderLoop() {
    context->renderer->tickLoop();
}