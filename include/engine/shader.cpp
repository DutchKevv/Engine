#include "shader.h"
#include "logger.h"
#include "GL/glew.h"

#include <iostream>

Shader &Shader::Use() {
    glUseProgram(this->ID);
    return *this;
}

void Shader::Compile(const GLchar *vertexSource, const GLchar *fragmentSource, const GLchar *geometrySource) {
    GLuint sVertex, sFragment, gShader;
    // Vertex Shader
    sVertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sVertex, 1, &vertexSource, NULL);
    glCompileShader(sVertex);
    checkCompileErrors(sVertex, "VERTEX", vertexSource);
    // Fragment Shader
    sFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(sFragment, 1, &fragmentSource, NULL);
    glCompileShader(sFragment);
    checkCompileErrors(sFragment, "FRAGMENT", fragmentSource);
    // If geometry shader source code is given, also compile geometry shader
    if (geometrySource != nullptr) {
        gShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(gShader, 1, &geometrySource, NULL);
        glCompileShader(gShader);
        checkCompileErrors(gShader, "GEOMETRY", geometrySource);
    }
    // Shader Program
    this->ID = glCreateProgram();
    glAttachShader(this->ID, sVertex);
    glAttachShader(this->ID, sFragment);
    if (geometrySource != nullptr)
        glAttachShader(this->ID, gShader);
    glLinkProgram(this->ID);
    checkCompileErrors(this->ID, "PROGRAM", "PROGRAM");
    // Delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(sVertex);
    glDeleteShader(sFragment);
    if (geometrySource != nullptr)
        glDeleteShader(gShader);
}

void Shader::SetFloat(const GLchar *name, GLfloat value, GLboolean useShader) {
    glUniform1f(glGetUniformLocation(this->ID, name), value);
}

void Shader::SetInteger(const GLchar *name, GLint value, GLboolean useShader) {
    glUniform1i(getUniformPos(name), value);
}

void Shader::SetVector2f(const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader) {
    glUniform2f(getUniformPos(name), x, y);
}

void Shader::SetVector2f(const GLchar *name, const glm::vec2 &value, GLboolean useShader) {
    glUniform2f(getUniformPos(name), value.x, value.y);
}

void Shader::SetVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader) {
    glUniform3f(getUniformPos(name), x, y, z);
}

void Shader::SetVector3f(const GLchar *name, const glm::vec3 &value, GLboolean useShader) {
    glUniform3f(getUniformPos(name), value.x, value.y, value.z);
}

void Shader::SetVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader) {
    glUniform4f(getUniformPos(name), x, y, z, w);
}

void Shader::SetVector4f(const GLchar *name, const glm::vec4 &value, GLboolean useShader) {
    glUniform4f(getUniformPos(name), value.x, value.y, value.z, value.w);
}

void Shader::SetMatrix4(const GLchar *name, const glm::mat4 &matrix, GLboolean useShader) {
    glUniformMatrix4fv(getUniformPos(name), 1, GL_FALSE, glm::value_ptr(matrix));
}


void Shader::checkCompileErrors(GLuint object, std::string type, const GLchar *name) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(object, 1024, NULL, infoLog);
            consoleLog(name);
            consoleLog(infoLog);

            std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- "
                      << name << "\n -- --------------------------------------------------- -- "
                      << std::endl;
        }
    } else {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(object, 1024, NULL, infoLog);
            consoleLog(name);
            consoleLog(infoLog);

            std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- "
                      << name << "\n -- --------------------------------------------------- -- "
                      << std::endl;
        }
    }
}

GLint Shader::getAttributePos(const char *name) {
    GLint attribute = glGetAttribLocation(ID, name);
    if (attribute == -1) {
        string str(name);
        consoleLog("Could not bind attribute " + str);
    }

    return attribute;
}

GLint Shader::getLayoutPos(const char *name) {
    return getAttributePos(name);
}

GLint Shader::getUniformPos(const char *name) {
    GLint uniform = glGetUniformLocation(ID, name);
    if (uniform == -1) {
        string str(name);
        consoleLog("Could not bind uniform " + str);
    }
    return uniform;
}