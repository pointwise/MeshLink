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
 **
 **  Mesh Storage and Manipulation
 **/

#if !defined(SURFMESH_H_)
#define SURFMESH_H_

#include "Types.h"
#include "MeshTopo.h"
#include "MeshModel.h"
#include <map>
#include <vector>

int projectToMeshTopoGeometry(
    MeshAssociativity &meshAssoc,
    MeshTopo &meshTopo,
    MLVector3D point,
    MLVector3D projectedPoint,
    MLREAL &projectionDist);


class SurfPoint {
public:
    MLVector3D r_;
};

class SurfFace;

class SurfEdge {
public:
    SurfEdge()
    {
        for (int i = 0; i < 2; ++i) {
            nodes_[i] = -1;
            faces_[i] = -1;
        }
    }
    SurfEdge(const SurfEdge &other)
    {
        for (int i = 0; i < 2; ++i) {
            nodes_[i] = other.nodes_[i];
            faces_[i] = other.faces_[i];
        }
    }

    // sort operator
    bool operator<(const SurfEdge &) const;

    // whether edges sort equally
    bool operator==(const SurfEdge &) const;

    SurfEdge & operator=(const SurfEdge &other) {
        for (int i = 0; i < 2; ++i) {
            nodes_[i] = other.nodes_[i];
            faces_[i] = other.faces_[i];
        }
        return *this;
    }

    // Reverse the nodes in an edge
    void flipEdge();

    static pwiFnvHash::FNVHash computeHash(MLINT i1, MLINT i2) {
        pwiFnvHash::FNVHash hash = pwiFnvHash::hashInit();
        if (i1 < i2) {
            hash = pwiFnvHash::hash(i1, hash);
            hash = pwiFnvHash::hash(i2, hash);
        }
        else {
            hash = pwiFnvHash::hash(i2, hash);
            hash = pwiFnvHash::hash(i1, hash);
        }
        return hash;
    }
    pwiFnvHash::FNVHash computeHash() {
        pwiFnvHash::FNVHash hash = pwiFnvHash::hashInit();
        if (nodes_[0] < nodes_[1]) {
            hash = pwiFnvHash::hash(nodes_[0], hash);
            hash = pwiFnvHash::hash(nodes_[1], hash);
        }
        else {
            hash = pwiFnvHash::hash(nodes_[1], hash);
            hash = pwiFnvHash::hash(nodes_[0], hash);
        }
        return hash;
    }


    MLINT nodes_[2];  // index into PointArray
    MLINT faces_[2];  // index into FaceArray
};

class SurfFace {
public:
    SurfFace():
        numNodes_(0)
    {
        for (int i = 0; i < 4; ++i) nodes_[i] = -1;
    }
    SurfFace(const SurfFace &other) {
        numNodes_ = other.numNodes_;
        for (int i = 0; i < 4; ++i) nodes_[i] = other.nodes_[i];
    }

    SurfFace & operator=(const SurfFace &other) {
        numNodes_ = other.numNodes_;
        for (int i = 0; i < 4; ++i) nodes_[i] = other.nodes_[i];
        return *this;
    }

    // Rotate the point indices in a face such that ind1 and ind2
    // are in the first and second nodes_ positions respectively
    void clockFace(MLINT ind1, MLINT ind2);

    MLINT nodes_[4];
    MLINT numNodes_;
};

typedef std::vector<SurfPoint> PointArray;
typedef std::vector<SurfEdge> EdgeArray;
typedef std::map<pwiFnvHash::FNVHash, MLINT> EdgeMap;
typedef std::vector<SurfFace> FaceArray;


/*********************************************************************
*  Class SurfMesh
*  A simple class for working with unstructured triangular mesh
*/
class SurfMesh {
public:
    // Read the FaceArray as a VRML file
    bool readVrml(const std::string &fname);

    // Write the FaceArray as a VRML file
    bool writeVrml(const std::string &fname);

    // Use the FaceArray to create the EdgeArray
    bool createEdges();

    // Find an edge in the mesh with the given indices
    // If found and matchOrientation is true, the returned
    // edge's nodes array will match the indices in the
    // order passed in.
    SurfEdge * findEdge(MLINT ind1, MLINT ind2, bool matchOrientation);

    // Split an edge
    bool splitEdge(MeshAssociativity &meshAssoc, 
        MeshModel* meshModel, SurfEdge &edge, 
        std::vector<MLINT> &modifiedEdges);

    // Check face data
    bool checkFaces() ;

    // Check edge data
    bool checkEdges() const;

    // Return true if the edge is used by the face
    bool faceHasEdge(const SurfFace &face, const SurfEdge &edge) const;

    // Add an edge to the mesh
    void addEdge(const SurfEdge &edge);

    // Update an edge in place with new values contained in source edge
    // The edge keeps its location in the SurfMesh structure, but
    // the data is updated to reflect the new construction.
    void updateEdge(SurfEdge &target, const SurfEdge &source);


    // Compute the aspect ratio of a face
    void computeFaceAspectRatio(const SurfFace &face,
        MLREAL &aspectRatio,
        MLREAL &minEdgeLength,
        MLREAL &maxEdgeLength);


    // Compute the cosine of the minimum included angle in the face
    MLREAL computeFaceMinIncludedAngleCosine(const SurfFace &face);

    MLREAL computeEdgeMinRadiusOfCurvature(
        SurfEdge &surfEdge,
        MeshAssociativity &meshAssoc,
        MeshModel &meshModel );

    void computeGeometryResolutionStatistics(
        MeshAssociativity &meshAssoc,
        MeshModel &meshModel
    );

    void setMeshConstraints(
        MLREAL minAllowedEdgeLength,
        MLREAL maxAllowedTriAspectRatio,
        MLREAL minAllowedTriIncludedAngle   // Min included angle (degrees)
    );

    // Given an edge, return is index in the Edge Array
    bool findEdgeArrayIndex(SurfEdge *edge, MLINT &edgeIndex);

    PointArray  points_;
    EdgeArray  edges_;
    EdgeMap    edgeMap_;
    FaceArray  faces_;

    MLREAL minAllowedEdgeLength_;
    MLREAL maxAllowedTriAspectRatio_;
    // Min included angle (degrees)
    MLREAL minAllowedTriIncludedAngle_;
    MLREAL minInclAngleCosineTol_;
};

#endif /* SURFMESH_H_ */

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
