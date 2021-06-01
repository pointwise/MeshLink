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
 **  SurfMesh Functions for edge refinement
 **/

#include "surf_mesh.h"
#include "quality.h"

//#include <stdio.h>
#include <string>
#include <algorithm>
#include <assert.h>


bool
SurfEdge::operator==(const SurfEdge &edge2) const
{
    if (*this < edge2) return false;
    if (edge2 < *this) return false;
    return true;
}

bool 
SurfEdge::operator<(const SurfEdge &edge2) const
{
    MLINT edge1_inds[2];
    MLINT edge2_inds[2];

    if (nodes_[0] < nodes_[1]) {
        edge1_inds[0] = nodes_[0];
        edge1_inds[1] = nodes_[1];
    }
    else {
        edge1_inds[0] = nodes_[1];
        edge1_inds[1] = nodes_[0];
    }

    if (edge2.nodes_[0] < edge2.nodes_[1]) {
        edge2_inds[0] = edge2.nodes_[0];
        edge2_inds[1] = edge2.nodes_[1];
    }
    else {
        edge2_inds[0] = edge2.nodes_[1];
        edge2_inds[1] = edge2.nodes_[0];
    }

    if (edge1_inds[0] < edge2_inds[0]) {
        return true;
    }
    else if (edge1_inds[0] > edge2_inds[0]) {
        return false;
    }
    else {
        if (edge1_inds[1] < edge2_inds[1]) {
            return true;
        }
        else if (edge1_inds[1] > edge2_inds[1]) {
            return false;
        }
        else {
            return false;
        }
    }
}


// Use the FaceArray to create the EdgeArray
bool
SurfMesh::createEdges()
{
    bool status = true;
    EdgeArray sortedEdges;
    SurfEdge edge;
    FaceArray::iterator fiter;
    MLINT iface;
    for (iface = 0, fiter = faces_.begin(); fiter != faces_.end(); ++iface, ++fiter) {
        edge.faces_[0] = iface;
        edge.nodes_[0] = fiter->nodes_[0];
        edge.nodes_[1] = fiter->nodes_[1];
        sortedEdges.push_back(edge);

        edge.faces_[0] = iface;
        edge.nodes_[0] = fiter->nodes_[1];
        edge.nodes_[1] = fiter->nodes_[2];
        sortedEdges.push_back(edge);

        edge.faces_[0] = iface;
        edge.nodes_[0] = fiter->nodes_[2];
        edge.nodes_[1] = fiter->nodes_[0];
        sortedEdges.push_back(edge);
    }

    std::sort(sortedEdges.begin(), sortedEdges.end());

    edges_.clear();
    addEdge(sortedEdges[0]);
    MLUINT numEdges = sortedEdges.size();
    MLUINT i;
    for (i = 1; i < numEdges; ++i) {
        SurfEdge &lastEdge = edges_.back();
        if (lastEdge == sortedEdges[i]) {
            lastEdge.faces_[1] = sortedEdges[i].faces_[0];
        }
        else {
            addEdge(sortedEdges[i]);
        }
    }
    return status;
}

// Rotate the point indices in a face such that ind1 and ind2
// are in the first and second nodes_ positions respectively
void 
SurfFace::clockFace(MLINT ind1, MLINT ind2)
{
    MLINT i,j;
    // ensure the points are in the face
    for (i = 0; i < numNodes_; ++i) {
        if (nodes_[i] == ind1) {
            j = i+1;
            if (j == numNodes_) j = 0;
            ML_assert(nodes_[j] == ind2);
            break;
        }
    }
    ML_assert(i < numNodes_);
    // clock nodes until we have the positions desired
    while (nodes_[0] != ind1) {
        j = nodes_[0];
        for (i = 0; i < numNodes_-1; ++i) {
            nodes_[i] = nodes_[i+1];
        }
        nodes_[i] = j;
    }
    ML_assert(nodes_[0] == ind1);
    ML_assert(nodes_[1] == ind2);
}

// Reverse the nodes in an edge
void 
SurfEdge::flipEdge()
{
    MLINT i;
    MLINT face;
    i = nodes_[0];
    nodes_[0] = nodes_[1];
    nodes_[1] = i;

    face = faces_[0];
    faces_[0] = faces_[1];
    faces_[1] = face;
}

// Find an edge in the mesh with the given indices
// If found and matchOrientation is true, the returned
// edge's nodes array will match the indices in the
// order passed in.
SurfEdge *
SurfMesh::findEdge(MLINT ind1, MLINT ind2, bool matchOrientation)
{
    pwiFnvHash::FNVHash hash = SurfEdge::computeHash(ind1, ind2);
    EdgeMap::iterator iter = edgeMap_.find(hash);
    if (iter != edgeMap_.end()) {
        MLINT edgeInd = iter->second;
        if (edgeInd < 0 || edgeInd >= (MLINT)edges_.size()) {
            ML_assert(false);
        }
        SurfEdge *edge = &(edges_[edgeInd]);
        if (matchOrientation) {
            if (edge->nodes_[0] != ind1) {
                edge->flipEdge();
            }
            ML_assert(edge->nodes_[0] == ind1 && edge->nodes_[1] == ind2);
        }
        ML_assert(
            (edge->nodes_[0] == ind1 && edge->nodes_[1] == ind2) ||
            (edge->nodes_[0] == ind2 && edge->nodes_[1] == ind1));
        return edge;
    }
    return NULL;
}


// Determine if a face contains an edge (by node index matching)
bool 
SurfMesh::faceHasEdge(const SurfFace &face, const SurfEdge &edge) const
{
    //SurfFace clockedFace = face;
    if (&(faces_[edge.faces_[0]]) == &face) {
        //clockFace(clockedFace, edge.nodes_[0], edge.nodes_[1]);
        return true;
    }
    if (&(faces_[edge.faces_[1]]) == &face) {
        //clockFace(clockedFace, edge.nodes_[1], edge.nodes_[0]);
        return true;
    }
    return false;
}


bool
SurfMesh::checkFaces() 
{
    MLINT numPoints = (MLINT)points_.size();
    FaceArray::const_iterator iter;
    
    for (iter = faces_.begin(); iter != faces_.end(); ++iter) {
        const SurfFace & face = *iter;
        ML_assert(face.numNodes_ == 3);
        ML_assert(face.nodes_[0] >= 0 && face.nodes_[0] < numPoints);
        ML_assert(face.nodes_[1] >= 0 && face.nodes_[1] < numPoints);
        ML_assert(face.nodes_[2] >= 0 && face.nodes_[2] < numPoints);

        ML_assert(NULL != findEdge(face.nodes_[0], face.nodes_[1], false));
        ML_assert(NULL != findEdge(face.nodes_[1], face.nodes_[2], false));
        ML_assert(NULL != findEdge(face.nodes_[2], face.nodes_[0], false));
    }
    return true;
}

bool 
SurfMesh::checkEdges() const
{
    MLINT numPoints = (MLINT)points_.size();
    EdgeArray::const_iterator iter;
    for (iter = edges_.begin(); iter != edges_.end(); ++iter) {
        const SurfEdge & edge = *iter;
        ML_assert(edge.nodes_[0] >= 0 && edge.nodes_[0] < numPoints);
        ML_assert(edge.nodes_[1] >= 0 && edge.nodes_[1] < numPoints);

        ML_assert(faceHasEdge(faces_[edge.faces_[0]], *iter));
        if (edge.faces_[1] >= 0) {
            ML_assert(faceHasEdge(faces_[edge.faces_[1]], *iter));
        }
    }
    return true;
}



void 
SurfMesh::addEdge(const SurfEdge &edge) {
    edges_.push_back(edge);
    pwiFnvHash::FNVHash hash = SurfEdge::computeHash(edge.nodes_[0], edge.nodes_[1]);
    edgeMap_[hash] = (MLINT)edges_.size()-1;
}


// Update an edge in place with new values contained in source edge
// The edge keeps its location in the SurfMesh structure, but
// the data is updated to reflect the new construction.
void
SurfMesh::updateEdge(SurfEdge &target, const SurfEdge &source) {

    pwiFnvHash::FNVHash oldHash = SurfEdge::computeHash(target.nodes_[0], target.nodes_[1]);
    EdgeMap::iterator iter = edgeMap_.find(oldHash);
    if (iter != edgeMap_.end()) {
        MLINT edgeInd = iter->second;

        // remove oldHash from map
        edgeMap_.erase(oldHash);

        target = source;

        // add to map with updated hash
        pwiFnvHash::FNVHash hash = SurfEdge::computeHash(target.nodes_[0], target.nodes_[1]);
        edgeMap_[hash] = edgeInd;
    }
    else {
        // edge not in database
        ML_assert(false);
    }
}

void
SurfMesh::computeFaceAspectRatio(const SurfFace &face,
    MLREAL &aspectRatio,
    MLREAL &minEdgeLength,
    MLREAL &maxEdgeLength) 
{
    aspectRatio = triAspectRatio(
        points_[face.nodes_[0]].r_,
        points_[face.nodes_[1]].r_,
        points_[face.nodes_[2]].r_,
        minEdgeLength, maxEdgeLength);
}

MLREAL
SurfMesh::computeFaceMinIncludedAngleCosine(const SurfFace &face)
{
    MLREAL cosAngle = triMinInclCosAngle(
        points_[face.nodes_[0]].r_,
        points_[face.nodes_[1]].r_,
        points_[face.nodes_[2]].r_ );
    return cosAngle;
}

void
SurfMesh::computeGeometryResolutionStatistics(
    MeshAssociativity &meshAssoc,
    MeshModel &meshModel
)
{
    //return;
    MLINT i;
    MLREAL edgeLength, radOfCurvature, curveSpanDeg;
    MLREAL maxCurveSpanDeg = 0.0;
    MLINT numEdges = edges_.size();
    MLVector3D *pt1, *pt2;
    MLREAL totalCurveSpan = 0.0;
    MLREAL avgCurveSpan = 0.0;
    MLINT numConstrainedEdges = 0;
    MLVector3D *maxCurveSpanPts[2] = { NULL, NULL };
    MLREAL maxCurveSpanEdgeLen = 0.0;
    MLREAL maxCurveSpanRoC = 0.0;
    MLREAL edgeLengthThresh = 1.5 * minAllowedEdgeLength_;
    for (i = 0; i < numEdges; ++i) {
        SurfEdge &edge = edges_[i];

        pt1 = &(points_[edge.nodes_[0]].r_);
        pt2 = &(points_[edge.nodes_[1]].r_);
        edgeLength = ptDist(*pt1, *pt2);
        if (edgeLength > edgeLengthThresh) {
            radOfCurvature = computeEdgeMinRadiusOfCurvature(edge, meshAssoc, meshModel);
            if (radOfCurvature < 1e2) {
                curveSpanDeg = calcCurvatureSpannedByEdgeLenth(edgeLength, radOfCurvature);
                if (maxCurveSpanDeg < curveSpanDeg) {
                    maxCurveSpanDeg = curveSpanDeg;
                    maxCurveSpanPts[0] = pt1;
                    maxCurveSpanPts[1] = pt2;
                    maxCurveSpanEdgeLen = edgeLength;
                    maxCurveSpanRoC = radOfCurvature;
                }
                numConstrainedEdges++;
                totalCurveSpan += curveSpanDeg;
            }
        }
    }
    if (numConstrainedEdges) {
        avgCurveSpan = totalCurveSpan / numConstrainedEdges;
    }

    printf("\nGeometry Resolution Statistics\n");

    printf("  Number of Faces: %6" MLINT_FORMAT "\n", (MLINT)faces_.size());
    printf("  Number of Edges: %6" MLINT_FORMAT "\n", (MLINT)edges_.size());

    printf("  Number of Constrained Edges: %6" MLINT_FORMAT "\n", numConstrainedEdges);
    printf("  Avg. Edge Circular Arc Subtension: %6.1f deg\n", avgCurveSpan);
    printf("  Max. Edge Circular Arc Subtension: %6.1f deg\n", maxCurveSpanDeg);
    printf("     ( %11.4e, %11.4e, %11.4e )\n", 
        (*maxCurveSpanPts[0])[0], (*maxCurveSpanPts[0])[1], (*maxCurveSpanPts[0])[2] );
    printf("     ( %11.4e, %11.4e, %11.4e )\n",
        (*maxCurveSpanPts[1])[0], (*maxCurveSpanPts[1])[1], (*maxCurveSpanPts[1])[2]);
    printf("     Edge Length      : %11.4e\n", maxCurveSpanEdgeLen);
    printf("     Rad. of Curvature: %11.4e\n", maxCurveSpanRoC);
}

MLREAL
SurfMesh::computeEdgeMinRadiusOfCurvature(
    SurfEdge &surfEdge,
    MeshAssociativity &meshAssoc,
    MeshModel &meshModel
)
{
    MLREAL radOfCurvature;
    MLREAL minRadOfCurvature = 1e9;

    // determine radius of curvature from surface geometry
    // by using the face-edge (MeshSheet) associativity
    std::vector<MeshSheet *> meshSheets;
    meshModel.getMeshSheets(meshSheets);
    std::vector<MeshSheet *>::iterator iter;
    std::map<MLINT, MeshEdge *> edgesToCheck;
    for (iter = meshSheets.begin(); iter != meshSheets.end(); ++iter) {
        const MeshSheet *meshSheet = *iter;
        MeshEdge *assocSheetEdge =
            meshSheet->findFaceEdgeByInds(
                surfEdge.nodes_[0] + 1,
                surfEdge.nodes_[1] + 1);

        if (assocSheetEdge) {
            edgesToCheck[assocSheetEdge->getGref()] = assocSheetEdge;
        }
    }

    std::map<MLINT, MeshEdge *>::iterator checkIter;
    for (checkIter=edgesToCheck.begin(); checkIter != edgesToCheck.end(); ++checkIter) {
        MeshEdge *assocSheetEdge = checkIter->second;
        if (0 != getEdgeRadOfCurvature(meshAssoc,
            *assocSheetEdge,
            points_[surfEdge.nodes_[0]].r_,
            points_[surfEdge.nodes_[1]].r_,
            radOfCurvature)) {
            continue;
        }
        minRadOfCurvature = (std::min)(radOfCurvature, minRadOfCurvature);
    }
    return minRadOfCurvature;
}

void 
SurfMesh::setMeshConstraints(
    MLREAL minAllowedEdgeLength,
    MLREAL maxAllowedTriAspectRatio,
    MLREAL minAllowedTriIncludedAngle   // Min included angle (degrees)
)
{
    minAllowedEdgeLength_ = minAllowedEdgeLength;
    maxAllowedTriAspectRatio_ = maxAllowedTriAspectRatio;
    minAllowedTriIncludedAngle_ = minAllowedTriIncludedAngle;
    minInclAngleCosineTol_ = cos(minAllowedTriIncludedAngle_ * PI / 180.);
}


// Given an edge, return is index in the Edge Array
bool 
SurfMesh::findEdgeArrayIndex(SurfEdge *edge, MLINT &edgeIndex)
{
    // pointer subtraction, only works because we're using 
    // a simple vector for EdgeArray
    edgeIndex = (MLINT)( edge - &(edges_[0]) );
    ML_assert( edgeIndex >= 0 && edgeIndex < (MLINT)(edges_.size()) );
    if (edgeIndex >= 0 && edgeIndex < (MLINT)(edges_.size())) {
        return true;
    }
    edgeIndex = 0;
    return false;
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
