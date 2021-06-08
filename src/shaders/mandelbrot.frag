#version 330 core

#define PI 3.1415926535897932384626433832795

precision highp float;

in vec4 gl_FragCoord;
in vec2 coords;

uniform vec2 exponent;

layout(location = 0) out vec4 fragColor;

const float maxIterations = 1028;

vec2 toPolar(vec2 complex)
{
    float theta = atan(complex.y, complex.x);
    float r = sqrt(complex.x * complex.x + complex.y * complex.y);

    return vec2(r, theta);
}

vec2 complexExp(vec2 w, vec2 z)
{
    if (w.x == 0 && w.y == 0)
        return w;

    float r = w.x;
    float theta = w.y;
    float c = z.x;
    float d = z.y;

    float scalar = pow(r, c) * exp(-d * theta);
    float angle = d * log(r) + c * theta;
    float re = cos(angle);
    float im = sin(angle);

    return vec2(scalar * re, scalar * im);
}

vec2 map(vec2 value, vec2 inMin, vec2 inMax, vec2 outMin, vec2 outMax)
{
    return outMin + (outMax - outMin) * (value - inMin) / (inMax - inMin);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float mandelbrot()
{
    float i = 0;

    vec2 z = vec2(0.0);
    vec2 c = coords;
    vec2 e = vec2(0.0);

    for (; i < maxIterations; i++)
    {
        if (z.x * z.x + z.y * z.y >= 4)
            break;

        e = complexExp(toPolar(z), exponent);
        z = vec2(e.x + c.x, e.y + c.y);
    }

    return i;
}

void main()
{
    float iterations = mandelbrot();

    vec3 color = vec3(0.0);

    if (iterations == maxIterations)
        color = vec3(0.0);
    else
    {
        float hue = sqrt(iterations / maxIterations);
        color = hsv2rgb(vec3(hue, 1.0, 1.0));
    }

    fragColor = vec4(color, 1.0);
}