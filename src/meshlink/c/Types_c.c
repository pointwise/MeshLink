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

#if defined(DEBUG)
#include <assert.h>
#endif


/* Helper function for trapping assertions */
void ML_assert(int val) {
#if defined(DEBUG)
    if (!val) {
        assert(val);
    }
#endif
}

/* Python wrapper test functions */
void printVec3(MLVector3D vec);
void printVec2(MLVector2D vec);

void addVec3(MLVector3D vec1, MLVector3D vec2);

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
