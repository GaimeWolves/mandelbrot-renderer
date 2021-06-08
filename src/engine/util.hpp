#pragma once

#include <glm/glm.hpp>

glm::highp_vec2 fromWorldToScreen(glm::highp_vec2 worldPos, glm::highp_vec4 worldBounds);
glm::highp_vec2 fromPixelToWorld(glm::highp_vec2 pixelPos, glm::highp_vec4 worldBounds);
glm::highp_vec2 fromPixelToScreen(glm::highp_vec2 pixelPos);

glm::highp_vec2 transform(glm::highp_vec2 point, glm::highp_vec4 inBounds, glm::highp_vec4 outBounds);

bool isPointInRectangle(glm::highp_vec2 point, glm::highp_vec4 aabb);

glm::highp_vec2 complexExp(glm::highp_vec2 w, glm::highp_vec2 z);