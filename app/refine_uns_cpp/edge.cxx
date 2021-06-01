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
#include <stdio.h>
#include <string>
#include <algorithm>
#include <assert.h>

/************************************************************************
* splitEdge
*
* Split an edge in the mesh by inserting a new point at the mid-point.
* This results in two new child edges of the parent and each attached
* face being split into two faces with one new edge.
*
*          ^                       ^
*        /   \                   / | \
*      /       \               /   |   \
*    /           \           /     |     \
*  <--------------->  ==>  <-------O------->
*    \            /         \      |      /
*      \        /             \    |    /
*        \    /                 \  |  /
*          \/                     \ /
*
*         This                Becomes This
*
*  We will reuse storage of the beginning edge and face(s).
*/

bool 
SurfMesh::splitEdge(MeshAssociativity &meshAssoc, 
    MeshModel* meshModel,
    SurfEdge &edge,
    std::vector<MLINT> &modifiedEdges  // indices of updated edges
)
{
    bool status = true;
    SurfEdge parentEdge = edge;
    SurfEdge childEdge1, childEdge2;
    SurfFace childFace1, childFace2;
    SurfPoint newPt;
    int n;
    modifiedEdges.clear();

    // parent edge mesh associativity indices (1-based)
    MLINT parentInds[4] = {-1,-1,-1,-1};
    parentInds[0] = edge.nodes_[0] + 1;
    parentInds[1] = edge.nodes_[1] + 1;

    // parentFace1 - the face on the "inside" of the edge
    SurfFace parentFace1 = faces_[parentEdge.faces_[0]];

    // clock the face so that nodes 0-1 are the edge being split
    parentFace1.clockFace( parentEdge.nodes_[0], parentEdge.nodes_[1] );

    // index of parentFace1 point opposite the split edge
    parentInds[2] = parentFace1.nodes_[2] + 1;

    // parentFace2 - the face on the "outside" of the edge
    // It's possible there isn't one (lamina boundary)
    if (parentEdge.faces_[1] >= 0) {
        SurfFace parentFace2 = faces_[parentEdge.faces_[1]];

        // clock the face so that nodes 0-1 are the edge being split
        parentFace2.clockFace( parentEdge.nodes_[1], parentEdge.nodes_[0] );

        // index of parentFace2 point opposite the split edge
        parentInds[3] = parentFace2.nodes_[2] + 1;
    }

    // The location of the point being inserted
    MLINT newPtInd = points_.size();
    for (n = 0; n < 3; ++n) {
        newPt.r_[n] = 0.5 * ( points_[parentEdge.nodes_[0]].r_[n] + 
                              points_[parentEdge.nodes_[1]].r_[n] );
    }

    // project the point to the lowest topology assocation
    MeshEdge *assocEdge = meshModel->findLowestTopoEdgeByInds(
            parentInds[0], parentInds[1]);
    if (assocEdge) {
        // parent edge was associated with geometry group
        // project split point onto geometry group
        MLVector3D projectedPoint;
        MLREAL projectionDist;
        if (0 != projectToMeshTopoGeometry(meshAssoc, *assocEdge,
            newPt.r_,
            projectedPoint, projectionDist)) {
            printf("bad edge point projection\n");
            ML_assert(0 == 1);
            return false;
        }
        else {
            // update point position
            for (n = 0; n < 3; ++n) {
                newPt.r_[n] = projectedPoint[n];
            }
        }
    }

    // add the point to the mesh
    points_.push_back(newPt);

    // indices of child edges
    childEdge1.nodes_[0] = parentEdge.nodes_[0];
    childEdge1.nodes_[1] = newPtInd;

    childEdge2.nodes_[0] = newPtInd;
    childEdge2.nodes_[1] = parentEdge.nodes_[1];

    //=== split face 1
    SurfEdge face1SplitEdge;
    SurfEdge face2SplitEdge;
    {
        // find the edge running from the split edge to the face 1 apex
        // orient it to match the face orientation
        SurfEdge *fe2 = NULL;
        fe2 = findEdge(parentFace1.nodes_[1], parentFace1.nodes_[2], true);
        ML_assert(NULL != fe2);
        ML_assert(fe2->faces_[0] == parentEdge.faces_[0]);

        // new edge splitting face 1 
        face1SplitEdge.nodes_[0] = newPtInd;
        face1SplitEdge.nodes_[1] = parentFace1.nodes_[2];

        // child face 1
        childFace1.numNodes_ = parentFace1.numNodes_;
        childFace1.nodes_[0] = childEdge1.nodes_[0];
        childFace1.nodes_[1] = childEdge1.nodes_[1];
        childFace1.nodes_[2] = parentFace1.nodes_[2];

        // child face 2
        childFace2.numNodes_ = parentFace1.numNodes_;
        childFace2.nodes_[0] = childEdge2.nodes_[0];
        childFace2.nodes_[1] = childEdge2.nodes_[1];
        childFace2.nodes_[2] = parentFace1.nodes_[2];

        // reuse face 1 storage for childFace1 topology
        faces_[parentEdge.faces_[0]] = childFace1;
        childEdge1.faces_[0]     = parentEdge.faces_[0];  // childEdge1 points into reused face 1
        face1SplitEdge.faces_[0] = parentEdge.faces_[0];  // face1SplitEdge points into reused face 1

        // add new face to mesh (childFace 2)
        faces_.push_back(childFace2);
        childEdge2.faces_[0] = faces_.size()-1;           // childEdge2 points into the new face

        fe2->faces_[0] = faces_.size() - 1;               // old face edge 2 now points into new face

        face1SplitEdge.faces_[1] = faces_.size() - 1;     // face1SplitEdge points away from new face
    }

    //=== split face 2
    bool haveFace2 = false;
    if (parentEdge.faces_[1] >= 0) {
        haveFace2 = true;
        // find the edge running from the split edge to the face 2 apex
        // orient it to match the face orientation
        SurfEdge *fe3 = NULL;
        SurfFace parentFace2 = faces_[parentEdge.faces_[1]];
        parentFace2.clockFace( parentEdge.nodes_[1], parentEdge.nodes_[0] );
        fe3 = findEdge(parentFace2.nodes_[2], parentFace2.nodes_[0], true);
        ML_assert(NULL != fe3);
        ML_assert(fe3->faces_[0] == parentEdge.faces_[1]);


        // new edge splitting face 2
        face2SplitEdge.nodes_[0] = parentFace2.nodes_[2];
        face2SplitEdge.nodes_[1] = newPtInd;

        // child face 1
        childFace1.numNodes_ = parentFace2.numNodes_;
        childFace1.nodes_[0] = childEdge1.nodes_[1];
        childFace1.nodes_[1] = childEdge1.nodes_[0];
        childFace1.nodes_[2] = parentFace2.nodes_[parentFace2.numNodes_ - 1];

        // child face 2
        childFace2.numNodes_ = parentFace2.numNodes_;
        childFace2.nodes_[0] = childEdge2.nodes_[1];
        childFace2.nodes_[1] = childEdge2.nodes_[0];
        childFace2.nodes_[2] = parentFace2.nodes_[parentFace2.numNodes_ - 1];

        // reuse face 2 storage
        faces_[parentEdge.faces_[1]] = childFace1;
        childEdge1.faces_[1] = parentEdge.faces_[1];      // childEdge1 points away from reused face 2
        face2SplitEdge.faces_[0] = parentEdge.faces_[1];  // facewSplitEdge points into reused face 1

        // add new face to mesh (childFace 2)
        faces_.push_back(childFace2);
        childEdge2.faces_[1] = faces_.size()-1;           // childEdge2 points away from new face

        fe3->faces_[0] = faces_.size() - 1;               // old face edge 3 now points into new  face

        face2SplitEdge.faces_[1] = faces_.size() - 1;     // face2SplitEdge points away from new face
    }

    // Now that face topology is up to date, we can update
    // the original edge topology to match childEdge1
    updateEdge(edge, childEdge1);
    MLINT edgeIndex;
    if (findEdgeArrayIndex( &(edge), edgeIndex ) ) {
        modifiedEdges.push_back(edgeIndex);
    }


    // add new child and faceSplit edges
    addEdge(childEdge2);
    modifiedEdges.push_back((MLINT)(edges_.size())-1);
    addEdge(face1SplitEdge); // add the splitting edge
    modifiedEdges.push_back((MLINT)(edges_.size()) - 1);
    if (haveFace2) {
        addEdge(face2SplitEdge);  // add the splitting edge
        modifiedEdges.push_back((MLINT)(edges_.size()) - 1);
    }


    /********************************************************************************************
     * Update MeshAssociativity
     * We've changed the mesh, we need to update the MeshAssociativity to match
     ********************************************************************************************/

    // Find the parent edge in the face-edge list (MeshSheet) of the MeshModel
    MeshEdge *assocSheetEdge =
        meshModel->findFaceEdgeByInds(
            parentInds[0], parentInds[1]);

    // Find the parent edge in the edge list (MeshString) of the MeshModel
    MeshEdge *assocStringEdge =
        meshModel->findLowestTopoEdgeByInds(
            parentInds[0], parentInds[1]);

    if (assocStringEdge && assocSheetEdge != assocStringEdge) {
        // Parent edge belongs to a MeshString
        // add child edges to the MeshModel
        // use parent edge MeshString associativity
        std::string name;
        name = assocStringEdge->getName();
        name += ".1";
        meshModel->addEdge(parentInds[0], newPtInd+1,
            assocStringEdge->getID(),
            assocStringEdge->getAref(),
            assocStringEdge->getGref(),
            name,
            NULL, NULL, false);
        name = assocStringEdge->getName();
        name += ".2";
        meshModel->addEdge(parentInds[1], newPtInd + 1,
            assocStringEdge->getID(),
            assocStringEdge->getAref(),
            assocStringEdge->getGref(),
            name,
            NULL, NULL, false);

        // Repeat the edge add in any MeshString containing the parent edge
        std::vector<MeshString *> meshStrings;
        meshModel->getMeshStrings(meshStrings);
        std::vector<MeshString *>::iterator stringIter;

        for (stringIter = meshStrings.begin(); stringIter != meshStrings.end(); ++stringIter) {
            MeshString *meshString = *stringIter;
            MeshEdge *assocStringEdge =
                meshString->findEdgeByInds(
                    parentInds[0],
                    parentInds[1]);

            if (assocStringEdge) {
                // Parent edge belongs to this MeshString
                // add child edges to the MeshString
                // use parent edge MeshString associativity
                std::string name;
                name = assocStringEdge->getName();
                name += ".1";
                meshString->addEdge(parentInds[0], newPtInd + 1,
                    assocStringEdge->getID(),
                    assocStringEdge->getAref(),
                    assocStringEdge->getGref(),
                    name,
                    NULL, NULL, false);
                name = assocStringEdge->getName();
                name += ".2";
                meshString->addEdge(parentInds[1], newPtInd + 1,
                    assocStringEdge->getID(),
                    assocStringEdge->getAref(),
                    assocStringEdge->getGref(),
                    name,
                    NULL, NULL, false);
            }
        }

    }
    if (assocSheetEdge) {
        // Parent edge belongs to a MeshSheet
        // add child face-edges to the MeshModel 
        // use parent edge MeshSheet associativity
        meshModel->addFaceEdge(parentInds[0], newPtInd + 1,
            assocSheetEdge->getID(),
            assocSheetEdge->getAref(),
            assocSheetEdge->getGref(),
            NULL, NULL);
        meshModel->addFaceEdge(parentInds[1], newPtInd + 1,
            assocSheetEdge->getID(),
            assocSheetEdge->getAref(),
            assocSheetEdge->getGref(),
            NULL, NULL);

        // Repeat the face-edge add in every MeshSheet containing the parent edge
        std::vector<MeshSheet *> meshSheets;
        meshModel->getMeshSheets(meshSheets);
        std::vector<MeshSheet *>::iterator iter;

        for (iter = meshSheets.begin(); iter != meshSheets.end(); ++iter) {
            MeshSheet *meshSheet = *iter;
            MeshEdge *assocSheetEdge =
                meshSheet->findFaceEdgeByInds(
                    parentInds[0],
                    parentInds[1]);

            if (assocSheetEdge) {
                // Parent edge belongs to this MeshSheet
                // add child face-edges to the MeshSheet 
                // use parent edge MeshSheet associativity
                meshSheet->addFaceEdge(parentInds[0], newPtInd + 1,
                    assocSheetEdge->getID(),
                    assocSheetEdge->getAref(),
                    assocSheetEdge->getGref(),
                    NULL, NULL);
                meshSheet->addFaceEdge(parentInds[1], newPtInd + 1,
                    assocSheetEdge->getID(),
                    assocSheetEdge->getAref(),
                    assocSheetEdge->getGref(),
                    NULL, NULL);
            }
        }


        MeshFace *assocFace1 = meshModel->findFaceByInds(
            parentInds[0], parentInds[1],
            parentInds[2]);
        if (assocFace1) {
            // Parent face 1 belongs to a MeshSheet
            // add face-splitting edge to the MeshModel 
            // use parent face MeshSheet associativity
            meshModel->addFaceEdge(
                face1SplitEdge.nodes_[0] + 1,
                face1SplitEdge.nodes_[1] + 1,
                assocFace1->getID(),
                assocFace1->getAref(),
                assocFace1->getGref(),
                NULL, NULL);

            // add child faces
            std::string name;
            name = assocFace1->getName();
            name += ".1";
            meshModel->addFace(
                parentInds[0],
                newPtInd + 1,
                parentInds[2],
                assocFace1->getID(),
                assocFace1->getAref(),
                assocFace1->getGref(),
                name,
                NULL, NULL, NULL, false);

            name = assocFace1->getName();
            name += ".2";
            meshModel->addFace(
                newPtInd + 1,
                parentInds[1],
                parentInds[2],
                assocFace1->getID(),
                assocFace1->getAref(),
                assocFace1->getGref(),
                name,
                NULL, NULL, NULL, false);

            // Repeat the face-splitting edge add in every MeshSheet containing the parent face
            for (iter = meshSheets.begin(); iter != meshSheets.end(); ++iter) {
                MeshSheet *meshSheet = *iter;

                MeshFace *assocSheetFace = meshSheet->findFaceByInds(
                    parentInds[0], parentInds[1],
                    parentInds[2]);

                if (assocSheetFace) {
                    // Parent face 1 belongs to this MeshSheet
                    // add face-splitting edge to the MeshSheet 
                    // use parent face MeshSheet associativity
                    meshSheet->addFaceEdge(
                        face1SplitEdge.nodes_[0] + 1,
                        face1SplitEdge.nodes_[1] + 1,
                        assocSheetFace->getID(),
                        assocSheetFace->getAref(),
                        assocSheetFace->getGref(),
                        NULL, NULL);

                    // add child faces in this MeshSheet
                    std::string name;
                    name = assocFace1->getName();
                    name += ".1";
                    meshSheet->addFace(
                        parentInds[0],
                        newPtInd + 1,
                        parentInds[2],
                        assocSheetFace->getID(),
                        assocSheetFace->getAref(),
                        assocSheetFace->getGref(),
                        name,
                        NULL, NULL, NULL, false);

                    name = assocFace1->getName();
                    name += ".2";
                    meshSheet->addFace(
                        newPtInd + 1,
                        parentInds[1],
                        parentInds[2],
                        assocSheetFace->getID(),
                        assocSheetFace->getAref(),
                        assocSheetFace->getGref(),
                        name,
                        NULL, NULL, NULL, false);
                }
            }
        }

        if (haveFace2) {
            MeshFace *assocFace2 = meshModel->findFaceByInds(
                parentInds[0], parentInds[1],
                parentInds[3]);
            if (assocFace2) {
                // Parent face 2 belongs to a MeshSheet
                // add face-splitting edge to the MeshModel 
                // use parent face MeshSheet associativity
                meshModel->addFaceEdge(
                    face2SplitEdge.nodes_[0] + 1,
                    face2SplitEdge.nodes_[1] + 1,
                    assocFace2->getID(),
                    assocFace2->getAref(),
                    assocFace2->getGref(),
                    NULL, NULL);

                // add child faces
                std::string name;
                name = assocFace2->getName();
                name += ".1";
                meshModel->addFace(
                    newPtInd + 1,
                    parentInds[0],
                    parentInds[3],
                    assocFace2->getID(),
                    assocFace2->getAref(),
                    assocFace2->getGref(),
                    name,
                    NULL, NULL, NULL, false);

                name = assocFace2->getName();
                name += ".2";
                meshModel->addFace(
                    parentInds[1],
                    newPtInd + 1,
                    parentInds[3],
                    assocFace2->getID(),
                    assocFace2->getAref(),
                    assocFace2->getGref(),
                    name,
                    NULL, NULL, NULL, false);


                // Repeat the face-splitting edge add in every MeshSheet containing the parent face
                for (iter = meshSheets.begin(); iter != meshSheets.end(); ++iter) {
                    MeshSheet *meshSheet = *iter;

                    MeshFace *assocSheetFace = meshSheet->findFaceByInds(
                        parentInds[0], parentInds[1],
                        parentInds[3]);

                    if (assocSheetFace) {
                        // Parent face 2 belongs to this MeshSheet
                        // add face-splitting edge to the MeshSheet 
                        // use parent face MeshSheet associativity
                        meshSheet->addFaceEdge(
                            face2SplitEdge.nodes_[0] + 1,
                            face2SplitEdge.nodes_[1] + 1,
                            assocSheetFace->getID(),
                            assocSheetFace->getAref(),
                            assocSheetFace->getGref(),
                            NULL, NULL);

                        // add child faces in this MeshSheet
                        std::string name;
                        name = assocFace2->getName();
                        name += ".1";
                        meshSheet->addFace(
                            newPtInd + 1,
                            parentInds[0],
                            parentInds[3],
                            assocSheetFace->getID(),
                            assocSheetFace->getAref(),
                            assocSheetFace->getGref(),
                            name,
                            NULL, NULL, NULL, false);

                        name = assocFace2->getName();
                        name += ".2";
                        meshSheet->addFace(
                            parentInds[1],
                            newPtInd + 1,
                            parentInds[3],
                            assocSheetFace->getID(),
                            assocSheetFace->getAref(),
                            assocSheetFace->getGref(),
                            name,
                            NULL, NULL, NULL, false);
                    }
                }
            }
        }
    }

    // MeshModel - delete parent face-edge and edge associativity
    meshModel->deleteEdgeByInds(parentInds[0], parentInds[1]);

    // MeshModel - delete parent face associativity
    meshModel->deleteFaceByInds(parentInds[0], parentInds[1], parentInds[2]);
    meshModel->deleteFaceByInds(parentInds[0], parentInds[1], parentInds[3]);

    // Repeat the edge delete in any MeshString containing the parent edge
    std::vector<MeshString *> meshStringsX;
    meshModel->getMeshStrings(meshStringsX);
    std::vector<MeshString *>::iterator stringIter;
    for (stringIter = meshStringsX.begin(); stringIter != meshStringsX.end(); ++stringIter) {
        MeshString *meshString = *stringIter;
        // delete parent edge associativity 
        meshString->deleteEdgeByInds(parentInds[0], parentInds[1]);
    }

    // Repeat the edge and face delete in any MeshSheet containing the parent edge/face
    std::vector<MeshSheet *> meshSheets;
    meshModel->getMeshSheets(meshSheets);
    std::vector<MeshSheet *>::iterator iter;
    for (iter = meshSheets.begin(); iter != meshSheets.end(); ++iter) {
        MeshSheet *meshSheet = *iter;
        // delete parent face-edge and edge associativity 
        meshSheet->deleteFaceEdgeByInds(parentInds[0], parentInds[1]);

        // delete parent face associativity
        meshSheet->deleteFaceByInds(parentInds[0], parentInds[1], parentInds[2]);
        meshSheet->deleteFaceByInds(parentInds[0], parentInds[1], parentInds[3]);
    }


#if defined(CHECK_TOPOLOGY)
    ML_assert(NULL == findEdge(parentEdge.nodes_[0], parentEdge.nodes_[1], false));

    //checkEdges();
    //checkFaces();

    MLINT i;
    for (i = 0; i < 2; ++i) {
        ML_assert((MLINT)points_.size() > childEdge1.nodes_[i]);
        ML_assert((MLINT)points_.size() > childEdge2.nodes_[i]);
    }

    for (i = 0; i < 3; ++i) {
        ML_assert(faces_[childEdge1.faces_[0]].nodes_[i] < (MLINT)points_.size());
        ML_assert(faces_[childEdge2.faces_[0]].nodes_[i] < (MLINT)points_.size());
    }

    ML_assert(faces_[childEdge1.faces_[0]].nodes_[0] == childEdge1.nodes_[0]);
    ML_assert(faces_[childEdge1.faces_[0]].nodes_[1] == childEdge1.nodes_[1]);
    if (parentEdge.faces_[1] >= 0) {
        for (i = 0; i < 3; ++i) {
            ML_assert(faces_[childEdge1.faces_[1]].nodes_[i] < (MLINT)points_.size());
            ML_assert(faces_[childEdge2.faces_[1]].nodes_[i] < (MLINT)points_.size());
        }
    }

    ML_assert(faces_[childEdge2.faces_[0]].nodes_[0] == childEdge2.nodes_[0]);
    ML_assert(faces_[childEdge2.faces_[0]].nodes_[1] == childEdge2.nodes_[1]);

    if (parentEdge.faces_[1] >= 0) {
        ML_assert(faces_[childEdge2.faces_[1]].nodes_[0] == childEdge2.nodes_[1]);
        ML_assert(faces_[childEdge2.faces_[1]].nodes_[1] == childEdge2.nodes_[0]);
    }
#endif
    return status;
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
