#include <iostream>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <glm/gtc/matrix_transform.hpp>

#define _USE_MATH_DEFINES
#include <cmath>

#include <vector>
#include <memory>
#include <filesystem>

#include "engine/shader.hpp"
#include "engine/globals.hpp"

#include "engine/scene.hpp"
#include "engine/mandelbrot.hpp"
#include "engine/juliaset.hpp"

static std::string execPath;

static SDL_Window *window;

static const int width = 800;
static const int height = 800;

static uint64_t NOW = 0;
static uint64_t LAST = 0;
static double deltaTime = 0;

Shader *mandelbrotShader = nullptr;
Shader *juliasetShader = nullptr;

const glm::highp_vec2 screenSize((float)width, (float)height);

static int currentScene = 0;
static std::unique_ptr<Scene> scenes[] = {
    std::make_unique<MandelbrotScene>(),
    std::make_unique<JuliasetScene>(),
};

static void openGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    std::cout << "[OpenGL Error] " << message << std::endl;
}

static void init()
{
    NOW = SDL_GetPerformanceCounter();

    SDL_WarpMouseInWindow(window, width / 2, height / 2);

    mandelbrotShader = new Shader((execPath + "shaders/mandelbrot.vert").c_str(), (execPath + "shaders/mandelbrot.frag").c_str());
    juliasetShader = new Shader((execPath + "shaders/juliaset.vert").c_str(), (execPath + "shaders/juliaset.frag").c_str());

    mandelbrotShader->createUniform("exponent");

    mandelbrotShader->bind();
    mandelbrotShader->loadVector2fUniform(glm::highp_vec2(4.25f, -3.0f), "exponent");
    mandelbrotShader->unbind();

    juliasetShader->createUniform("point");
    juliasetShader->createUniform("exponent");

    juliasetShader->bind();
    juliasetShader->loadVector2fUniform(glm::highp_vec2(), "point");
    juliasetShader->loadVector2fUniform(glm::highp_vec2(4.25f, -3.0f), "exponent");
    juliasetShader->unbind();

    for (auto &&scene : scenes)
    {
        scene->init();
        scene->refresh();
    }
}

static void handleEvents(SDL_Event event)
{
    scenes[currentScene]->handleEvents(event);
}

static void update(double dt)
{
    scenes[currentScene]->update(dt);

    if (scenes[currentScene]->isSwapRequested())
    {
        scenes[currentScene]->reset();
        auto point = scenes[currentScene]->getJuliaPoint();
        auto exponent = scenes[currentScene]->getExponent();
        currentScene = (currentScene + 1) % (sizeof(scenes) / sizeof(std::unique_ptr<Scene>));
        scenes[currentScene]->setJuliaPoint(point);
        scenes[currentScene]->setExponent(exponent);
        scenes[currentScene]->reset();
    }
}

static void render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    scenes[currentScene]->render();
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    window = SDL_CreateWindow("Mandelbrot", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cout << "Error: " << glewGetErrorString(err) << std::endl;
        std::cin.get();
        return -1;
    }
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    //glEnable(GL_DEBUG_OUTPUT);
    //glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    //glDebugMessageCallback(openGLDebugCallback, 0);

    execPath = std::filesystem::path(std::string(argv[0])).remove_filename().string();

    init();

    bool close = false;

    while (!close)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
            {
                close = true;
            }

            handleEvents(event);
        }

        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = (double)((NOW - LAST) / (double)SDL_GetPerformanceFrequency());

        SDL_SetWindowTitle(window, (std::to_string(1 / deltaTime) + " FPS").c_str());

        update(deltaTime);
        render();

        SDL_GL_SwapWindow(window);
    }

    return 0;
}