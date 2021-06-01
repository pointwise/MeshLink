/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

/***********************************************************************
 **  Refinement Queue Functions
 **/

#if !defined(QUEUE_H_)
#define QUEUE_H_

#include "Types.h"
#include "surf_mesh.h"
#include "MeshAssociativity.h"


typedef std::vector<EdgeQual> EdgeRefineQueue;


void addNeighborsToQueue(
    SurfMesh &surfMesh,
    MeshAssociativity &meshAssoc,
    MeshModel &meshModel,
    GeometryKernel *geom_kernel,
    MLREAL qualityThreshold,
    EdgeRefineQueue &queue);

#endif /* QUEUE_H_ */

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
