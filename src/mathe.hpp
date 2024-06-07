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

#define array_size(x) (sizeof(x)/sizeof(*x))