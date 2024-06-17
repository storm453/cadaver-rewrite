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

    if(hyp == 0) return V2 {0, 0};

    V2 temp;
    
    temp.x = temp.x / hyp;
    temp.y = temp.y / hyp;

    return temp;
}

V2 operator-(V2 minu, V2 subtra)
{
    V2 temp;

    temp.x = minu.x - subtra.x;
    temp.y = minu.y - subtra.y;

    return temp;
}

V2 operator+(V2 first, V2 second)
{
    V2 result;

    result.x = first.x + second.x;
    result.y = first.y + second.y;

    return result;
}

V2 operator*(V2 first, V2 second)
{
    V2 result;

    result.x = first.x * second.x;
    result.y = first.y * second.y;

    return result;
}