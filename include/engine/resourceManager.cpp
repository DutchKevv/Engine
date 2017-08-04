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

char *file_read(const char *filename) {
    FILE* in = fopen(filename, "rb");
    if (in == NULL) return NULL;

    int res_size = BUFSIZ;
    char* res = (char*)malloc(res_size);
    int nb_read_total = 0;

    while (!feof(in) && !ferror(in)) {
        if (nb_read_total + BUFSIZ > res_size) {
            if (res_size > 10*1024*1024) break;
            res_size = res_size * 2;
            res = (char*)realloc(res, res_size);
        }
        char* p_res = res + nb_read_total;
        nb_read_total += fread(p_res, 1, BUFSIZ, in);
    }

    fclose(in);
    res = (char*)realloc(res, nb_read_total + 1);
    res[nb_read_total] = '\0';
    return res;
}

GLuint create_shader(const char *filename, GLenum type) {
    const GLchar *source = file_read(filename);
    if (source == NULL) {
        string sFilename(filename);
        consoleLog("Error opening %s: " + sFilename);
        perror("");
        return 0;
    }
    GLuint res = glCreateShader(type);
    const GLchar *sources[] = {
            // Define GLSL version
#ifdef __EMSCRIPTEN__
            "#version 300 es\n"
#else
            "#version 330 core\n"
#endif
            ,
            // GLES2 precision specifiers
#ifdef __EMSCRIPTEN__
    // Define default float precision for fragment shaders:
    (type == GL_FRAGMENT_SHADER) ?
    "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
    "precision highp float;           \n"
    "#else                            \n"
    "precision mediump float;         \n"
    "#endif                           \n"
    : ""
    // Note: OpenGL ES automatically defines this:
    // #define GL_ES
#else
            // Ignore GLES 2 precision specifiers:
            "#define lowp   \n"
                    "#define mediump\n"
                    "#define highp  \n"
#endif
            ,
            source};
    glShaderSource(res, 3, sources, NULL);
    free((void *) source);

    glCompileShader(res);
    GLint compile_ok = GL_FALSE;
    glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);

    if (compile_ok == GL_FALSE) {
        fprintf(stderr, "%s:", filename);
//        print_log(res);
        glDeleteShader(res);
        return 0;
    }

    return res;
}

Shader ResourceManager::LoadShader(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile, std::string name)
{
    Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
    return Shaders[name];
}

Shader ResourceManager::GetShader(std::string name)
{
    Shader shader = Shaders[name];

    return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const GLchar *file, GLboolean alpha, std::string name)
{
    Textures[name] = loadTextureFromFile(file, alpha);
    return Textures[name];
}

Texture2D ResourceManager::GetTexture(std::string name)
{
    return Textures[name];
}

void ResourceManager::Clear()
{
    // (Properly) delete all shaders
    for (auto iter : Shaders)
        glDeleteProgram(iter.second.ID);
    // (Properly) delete all textures
    for (auto iter : Textures)
        glDeleteTextures(1, &iter.second.ID);
}

Shader ResourceManager::loadShaderFromFile(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile)
{
    consoleLog(vShaderFile);
    consoleLog(fShaderFile);

    GLuint program = glCreateProgram();
    GLuint shader;

    if (vShaderFile) {
        shader = create_shader(vShaderFile, GL_VERTEX_SHADER);
        if (shader)

        glAttachShader(program, shader);
    }

    if (fShaderFile) {
        shader = create_shader(fShaderFile, GL_FRAGMENT_SHADER);
        if (shader)
        glAttachShader(program, shader);
    }

    glLinkProgram(program);
    GLint link_ok = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
        consoleLog("PROGRAM LINK ERROR");
        glDeleteProgram(program);
    }

    Shader _shader;
    _shader.ID = program;
//    shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
    return _shader;
}

Texture2D ResourceManager::loadTextureFromFile(const GLchar *file, GLboolean alpha)
{
//    // Flip all textures on Y axis
//    stbi_set_flip_vertically_on_load(false);

    // Create Texture object
    Texture2D texture;
    if (alpha)
    {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }
    // Load image
    int width, height, nrChannels;
    unsigned char* image = stbi_load(file, &width, &height, &nrChannels, 0);

    if (!image) {
        consoleLog("Failed to load texture");
    }

    // Now generate texture
    texture.Generate(width, height, image);

    // And finally free image data
    stbi_image_free(image);

    return texture;
}