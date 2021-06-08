#include "util.hpp"

#include <glm/glm.hpp>
#include <cmath>

#include "globals.hpp"

glm::highp_vec2 map(glm::highp_vec2 value, glm::highp_vec2 inMin, glm::highp_vec2 inMax, glm::highp_vec2 outMin, glm::highp_vec2 outMax)
{
    return outMin + (outMax - outMin) * (value - inMin) / (inMax - inMin);
}

glm::highp_vec2 fromWorldToScreen(glm::highp_vec2 worldPos, glm::highp_vec4 worldBounds)
{
    return map(
        worldPos,
        glm::highp_vec2(worldBounds.x, worldBounds.y),
        glm::highp_vec2(worldBounds.z, worldBounds.w),
        glm::highp_vec2(-1.0f, -1.0f),
        glm::highp_vec2(1.0f, 1.0f));
}

glm::highp_vec2 fromPixelToWorld(glm::highp_vec2 pixelPos, glm::highp_vec4 worldBounds)
{
    pixelPos.y = screenSize.y - pixelPos.y;
    return map(
        pixelPos,
        glm::highp_vec2(0, 0),
        glm::highp_vec2(screenSize.x, screenSize.y),
        glm::highp_vec2(worldBounds.x, worldBounds.y),
        glm::highp_vec2(worldBounds.z, worldBounds.w));
}

glm::highp_vec2 fromPixelToScreen(glm::highp_vec2 pixelPos)
{
    return map(
        pixelPos,
        glm::highp_vec2(0, 0),
        glm::highp_vec2(screenSize.x, screenSize.y),
        glm::highp_vec2(-1.0f, 1.0f),
        glm::highp_vec2(1.0f, -1.0f));
}

glm::highp_vec2 transform(glm::highp_vec2 point, glm::highp_vec4 inBounds, glm::highp_vec4 outBounds)
{
    return map(
        point,
        glm::highp_vec2(inBounds.x, inBounds.y),
        glm::highp_vec2(inBounds.z, inBounds.w),
        glm::highp_vec2(outBounds.x, outBounds.y),
        glm::highp_vec2(outBounds.z, outBounds.w));
}

bool isPointInRectangle(glm::highp_vec2 point, glm::highp_vec4 aabb)
{
    return point.x >= aabb.x && point.x <= aabb.z && point.y >= aabb.y && point.y <= aabb.w;
}

static glm::highp_vec2 toPolar(glm::highp_vec2 c)
{
    float theta = std::atan2(c.y, c.x);
    float r = std::sqrt(c.x * c.x + c.y * c.y);

    return glm::highp_vec2(r, theta);
}

glm::highp_vec2 complexExp(glm::highp_vec2 w, glm::highp_vec2 z)
{
    if (w.x == 0 && w.y == 0)
        return w;

    w = toPolar(w);

    float r = w.x;
    float theta = w.y;
    float c = z.x;
    float d = z.y;

    float scalar = std::pow(r, c) * std::exp(-d * theta);
    float angle = d * std::log(r) + c * theta;
    float re = std::cos(angle);
    float im = std::sin(angle);

    return glm::highp_vec2(scalar * re, scalar * im);
}