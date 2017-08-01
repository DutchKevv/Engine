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

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

Renderer *renderer;

using namespace std;

void _keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

void _mouseCallback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void _windowSizeCallback(GLFWwindow *window, int width, int height);

void _windowCloseCallback(GLFWwindow *window);

void _errorCallback(int error, const char *description);

void _renderLoop();


Renderer::Renderer(Context &context) : context(context) {
    renderer = this;
};

int Renderer::setWindowSize(int width, int height) {
    glfwSetWindowSize(context.window, width, height);
    return 0;
}

void Renderer::tickLoop() {
    if (context.type == 0) {
        this->gameLoop();
    } else {
        this->appLoop();
    }
}

/**
 * Game Loop
 */
void Renderer::gameLoop() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (context.camera) {
        context.camera->processInput(context.window);
    }

    for (BaseRenderObj *obj : renderObjects) {
        obj->draw();
    }

    glfwSwapBuffers(context.window);
    glfwPollEvents();
}

/**
 * App Loop
 */
void Renderer::appLoop() {
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
    while (!glfwWindowShouldClose(context.window)) {
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

    renderObject->draw();

    glfwSwapBuffers(context.window);
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
    renderObj->init(context);
    return renderObj->id;
}

/**
 * Attach camera
 * @param camera
 * @return
 */
int Renderer::attachCamera(Camera *camera) {
    context.camera = camera;
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
    context.window = glfwCreateWindow(1920, 1080, "Engine", NULL, NULL);

    if (context.window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. \n");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(context.window);
    glfwSetInputMode(context.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(context.window, _mouseCallback);
    glfwSetKeyCallback(context.window, _keyCallback);
    glfwSetScrollCallback(context.window, scroll_callback);
    glfwSetWindowSizeCallback(context.window, _windowSizeCallback);
    glfwSetWindowCloseCallback(context.window, _windowCloseCallback);

    glfwGetWindowSize(context.window, &context.windowW, &context.windowH);
    context.mouseLastX = context.windowW / 2.0f;
    context.mouseLastY = context.windowW / 2.0f;

    consoleLog(context.windowW);
    consoleLog(context.windowH);

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

    // Flip all textures on Y axis
    stbi_set_flip_vertically_on_load(true);

    return 0;
};


void Renderer::mouseCallback(GLFWwindow *window, double xpos, double ypos) {
    if (context.firstMouse) {
        context.mouseLastX = xpos;
        context.mouseLastY = ypos;
        context.firstMouse = false;
    }

    double xOffset = xpos - context.mouseLastX;
    double yOffset = context.mouseLastY - ypos;
    context.mouseLastX = xpos;
    context.mouseLastY = ypos;

    if (!context.camera)
        return;

    context.camera->ProcessMouseMovement(xOffset, yOffset);
}

void Renderer::scrollCallback(double xoffset, double yoffset) {
    if (!context.camera)
        return;

    context.camera->ProcessMouseScroll(xoffset, yoffset);
}

void Renderer::windowSizeCallback(int width, int height) {
    context.windowW = width;
    context.windowH = height;
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