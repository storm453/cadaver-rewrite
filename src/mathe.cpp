#include <cmath>
#include "mathe.h"

float length(V2 vector)
{
    return sqrtf(vector.x * vector.x + vector.y * vector.y);
}