/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

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

/****************************************************************************
 *
 * This file is licensed under the Cadence Public License Version 1.0 (the
 * "License"), a copy of which is found in the included file named "LICENSE",
 * and is distributed "AS IS." TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE
 * LAW, CADENCE DISCLAIMS ALL WARRANTIES AND IN NO EVENT SHALL BE LIABLE TO
 * ANY PARTY FOR ANY DAMAGES ARISING OUT OF OR RELATING TO USE OF THIS FILE.
 * Please see the License for the full text of applicable terms.
 *
 ****************************************************************************/
