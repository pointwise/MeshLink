/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

//#include "MeshAssociativity.h"

#include "GeomKernel_Geode.h"
#include "GeomKernel_Geode_c.h"

int
ML_createGeometryKernelGeodeObj(GeometryKernelObj *geomKernelObj)
{
    if (geomKernelObj) {
        *geomKernelObj = (GeometryKernelObj) new GeometryKernelGeode;
        if (*geomKernelObj) {
            return 0;
        }
    }
    return 1;
}

void
ML_freeGeometryKernelGeodeObj(GeometryKernelObj *geomKernelObj)
{
    GeometryKernelGeode *geomKernel = (GeometryKernelGeode *)*geomKernelObj;
    if (geomKernel) {
        delete geomKernel;
        *geomKernelObj = NULL;
    }
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
