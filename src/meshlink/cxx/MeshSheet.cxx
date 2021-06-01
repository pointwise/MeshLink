/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

#include "MeshAssociativity.h"
#include "MeshTopo.h"

#include <iostream>
#include <sstream>


/****************************************************************************
 * Mesh Sheet Class
 ***************************************************************************/
MLUINT MeshSheet::nameCounter_ = 0;

const std::string &
MeshSheet::getBaseName() const
{
    static const std::string base("ml_sheet-");
    return base;
};

MLUINT &
MeshSheet::getNameCounter() {
    return nameCounter_;
};

MeshSheet::MeshSheet() :
    MeshTopo() 
{
    faceEdgeCounter_ = 0;
    faceCounter_ = 0;
};

MeshSheet::MeshSheet(
    MLINT mid,
    MLINT aref,
    MLINT gref,
    const std::string &name) :
    MeshTopo(mid, aref, gref, name)
{
    // name arg is allowed to be empty, setName ensures a unique name
    setName(name);
    faceEdgeCounter_ = 0;
    faceCounter_ = 0;
};

MeshSheet::MeshSheet(
    const std::string &ref,
    MLINT mid,
    MLINT aref,
    MLINT gref,
    const std::string &name) :
    MeshTopo(ref, mid, aref, gref, name)
{
    // name arg is allowed to be empty, setName ensures a unique name
    setName(name);
    faceEdgeCounter_ = 0;
    faceCounter_ = 0;
};

MeshSheet::~MeshSheet()
{
    std::map<pwiFnvHash::FNVHash, MeshPoint*>::iterator piter;
    for (piter = pointMap_.begin(); piter != pointMap_.end(); ++piter) {
        delete piter->second;
    }

    std::map<pwiFnvHash::FNVHash, MeshEdge*>::iterator eiter;
    for (eiter = faceEdgeMap_.begin(); eiter != faceEdgeMap_.end(); ++eiter) {
        delete eiter->second;
    }
    std::map<pwiFnvHash::FNVHash, MeshFace*>::iterator fiter;
    for (fiter = faceMap_.begin(); fiter != faceMap_.end(); ++fiter) {
        delete fiter->second;
    }
}


// Find a face-edge in the associativity data
MeshEdge *
MeshSheet::findFaceEdgeByInds(MLINT i1, MLINT i2) const
{
    pwiFnvHash::FNVHash hash = MeshEdge::computeHash(i1, i2);
    std::map<pwiFnvHash::FNVHash, MeshEdge*>::const_iterator iter;

    // Look in MeshFaceEdge list
    iter = faceEdgeMap_.find(hash);
    if (iter != faceEdgeMap_.end()) {
        return iter->second;
    }
    return NULL;
}


// Delete a face-edge from the associativity data
void
MeshSheet::deleteFaceEdgeByInds(MLINT i1, MLINT i2)
{
    pwiFnvHash::FNVHash hash = MeshEdge::computeHash(i1, i2);
    std::map<pwiFnvHash::FNVHash, MeshEdge*>::iterator iter;
    MeshEdge *edge = NULL;
    // Look in MeshFaceEdge list
    iter = faceEdgeMap_.find(hash);
    if (iter == faceEdgeMap_.end()) {
        return;
    }
    // found FaceEdge
    edge = iter->second;
    faceEdgeMap_.erase(iter);
    delete edge;
}


// Add a face edge to the associativity data
void
MeshSheet::addFaceEdge(MLINT i1, MLINT i2, 
    MLINT mid,
    MLINT aref,
    MLINT gref,
    ParamVertex *pv1, ParamVertex *pv2)
{
    if (NULL != findFaceEdgeByInds(i1, i2)) {
        // face-edge already exists
        return;
    }

    // face edges don't have names
	std::string name;
    MeshEdge *edge = new MeshEdge(i1, i2, mid, aref,
        gref, name, pv1, pv2);
    faceEdgeMap_[edge->getHash()] = edge;

    /// Assign incremented faceEdgeCounter as faceEdge order
    edge->setOrderCounter(faceEdgeCounter_++);
}



// Add a face to the associativity data
bool
MeshSheet::addFace(MLINT i1, MLINT i2, MLINT i3, 
    MLINT mid,
    MLINT aref,
    MLINT gref,
    const std::string &name,
    ParamVertex *pv1, ParamVertex *pv2, ParamVertex *pv3, bool mapID)
{
    if (NULL != findFaceByInds(i1, i2, i3)) {
        // face already exists
        return true;
    }
    MeshFace *face = new MeshFace(i1, i2, i3, mid, aref,
        gref, name, pv1, pv2, pv3);
    MeshTopo *existing = getMeshFaceByName(face->name_);
    if (existing) {
        delete face;
        return false;
    }

    meshFaceNameMap_[face->getName()] = face;  // owner
    faceMap_[face->getHash()] = face;
    if (mapID) {
        meshFaceIDToNameMap_[mid] = face->getName();
    }

    /// Assign incremented faceCounter as face order
    face->setOrderCounter(faceCounter_++);

    return true;
}
bool
MeshSheet::addFace(
    const std::string &ref,
    MLINT mid,
    MLINT aref,
    MLINT gref,
    const std::string &name,
    ParamVertex *pv1, ParamVertex *pv2, ParamVertex *pv3, bool mapID)
{
    MeshFace *face = new MeshFace(ref, mid, aref,
        gref, name, pv1, pv2, pv3);
    MeshTopo *existing = getMeshFaceByName(face->name_);
    if (existing) {
        delete face;
        return false;
    }

    meshFaceNameMap_[face->getName()] = face;  // owner

    if (!face->getRef().empty()) {
        meshFaceRefToNameMap_[face->getRef()] = face->getName();
    }

    if (mapID) {
        meshFaceIDToNameMap_[mid] = face->getName();
    }

    /// Assign incremented faceCounter as face order
    face->setOrderCounter(faceCounter_++);

    return true;
}


bool
MeshSheet::addFace(MLINT i1, MLINT i2, MLINT i3, MLINT i4,
    MLINT mid,
    MLINT aref,
    MLINT gref,
    const std::string &name,
    ParamVertex *pv1, ParamVertex *pv2, ParamVertex *pv3, ParamVertex *pv4,
    bool mapID)
{
    if (NULL != findFaceByInds(i1, i2, i3, i4)) {
        // face already exists
        return true;
    }
    MeshFace *face = new MeshFace(i1, i2, i3, i4, mid, aref,
        gref, name, pv1, pv2, pv3, pv4);
    MeshTopo *existing = getMeshFaceByName(face->name_);
    if (existing) {
        delete face;
        return false;
    }

    meshFaceNameMap_[face->getName()] = face;  // owner
    faceMap_[face->getHash()] = face;
    if (mapID) {
        meshFaceIDToNameMap_[mid] = face->getName();
    }

    /// Assign incremented faceCounter as face order
    face->setOrderCounter(faceCounter_++);

    return true;
}
bool
MeshSheet::addFace(
    const std::string &ref,
    MLINT mid,
    MLINT aref,
    MLINT gref,
    const std::string &name,
    ParamVertex *pv1, ParamVertex *pv2, ParamVertex *pv3, ParamVertex *pv4,
    bool mapID)
{
    MeshFace *face = new MeshFace(ref, mid, aref,
        gref, name, pv1, pv2, pv3, pv4);
    MeshTopo *existing = getMeshFaceByName(face->name_);
    if (existing) {
        delete face;
        return false;
    }

    meshFaceNameMap_[face->getName()] = face;  // owner

    if (!face->getRef().empty()) {
        meshFaceRefToNameMap_[face->getRef()] = face->getName();
    }

    if (mapID) {
        meshFaceIDToNameMap_[mid] = face->getName();
    }

    /// Assign incremented faceCounter as face order
    face->setOrderCounter(faceCounter_++);

    return true;
}


// Find a face in the associativity data
MeshFace *
MeshSheet::findFaceByInds(MLINT i1, MLINT i2, MLINT i3, MLINT i4) const
{
    pwiFnvHash::FNVHash hash = MeshFace::computeHash(i1, i2, i3, i4);
    std::map<pwiFnvHash::FNVHash, MeshFace*>::const_iterator iter;
    iter = faceMap_.find(hash);
    if (iter != faceMap_.end()) {
        return iter->second;
    }
    // unfound
    return NULL;
}


// Delete a face from the associativity data
void
MeshSheet::deleteFaceByInds(MLINT i1, MLINT i2, MLINT i3, MLINT i4)
{
    pwiFnvHash::FNVHash hash = MeshFace::computeHash(i1, i2, i3, i4);
    std::map<pwiFnvHash::FNVHash, MeshFace*>::iterator iter;
    iter = faceMap_.find(hash);
    if (iter == faceMap_.end()) return;
    MeshFace *face = iter->second;
    faceMap_.erase(iter);

    MeshTopoRefToNameMap::iterator refIter = meshFaceRefToNameMap_.find(face->getRef());
    if (refIter != meshFaceRefToNameMap_.end()) {
        meshFaceRefToNameMap_.erase(refIter);
    }

    MeshTopoIDToNameMap::iterator idIter = meshFaceIDToNameMap_.find(face->getID());
    if (idIter != meshFaceIDToNameMap_.end()) {
        meshFaceIDToNameMap_.erase(idIter);
    }

    // nameMap is the owner
    MeshFaceNameMap::iterator nameIter = meshFaceNameMap_.find(face->getName());
    if (nameIter != meshFaceNameMap_.end()) {
        delete nameIter->second;
        meshFaceNameMap_.erase(nameIter);
    }
}

MeshFace * 
MeshSheet::getMeshFaceByName(const std::string &name) const
{
    MeshFaceNameMap::const_iterator iter = meshFaceNameMap_.find(name);
    if (iter == meshFaceNameMap_.end()) {
        return NULL;
    }
    return iter->second;
}

MeshFace * 
MeshSheet::getMeshFaceByRef(const std::string &ref) const
{
    MeshTopoRefToNameMap::const_iterator iter = meshFaceRefToNameMap_.find(ref);
    if (iter == meshFaceRefToNameMap_.end()) {
        return NULL;
    }
    return getMeshFaceByName(iter->second);
}


MLINT 
MeshSheet::getNumFaceEdges() const
{
    return (MLINT)faceEdgeMap_.size();
}

MLINT 
MeshSheet::getNumFaces() const
{
    return (MLINT)meshFaceNameMap_.size();
}

void 
MeshSheet::getFaceEdges(std::vector<const MeshEdge *> &edges) const
{
    edges.clear();
    edges.resize(faceEdgeMap_.size());
    std::map<pwiFnvHash::FNVHash, MeshEdge*>::const_iterator iter;
    size_t i;
    for (i = 0, iter = faceEdgeMap_.begin(); iter != faceEdgeMap_.end(); ++iter, ++i) {
        edges[i] = iter->second;
    }

    // sort by creation order in the MeshSheet
    std::sort(edges.begin(), edges.end(), MeshTopo::OrderCompare);

    return;
}


void 
MeshSheet::getMeshFaces(std::vector<const MeshFace *> &faces) const
{
    faces.clear();
    faces.resize(meshFaceNameMap_.size());
    MeshFaceNameMap::const_iterator iter;
    size_t i;
    for (i = 0, iter = meshFaceNameMap_.begin(); iter != meshFaceNameMap_.end(); ++iter, ++i) {
        faces[i] = iter->second;
    }

    // sort by creation order in the MeshSheet
    std::sort(faces.begin(), faces.end(), MeshTopo::OrderCompare);

    return;
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
