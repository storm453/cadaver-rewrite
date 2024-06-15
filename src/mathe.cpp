#include <cmath>
#include "mathe.hpp"

float lerp(float a, float b, float t)
{
    return a * (1 - t) + b * t;
}

float length(V2 vector)
{
    return sqrtf(vector.x * vector.x + vector.y * vector.y);
}

V2 normalize(V2 vector)
{
    float hyp = length(vector);

    V2 temp;
    
    temp.x = temp.x / hyp;
    temp.y = temp.y / hyp;

    return temp;
}

V2 sub(V2 minu, V2 subtra)
{
    V2 temp;

    temp.x = minu.x - subtra.x;
    temp.y = minu.y - subtra.y;

    return temp;
}