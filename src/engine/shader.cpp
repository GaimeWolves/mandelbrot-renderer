#include "shader.hpp"

Shader::Shader(const char *vertexShaderFilename, const char *fragmentShaderFilename)
{
    shaderID = createShader(vertexShaderFilename, fragmentShaderFilename);
}

Shader::~Shader()
{
    glDeleteProgram(shaderID);
}

void Shader::bind()
{
    glUseProgram(shaderID);
}

void Shader::unbind()
{
    glUseProgram(0);
}

void Shader::createUniform(const std::string name)
{
    GLuint loc = glGetUniformLocation(shaderID, name.c_str());

    if (loc == -1)
    {
        std::cout << "Uniform " << name << " not found" << std::endl;
        return;
    }

    uniforms[name] = loc;
}

void Shader::loadMatrix4fUniform(glm::mat4 matrix, const std::string name)
{
    glUniformMatrix4fv(uniforms[name], 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::loadVector2fUniform(glm::vec2 vector, const std::string name)
{
    glUniform2fv(uniforms[name], 1, &vector[0]);
}

void Shader::loadVector3fUniform(glm::vec3 vector, const std::string name)
{
    glUniform3fv(uniforms[name], 1, &vector[0]);
}

void Shader::loadVector4fUniform(glm::vec4 vector, const std::string name)
{
    glUniform4fv(uniforms[name], 1, &vector[0]);
}

void Shader::loadFloatUniform(float value, const std::string name)
{
    glUniform1fv(uniforms[name], 1, &value);
}

void Shader::loadIntUniform(int value, const std::string name)
{
    glUniform1iv(uniforms[name], 1, &value);
}

GLuint Shader::compile(std::string shaderSource, GLenum type)
{
    GLuint id = glCreateShader(type);
    const char *src = shaderSource.c_str();
    glShaderSource(id, 1, &src, 0);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result != GL_TRUE)
    {
        int length = 0;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char *message = new char[length];
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Shader compilation error: " << message << std::endl;
        delete[] message;
        return 0;
    }
    return id;
}

std::string Shader::parse(const char *filename)
{
    FILE *file;

#ifdef _WIN32
    if (fopen_s(&file, filename, "rb"))
    {
        std::cout << "File " << filename << " not found" << std::endl;
        return "";
    }
#else
    file = fopen(filename, "rb");
    if (file == nullptr)
    {
        std::cout << "File " << filename << " not found" << std::endl;
        return "";
    }
#endif

    std::string contents;
    fseek(file, 0, SEEK_END);
    size_t filesize = ftell(file);
    rewind(file);
    contents.resize(filesize);

    fread(&contents[0], 1, filesize, file);
    fclose(file);

    return contents;
}

GLuint Shader::createShader(const char *vertexShaderFilename, const char *fragmentShaderFilename)
{
    std::string vertexShaderSource = parse(vertexShaderFilename);
    std::string fragmentShaderSource = parse(fragmentShaderFilename);

    GLuint program = glCreateProgram();
    GLuint vs = compile(vertexShaderSource, GL_VERTEX_SHADER);
    std::cout << "Vertex shader " << vertexShaderFilename << " compiled" << std::endl;

    GLuint fs = compile(fragmentShaderSource, GL_FRAGMENT_SHADER);
    std::cout << "Fragment shader " << fragmentShaderFilename << " compiled" << std::endl;

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    std::cout << "Shader linked" << std::endl;

    glDetachShader(program, vs);
    glDetachShader(program, fs);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}