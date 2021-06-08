#pragma once

#include <glm/glm.hpp>
#include <SDL2/SDL.h>

class Scene
{
public:
    Scene() {}
    ~Scene() {}

    virtual void init() {}
    virtual void handleEvents(const SDL_Event &event) {}
    virtual void update(const double dt) {}
    virtual void refresh() {}
    virtual void reset() {}
    virtual void render() {}

    virtual const void setJuliaPoint(const glm::highp_vec2 &point) {}
    virtual const glm::highp_vec2 &getJuliaPoint() { return juliaPoint; }

    virtual const void setExponent(const glm::highp_vec2 &point) {}
    virtual const glm::highp_vec2 &getExponent() { return exponent; }

    virtual const bool &isSwapRequested() { return swapRequested; }

private:
    glm::highp_vec2 juliaPoint;
    glm::highp_vec2 exponent;
    bool swapRequested;
};