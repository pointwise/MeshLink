#include "Types.h"
#include <assert.h>
#include <stdio.h>



void printVec3(MLVector3D vec)
{
    printf("MLVector3D: %f  %f  %f\n",
        vec[0], vec[1], vec[2]);
}
void printVec2(MLVector2D vec)
{
    printf("MLVector2D: %f  %f\n",
    vec[0], vec[1]);
}

void addVec3(MLVector3D vec1, MLVector3D vec2)
{
    vec1[0] += vec2[0];
    vec1[1] += vec2[1];
    vec1[2] += vec2[2];

    vec2[0] += 1;
    vec2[1] += 1;
    vec2[2] += 1;
}
