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

#include "Types.h"
#include "MeshAssociativity.h"
#include "surf_mesh.h"
#include "quality.h"

void
EdgeQual::computeQuality(
    SurfMesh &surfMesh,
    MeshAssociativity &meshAssoc,
    MeshModel &meshModel,
    GeometryKernel *geom_kernel,
    MLREAL minAllowedEdgeLength
)
{
    quality_ = 0.0;
    static const MLREAL PREVENT_SPLIT_QUALITY = 0.0;
    static const MLREAL FORCE_SPLIT_QUALITY = 1e9;
    static const MLREAL BAD_TRI_QUALITY = PREVENT_SPLIT_QUALITY;
    MLINT n;
    MLREAL radOfCurvature;
    MLINT oppPtInd = -1;
    MLREAL aspectRatio;
    MLREAL minEdgeLen;
    MLREAL maxEdgeLen;
    MLREAL cosAngle;

    SurfEdge &surfEdge = surfMesh.edges_[index_];

    // Check mesh quality constraints first

    edgeLen_ = ptDist(surfMesh.points_[surfEdge.nodes_[0]].r_,
        surfMesh.points_[surfEdge.nodes_[1]].r_);
    if (edgeLen_ < 1.5 * minAllowedEdgeLength) {
        // edge is too short to split
        return;
    }

    for (int iface = 0; iface < 2; ++iface) {
        if (surfEdge.faces_[iface] >= 0) {
            SurfFace &face = surfMesh.faces_[surfEdge.faces_[iface]];
            oppPtInd = face.nodes_[0];
            for (n = 0; n < 3; ++n) {
                if (oppPtInd != surfEdge.nodes_[0] &&
                    oppPtInd != surfEdge.nodes_[1]) {
                    break;
                }
                oppPtInd = face.nodes_[n];
            }
            ML_assert(oppPtInd != surfEdge.nodes_[0] &&
                oppPtInd != surfEdge.nodes_[1]);

            // Triangle aspect ratio
            surfMesh.computeFaceAspectRatio(face, aspectRatio, minEdgeLen, maxEdgeLen);

            if (aspectRatio > surfMesh.maxAllowedTriAspectRatio_ && edgeLen_ > 0.8*maxEdgeLen) {
                // a long edge in a triangle with high aspect ratio
                // force a split
                quality_ = FORCE_SPLIT_QUALITY;
                return;
            }

            // Triangle min. included angle
            if (aspectRatio < 3.0 && edgeLen_ > 0.8*maxEdgeLen) {
                cosAngle = surfMesh.computeFaceMinIncludedAngleCosine(face);

                if (cosAngle > surfMesh.minInclAngleCosineTol_) {
                    // a long edge in a triangle with poor incl. angle
                    // force a split
                    quality_ = FORCE_SPLIT_QUALITY;
                    return;
                }
            }
        }
    }

    // Passed mesh quality constraints - check geometry curvature

    // determine radius of curvature from surface geometry
    radOfCurvature = surfMesh.computeEdgeMinRadiusOfCurvature(
        surfEdge, meshAssoc, meshModel);

    // quality = degrees of geometry radius of curvature subtended by the edge
    quality_ = calcCurvatureSpannedByEdgeLenth(edgeLen_, radOfCurvature);
}


MLREAL calcCurvatureSpannedByEdgeLenth(
    MLREAL edgeLength,
    MLREAL radOfCurvature
)
{
    return 360.0 * edgeLength / (TWO_PI * radOfCurvature);
}


MLREAL ptDist(MLVector3D pt1, MLVector3D pt2)
{
    MLREAL dist = 0.0;
    int n;
    for (n = 0; n < 3; ++n) {
        dist += (pt1[n] - pt2[n])*(pt1[n] - pt2[n]);
    }
    dist = sqrt(dist);
    return dist;
}

int getEdgeRadOfCurvature(MeshAssociativity &meshAssoc,
    const MeshEdge &edge,
    MLVector3D point1,
    MLVector3D point2,
    MLREAL &radOfCurvature)
{
    GeometryGroup *geom_group = NULL;
    MLVector2D UV = { 0.0, 0.0 };
    ParamVertex *const* paramVerts;
    edge.getParamVerts(&paramVerts);
    MLINT numSamples = 3;
    MLINT isam, n;
    MLREAL minCurv = 1e20;
    bool gotCurvature = false;
    MLREAL w;
    MLREAL invNumSamples = 1.0 / (numSamples + 1);
    MLVector3D samPoint;
    std::string projectName;
    MLREAL minRadOfCurvature, maxRadOfCurvature;

    GeometryKernel *geomKernel = meshAssoc.getActiveGeometryKernel();
    if (!geomKernel) {
        printf("ERROR: no active geometry kernel\n");
        ML_assert(0 == 1);
    }
    ProjectionData projectionData(geomKernel);

    for (isam = 0; isam < numSamples; ++isam) {
        geom_group = NULL;
        w = (isam + 1) * invNumSamples;
        projectName.clear();
        if (0 && paramVerts[0] && paramVerts[1]) {
            // have parametric data
            MLINT gref[2];
            for (int n = 0; n < 2; ++n) {
                gref[n] = paramVerts[n]->getGref();
            }
            if (gref[0] == gref[1]) {
                // all vertices constrained to the same geometry entity
                // interpolate UV and eval
                MLREAL u[2], v[2];
                for (int n = 0; n < 2; ++n) {
                    paramVerts[n]->getUV(&(u[n]), &(v[n]));
                }
                MLREAL dU = (u[1] - u[0]) * invNumSamples;
                MLREAL dV = (v[1] - v[0]) * invNumSamples;
                UV[0] = (1.0-w)*u[0] + w*u[1];
                UV[1] = (1.0-w)*v[0] + w*v[1];

                geom_group = meshAssoc.getGeometryGroupByID(gref[0]);
            }
        }

        if (NULL == geom_group) {
            // project midpoint to get UV and eval
            geom_group = meshAssoc.getGeometryGroupByID(edge.getGref());
            if (geom_group) {
                for (n = 0; n < 3; ++n) {
                    samPoint[n] = (1.0 - w)*point1[n] + w * point2[n];
                }
                if (!geomKernel->projectPoint(geom_group, samPoint, projectionData)) {
                    printf("Point projection failed\n");
                    return 1;
                }
                else {
                    if (!geomKernel->getProjectionUV(projectionData, UV)) {
                        return 1;
                    }
                    if (!geomKernel->getProjectionEntityName(projectionData, projectName)) {
                        return 1;
                    }
                }
            }
        }

        if (NULL == geom_group) {
            return 1;
        }

        std::vector<std::string> entityNames;
        if (!projectName.empty()) {
            entityNames.push_back(projectName);
        }
        else {
            entityNames = geom_group->getEntityNames();
        }
        for (MLINT i = 0; i < (MLINT)entityNames.size(); ++i) {
            if (geomKernel->evalRadiusOfCurvature(UV, entityNames[i], 
                &minRadOfCurvature, &maxRadOfCurvature)) {
                if (minCurv > minRadOfCurvature) {
                    minCurv = minRadOfCurvature;
                    gotCurvature = true;
                }
            }
        }
    }
    if (!gotCurvature) {
        printf("getRadiusOfCurvature failed\n");
        return 1;
    }
    radOfCurvature = minCurv;
    return 0;
}


MLREAL vdot(MLVector3D v1, MLVector3D v2)
{
    int n;
    MLREAL len1, len2, dot;


    len1 = len2 = dot = 0.0;
    for (n = 0; n < 3; ++n) {
        len1 += v1[n] * v1[n];
        len2 += v2[n] * v2[n];
        dot += v1[n] * v2[n];
    }
    len1 = sqrt(len1);
    len2 = sqrt(len2);
    return dot / len1 / len2;
}

MLREAL triMinInclCosAngle(MLVector3D pt1, MLVector3D pt2, MLVector3D pt3)
{
    int n;
    MLVector3D v1, v2;
    MLREAL dot, maxDot;
    maxDot = -2.0;

    for (n = 0; n < 3; ++n) {
        v1[n] = pt2[n] - pt1[n];
        v2[n] = pt3[n] - pt1[n];
    }
    dot = vdot(v1, v2);
    maxDot = dot;

    for (n = 0; n < 3; ++n) {
        v1[n] = pt3[n] - pt2[n];
        v2[n] = pt1[n] - pt2[n];
    }
    dot = vdot(v1, v2);
    maxDot = (std::max)(dot, maxDot);

    for (n = 0; n < 3; ++n) {
        v1[n] = pt2[n] - pt3[n];
        v2[n] = pt1[n] - pt3[n];
    }
    dot = vdot(v1, v2);
    maxDot = (std::max)(dot, maxDot);

    return maxDot;
}


MLREAL triAspectRatio(MLVector3D pt1, 
    MLVector3D pt2, 
    MLVector3D pt3,
    MLREAL &minLen,
    MLREAL &maxLen)
{
    maxLen = -1.0;
    minLen = 1e9;
    MLREAL edgeLen;
    int n;

    edgeLen = 0.0;
    for (n = 0; n < 3; ++n) {
        edgeLen += (pt1[n] - pt2[n]) * (pt1[n] - pt2[n]);
    }
    edgeLen = sqrt(edgeLen);
    minLen = (std::min)(edgeLen, minLen);
    maxLen = (std::max)(edgeLen, maxLen);

    edgeLen = 0.0;
    for (n = 0; n < 3; ++n) {
        edgeLen += (pt2[n] - pt3[n]) * (pt2[n] - pt3[n]);
    }
    edgeLen = sqrt(edgeLen);
    minLen = (std::min)(edgeLen, minLen);
    maxLen = (std::max)(edgeLen, maxLen);

    edgeLen = 0.0;
    for (n = 0; n < 3; ++n) {
        edgeLen += (pt1[n] - pt3[n]) * (pt1[n] - pt3[n]);
    }
    edgeLen = sqrt(edgeLen);
    minLen = (std::min)(edgeLen, minLen);
    maxLen = (std::max)(edgeLen, maxLen);

    return (maxLen / minLen);
}




// Closest point projection onto geometry of constrained mesh topo
int projectToMeshTopoGeometry(
    MeshAssociativity &meshAssoc,
    MeshTopo &meshTopo,
    MLVector3D point,
    MLVector3D projectedPoint,
    MLREAL &projectionDist)
{
    GeometryKernel *geom_kernel = meshAssoc.getActiveGeometryKernel();
    if (!geom_kernel) {
        printf("ERROR: no active geometry kernel\n");
        return 1;
    }

    ProjectionData projectionData(geom_kernel);
    GeometryGroup *geom_group =
        meshAssoc.getGeometryGroupByID(meshTopo.getGref());
    if (geom_group) {
        // point associated with geometry group
        // project point onto geometry group
        if (!geom_kernel->projectPoint(geom_group, point, projectionData)) {
            printf("Point projection failed\n");
            return 1;
        }
        else {
            geom_kernel->getProjectionXYZ(projectionData, projectedPoint);
            projectionDist = 0.0;
            for (int n = 0; n < 3; ++n) {
                projectionDist += std::pow((point[n] - projectedPoint[n]), 2.0);
            }
            projectionDist = std::sqrt(projectionDist);
        }
    }
    else {
        printf("missing Geometry Group\n");
        ML_assert(NULL != geom_group);
        return 1;
    }
    return 0;
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
