#pragma once

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
    Shader(const char *vertexShaderFilename, const char *fragmentShaderFilename);
    ~Shader();

    void bind();
    void unbind();

    void createUniform(const std::string name);
    void loadMatrix4fUniform(glm::mat4 matrix, const std::string name);
    void loadVector2fUniform(glm::vec2 vector, const std::string name);
    void loadVector3fUniform(glm::vec3 vector, const std::string name);
    void loadVector4fUniform(glm::vec4 vector, const std::string name);
    void loadFloatUniform(float value, const std::string name);
    void loadIntUniform(int value, const std::string name);

    GLuint getShaderID() const { return shaderID; }

private:
    GLuint compile(std::string shaderSource, GLenum type);
    std::string parse(const char *filename);
    GLuint createShader(const char *vertexShaderFilename, const char *fragmentShaderFilename);

    GLuint shaderID;
    std::map<const std::string, GLuint> uniforms;
};