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

// MeshLink Library Headers
#include "Types.h"
#include "MeshAssociativity.h"

// Refine_Uns Project Headers
#include "surf_mesh.h"
#include "quality.h"
#include "queue.h"

/******************************************************
* addNeighborsToQueue
*
* Attempt to ease mesh quality degredation on the mesh
* by blending "bad" edge quality to neighbor edges.
* The idea is that neighbors of "really bad" edges should
* be split as well.
*/
void addNeighborsToQueue(
    SurfMesh &surfMesh,
    MeshAssociativity &meshAssoc,
    MeshModel &meshModel,
    GeometryKernel *geom_kernel,
    MLREAL qualityThreshold,
    EdgeRefineQueue &queue)
{
    const MLREAL qualFrac = 0.5;  // Fraction of bad quality to give to neighbor
    MLINT numRounds = 2;          // number of rounds (e.g. 2 = neighbors of neighbors)    

    MLINT i, n, iface;
    MLINT num = (MLINT)queue.size();
    MLINT oppPtInd;
    SurfEdge *neighborSurfEdge;
    MLINT numEdges = (MLINT)surfMesh.edges_.size();
    MLINT neighborEdgeInd;
    EdgeQual edgeQnew(0);
    std::set<MLINT>::iterator setIter;
    MLREAL quality;
    MLINT iround;

    // queueSet keeps us from rechecking edges
    std::set<MLINT> queueSet;
    for (i = 0; i < num; ++i) {
        EdgeQual &edgeQ = queue[i];
        queueSet.insert(edgeQ.index_);
    }

    for (iround = 0; iround < numRounds; ++iround) {
        num = (MLINT)queue.size();
        for (i = 0; i < num; ++i) {
            EdgeQual &edgeQ = queue[i];
            SurfEdge &surfEdge = surfMesh.edges_[edgeQ.index_];
            quality = edgeQ.quality_;

            // check edges in faces attached to this edge
            for (iface = 0; iface < 2; ++iface) {
                if (surfEdge.faces_[iface] >= 0) {
                    SurfFace &parentFace = surfMesh.faces_[surfEdge.faces_[iface]];
                    oppPtInd = parentFace.nodes_[0];
                    for (n = 0; n < 3; ++n) {
                        if (oppPtInd != surfEdge.nodes_[0] &&
                            oppPtInd != surfEdge.nodes_[1]) {
                            break;
                        }
                        oppPtInd = parentFace.nodes_[n];
                    }
                    ML_assert(oppPtInd != surfEdge.nodes_[0] &&
                        oppPtInd != surfEdge.nodes_[1]);

                    // find edge running from node[0] to oppPtInd
                    neighborSurfEdge = surfMesh.findEdge(surfEdge.nodes_[0], oppPtInd, false);
                    ML_assert(NULL != neighborSurfEdge);

                    surfMesh.findEdgeArrayIndex(neighborSurfEdge, neighborEdgeInd);

                    setIter = queueSet.find(neighborEdgeInd);
                    if (setIter == queueSet.end()) {
                        // edge not yet in the queue
                        edgeQnew.index_ = neighborEdgeInd;
                        edgeQnew.computeQuality(surfMesh, meshAssoc,
                            meshModel, geom_kernel, surfMesh.minAllowedEdgeLength_);

                        // spread quality to neighbors
                        edgeQnew.quality_ = (std::max)(quality*qualFrac, edgeQnew.quality_);

                        if (edgeQnew.quality_ > qualityThreshold) {
                            queueSet.insert(neighborEdgeInd);
                            queue.push_back(edgeQnew);
                        }
                    }

                    // find edge running from node[1] to oppPtInd
                    neighborSurfEdge = surfMesh.findEdge(surfEdge.nodes_[1], oppPtInd, false);
                    ML_assert(NULL != neighborSurfEdge);

                    surfMesh.findEdgeArrayIndex(neighborSurfEdge, neighborEdgeInd);

                    setIter = queueSet.find(neighborEdgeInd);
                    if (setIter == queueSet.end()) {
                        // edge not yet in the queue
                        edgeQnew.index_ = neighborEdgeInd;
                        edgeQnew.computeQuality(surfMesh, meshAssoc,
                            meshModel, geom_kernel, surfMesh.minAllowedEdgeLength_);

                        // spread quality to neighbors
                        edgeQnew.quality_ = (std::max)(quality*qualFrac, edgeQnew.quality_);

                        if (edgeQnew.quality_ > qualityThreshold) {
                            queueSet.insert(neighborEdgeInd);
                            queue.push_back(edgeQnew);
                        }
                    }
                }
            }
        }
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
