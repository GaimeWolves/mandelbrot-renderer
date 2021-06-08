#include "juliaset.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include <vector>

#include "globals.hpp"
#include "util.hpp"

static const GLfloat juliasetVertices[] = {
    -1, -1,
    -1, 1,
    1, -1,
    1, 1};

static const GLfloat mandelbrotVertices[] = {
    0.4f, 0.4f,
    0.4f, 0.95f,
    0.95f, 0.4f,
    0.95f, 0.95f};

static const GLfloat mandelbrotBounds[] = {
    -2, -2,
    -2, 2,
    2, -2,
    2, 2};

static const glm::highp_vec4 mandelbrotAABB(mandelbrotVertices[0], mandelbrotVertices[1], mandelbrotVertices[6], mandelbrotVertices[7]);

static const int MaxIterations = 4096;

static GLuint juliasetVAO;
static GLuint juliasetPositionVBO;
static GLuint juliasetBoundsVBO;

static GLuint mandelbrotVAO;
static GLuint mandelbrotPositionVBO;
static GLuint mandelbrotBoundsVBO;

static GLuint lineVAO;
static GLuint lineVBO;

static glm::highp_vec2 linePoint(0.0f, 0.0f);
static int lineVertexCount;

static glm::highp_vec2 origin(0.0f, 0.0f);
static glm::highp_vec4 bounds(-2.0, -2.0, 2.0, 2.0);
static float worldWidth = 4.0f;
static float worldScale = worldWidth / screenSize.x;

static bool mouseDown;
static bool pointGrabbed;
static glm::highp_vec2 lastMousePos(0.0f, 0.0f);

void JuliasetScene::updateJuliasetLine()
{
    std::vector<glm::highp_vec2> vertices = std::vector<glm::highp_vec2>();
    vertices.push_back(fromWorldToScreen(linePoint, bounds));

    glm::highp_vec2 z(linePoint);
    glm::highp_vec2 c(juliaPoint);

    for (int i = 0; i < MaxIterations; i++)
    {
        if (z.x * z.x + z.y * z.y >= 4)
            break;

        auto exp = complexExp(z, exponent);

        z.x = exp.x + c.x;
        z.y = exp.y + c.y;

        vertices.push_back(fromWorldToScreen(z, bounds));
    }

    lineVertexCount = vertices.size();

    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, lineVertexCount * sizeof(linePoint), &vertices[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

JuliasetScene::JuliasetScene()
{
}

JuliasetScene::~JuliasetScene()
{
}

void JuliasetScene::init()
{
    glGenVertexArrays(1, &juliasetVAO);
    glBindVertexArray(juliasetVAO);

    glGenBuffers(1, &juliasetPositionVBO);
    glBindBuffer(GL_ARRAY_BUFFER, juliasetPositionVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(juliasetVertices), juliasetVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &juliasetBoundsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, juliasetBoundsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mandelbrotBounds), mandelbrotBounds, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    glGenVertexArrays(1, &mandelbrotVAO);
    glBindVertexArray(mandelbrotVAO);

    glGenBuffers(1, &mandelbrotPositionVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mandelbrotPositionVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mandelbrotVertices), mandelbrotVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &mandelbrotBoundsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mandelbrotBoundsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mandelbrotBounds), mandelbrotBounds, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    glGenVertexArrays(1, &lineVAO);
    glBindVertexArray(lineVBO);
    glGenBuffers(1, &lineVBO);
    glBindVertexArray(0);
}

void JuliasetScene::handleEvents(const SDL_Event &event)
{
    switch (event.type)
    {
    case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            lastMousePos = glm::highp_vec2(event.button.x, event.button.y);
            auto realPosition = fromPixelToWorld(lastMousePos, bounds);
            auto screenPos = fromPixelToScreen(lastMousePos);

            if (isPointInRectangle(screenPos, mandelbrotAABB))
            {
                swapRequested = true;
                return;
            }

            mouseDown = true;

            pointGrabbed = glm::distance(realPosition, linePoint) < worldWidth / screenSize.x * 5;
        }
        break;
    case SDL_MOUSEBUTTONUP:
        if (event.button.button == SDL_BUTTON_LEFT)
            mouseDown = false;
        break;
    case SDL_MOUSEMOTION:
        if (mouseDown)
        {
            glm::highp_vec2 currentMousePos(event.motion.x, event.motion.y);
            glm::highp_vec2 delta = (lastMousePos - currentMousePos) * worldScale;
            lastMousePos = currentMousePos;

            if (pointGrabbed)
            {
                linePoint.x -= delta.x;
                linePoint.y += delta.y;
            }
            else
            {
                origin.x += delta.x;
                origin.y -= delta.y;
            }

            refresh();
        }
        break;
    case SDL_MOUSEWHEEL:
        if (event.wheel.y > 0)
            worldWidth *= 0.9f;
        else
            worldWidth *= 1.1f;

        refresh();
        break;
    case SDL_KEYDOWN:
        switch (event.key.keysym.sym)
        {
        case SDL_KeyCode::SDLK_LEFT:
            exponent.x -= 0.1;
            break;
        case SDL_KeyCode::SDLK_RIGHT:
            exponent.x += 0.1;
            break;
        case SDL_KeyCode::SDLK_UP:
            exponent.y += 0.1;
            break;
        case SDL_KeyCode::SDLK_DOWN:
            exponent.y -= 0.1;
            break;
        }

    default:
        break;
    }
}

void JuliasetScene::update(const double dt)
{
}

void JuliasetScene::refresh()
{
    worldScale = worldWidth / screenSize.x;

    bounds.x = origin.x - worldWidth / 2.0f;
    bounds.y = origin.y - worldWidth / 2.0f;
    bounds.z = origin.x + worldWidth / 2.0f;
    bounds.w = origin.y + worldWidth / 2.0f;

    float boundsData[] = {
        bounds.x, bounds.y,
        bounds.x, bounds.w,
        bounds.z, bounds.y,
        bounds.z, bounds.w};

    glBindVertexArray(juliasetVAO);
    glBindBuffer(GL_ARRAY_BUFFER, juliasetBoundsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boundsData), boundsData, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    updateJuliasetLine();
}

void JuliasetScene::reset()
{
    origin = glm::highp_vec2(0.0f, 0.0f);
    worldWidth = 4.0f;
    swapRequested = false;

    refresh();
}

void JuliasetScene::render()
{
    juliasetShader->bind();

    juliasetShader->loadVector2fUniform(juliaPoint, "point");
    juliasetShader->loadVector2fUniform(exponent, "exponent");

    glBindVertexArray(juliasetVAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, juliasetPositionVBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, juliasetBoundsVBO);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);

    juliasetShader->unbind();

    glBindVertexArray(lineVAO);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glDrawArrays(GL_LINE_STRIP, 0, lineVertexCount);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);

    auto screenPos = fromWorldToScreen(linePoint, bounds);
    glPointSize(7.0f);
    glBegin(GL_POINTS);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(screenPos.x, screenPos.y);
    glEnd();

    glPointSize(5.0f);
    glBegin(GL_POINTS);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex2f(screenPos.x, screenPos.y);
    glColor3f(1.0f, 1.0f, 1.0f);
    glEnd();

    mandelbrotShader->bind();

    mandelbrotShader->loadVector2fUniform(exponent, "exponent");

    glBindVertexArray(mandelbrotVAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, mandelbrotPositionVBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, mandelbrotBoundsVBO);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);

    mandelbrotShader->unbind();

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2.0f);
    glBegin(GL_QUAD_STRIP);
    glColor3f(0.3f, 0.3f, 0.3f);
    for (int i = 0; i < 4; i++)
        glVertex2f(mandelbrotVertices[i * 2], mandelbrotVertices[i * 2 + 1]);
    glColor3f(1.0f, 1.0f, 1.0f);
    glEnd();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    auto mandelbrotPos = transform(juliaPoint, glm::highp_vec4(-2.0f, -2.0f, 2.0f, 2.0f), mandelbrotAABB);
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(mandelbrotPos.x, mandelbrotPos.y);
    glEnd();

    glPointSize(3.0f);
    glBegin(GL_POINTS);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex2f(mandelbrotPos.x, mandelbrotPos.y);
    glColor3f(1.0f, 1.0f, 1.0f);
    glEnd();
}