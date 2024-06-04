#include <cmath>
#include "mathe.h"

float lerp(float a, float b, float t)
{
    return a * (1 - t) + b * t;
}

float length(V2 vector)
{
    return sqrtf(vector.x * vector.x + vector.y * vector.y);
}