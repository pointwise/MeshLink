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
 **  Mesh Quality Functions
 **/

#if !defined(QUALITY_H_)
#define QUALITY_H_

#include "Types.h"
#include "surf_mesh.h"
#include "MeshAssociativity.h"
#include <cmath>


static const MLREAL PI = 4.0*atan(1.0);
static const MLREAL TWO_PI = 2.0*PI;

class EdgeQual {
public:
    EdgeQual(MLINT i) :
        index_(i),
        edgeLen_(1e9),
        quality_(0.0)
    {
    }

    bool operator<(const EdgeQual &other) const
    {
#if defined(SORT_BY_QUALITY)
        // sort by edge quality - best quality edge first
        //if (quality_ < other.quality_) return true;
        //return false;
#else
        // sort by edge length - shortest edge first
        if (edgeLen_ < other.edgeLen_) return true;
        return false;
#endif
    }
    void computeQuality(
        SurfMesh &surfMesh,
        MeshAssociativity &meshAssoc,
        MeshModel &meshModel,
        GeometryKernel *geom_kernel,
        MLREAL minAllowedEdgeLength
    );

    MLINT index_;
    MLREAL edgeLen_;
    MLREAL quality_;
private:
    EdgeQual();
};


MLREAL ptDist(MLVector3D pt1, MLVector3D pt2);

// Get the radius of curvature of the geometry along an edge
// Uses sampling of the geometry between the end points
int getEdgeRadOfCurvature(MeshAssociativity &meshAssoc,
    const MeshEdge &edge,
    MLVector3D endpoint1,
    MLVector3D endpoint2,
    MLREAL &radOfCurvature);

// Given a radius of curvature and an edge length,
// determine the arc sector angle (deg) of the edge on the circle.
MLREAL calcCurvatureSpannedByEdgeLenth(
    MLREAL edgeLength,
    MLREAL radOfCurvature
);

// Dot product of two vectors
MLREAL vdot(MLVector3D v1, MLVector3D v2);

// Compute the minimum included angle in the triangle
MLREAL triMinInclCosAngle(MLVector3D pt1, MLVector3D pt2, MLVector3D pt3);

// Compute the aspect ratio of the triangle
MLREAL triAspectRatio(MLVector3D pt1,
    MLVector3D pt2,
    MLVector3D pt3,
    MLREAL &minLen,
    MLREAL &maxLen);


// Closest point projection onto geometry of constrained mesh topo
int projectToMeshTopoGeometry(
    MeshAssociativity &meshAssoc,
    MeshTopo &meshTopo,
    MLVector3D point,
    MLVector3D projectedPoint,
    MLREAL &projectionDist);


#endif /* QUALITY_H_ */

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
