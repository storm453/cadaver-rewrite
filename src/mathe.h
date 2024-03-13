#pragma once

struct V2
{
    float x, y;
};

struct V2i
{
    int x, y;
};

#define array_size(x) (sizeof(x)/sizeof(*x))