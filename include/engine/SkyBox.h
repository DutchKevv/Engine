//
// Created by Kewin Brandsma on 17/07/2017.
//

#pragma once;

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "camera.h"
//#include "Camera.h"
//#include "common/Shader.h"

class SkyBox {
private:
    GLFWwindow *window;
    Camera *camera;
    Shader *shader;

    int setup();
public:
    SkyBox(GLFWwindow *window, Camera *camera);
    int render();
    int move();
    int destroy();
};


#endif //BACKSPACE_SKYBOX_H
