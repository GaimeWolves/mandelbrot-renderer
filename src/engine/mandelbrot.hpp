#pragma once

#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include "scene.hpp"

class MandelbrotScene : public Scene
{
public:
    MandelbrotScene();
    ~MandelbrotScene();

    void init();
    void handleEvents(const SDL_Event &event);
    void update(const double dt);
    void refresh();
    void reset();
    void render();

    const void setJuliaPoint(const glm::highp_vec2 &point)
    {
        juliaPoint.x = point.x;
        juliaPoint.y = point.y;
    }

    const glm::highp_vec2 &getJuliaPoint() { return juliaPoint; }

    const glm::highp_vec2 &getExponent() { return exponent; }

    const void setExponent(const glm::highp_vec2 &exponent)
    {
        this->exponent.x = exponent.x;
        this->exponent.y = exponent.y;
    }

    const bool &isSwapRequested() { return swapRequested; }

private:
    glm::highp_vec2 juliaPoint = glm::highp_vec2(0.0f, 0.0f);
    glm::highp_vec2 exponent = glm::highp_vec2(2.0f, 0.0f);
    bool swapRequested = false;

    void updateMandelbrotLine();
};