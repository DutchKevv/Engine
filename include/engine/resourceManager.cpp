/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "resourceManager.h"
#include "../../extern/stb_image.h"
#include "logger.h"

#include <iostream>
#include <sstream>
#include <fstream>

// Instantiate static variables
std::map<std::string, Texture2D>    ResourceManager::Textures;
std::map<std::string, Shader>       ResourceManager::Shaders;


Shader ResourceManager::LoadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile, std::string name) {
    Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
    return Shaders[name];
}

Shader ResourceManager::GetShader(std::string name) {
    Shader shader = Shaders[name];

    return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const GLchar *file, GLboolean alpha, std::string name) {
    Textures[name] = loadTextureFromFile(file, alpha);
    return Textures[name];
}

Texture2D ResourceManager::GetTexture(std::string name) {
    return Textures[name];
}

void ResourceManager::Clear() {
    // (Properly) delete all shaders
    for (auto iter : Shaders)
        glDeleteProgram(iter.second.ID);
    // (Properly) delete all textures
    for (auto iter : Textures)
        glDeleteTextures(1, &iter.second.ID);
}

char *file_read(const char *filename) {
    FILE *in = fopen(filename, "rb");
    if (in == NULL) return NULL;

    int res_size = BUFSIZ;
    char *res = (char *) malloc(res_size);
    int nb_read_total = 0;

    while (!feof(in) && !ferror(in)) {
        if (nb_read_total + BUFSIZ > res_size) {
            if (res_size > 10 * 1024 * 1024) break;
            res_size = res_size * 2;
            res = (char *) realloc(res, res_size);
        }
        char *p_res = res + nb_read_total;
        nb_read_total += fread(p_res, 1, BUFSIZ, in);
    }

    fclose(in);
    res = (char *) realloc(res, nb_read_total + 1);
    res[nb_read_total] = '\0';
    return res;
}

Shader ResourceManager::loadShaderFromFile(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile) {
    // 1. Retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    try {
        // Open files
        std::ifstream vertexShaderFile(vShaderFile);
        std::ifstream fragmentShaderFile(fShaderFile);
        std::stringstream vShaderStream, fShaderStream;
        // Read file's buffer contents into streams
        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();
        // close file handlers
        vertexShaderFile.close();
        fragmentShaderFile.close();
        // Convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        // If geometry shader path is present, also load a geometry shader
        if (gShaderFile != nullptr) {
            std::ifstream geometryShaderFile(gShaderFile);
            std::stringstream gShaderStream;
            gShaderStream << geometryShaderFile.rdbuf();
            geometryShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }
    catch (std::exception e) {
        consoleLog("ERROR::SHADER: Failed to read shader files");
    }

#ifdef __EMSCRIPTEN__
    vertexCode = "#version 300 es \n" + vertexCode;
    fragmentCode = "#version 300 es \n precision mediump float;\n" + fragmentCode;

//#ifdef GL_FRAGMENT_PRECISION_HIGH
//    fragmentCode += "precision highp float;\n";
//#else
//    fragmentCode += "precision mediump float;\n";
//#endif

#else
    vertexCode = "#version 330 core\n" + vertexCode;
    fragmentCode = "#version 330 core\n" + fragmentCode;
#endif

    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();
    const char *gShaderCode = geometryCode.c_str();
    // 2. Now create shader object from source code

    Shader shader;
    shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
    return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const GLchar *file, GLboolean alpha) {
//    // Flip all textures on Y axis
//    stbi_set_flip_vertically_on_load(false);

    // Create Texture object
    Texture2D texture;
    if (alpha) {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }
    // Load image
    int width, height, nrChannels;
    unsigned char *image = stbi_load(file, &width, &height, &nrChannels, 0);

    if (!image) {
        consoleLog("Failed to load texture");
    }

    // Now generate texture
    texture.Generate(width, height, image);

    // And finally free image data
    stbi_image_free(image);

    return texture;
}