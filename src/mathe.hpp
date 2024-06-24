#pragma once

struct V2
{
    float x, y;
};

struct V2i
{
    int x, y;
};

float lerp(float a, float b, float t);
float length(V2 vector);
V2 normalize(V2 vector);
V2 operator-(V2 minu, V2 subtra);
V2 operator+(V2 first, V2 second);
V2 operator*(V2 first, V2 second);

#define array_size(x) (sizeof(x)/sizeof(*x))