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
 * Mesh String Class
 ***************************************************************************/
MLUINT MeshString::nameCounter_ = 0;

const std::string &
MeshString::getBaseName() const
{
    static const std::string base("ml_string-");
    return base;
};

MLUINT &
MeshString::getNameCounter() {
    return nameCounter_;
};

MeshString::MeshString() :
    MeshTopo() 
{
    edgeCounter_ = 0;
};

MeshString::MeshString(
    MLINT mid,
    MLINT aref,
    MLINT gref,
    const std::string &name) :
    MeshTopo(mid, aref, gref, name)
{
    // name arg is allowed to be empty, setName ensures a unique name
    setName(name);
    edgeCounter_ = 0;
};
MeshString::MeshString(
    const std::string &ref,
    MLINT mid,
    MLINT aref,
    MLINT gref,
    const std::string &name) :
    MeshTopo(ref, mid, aref, gref, name)
{
    // name arg is allowed to be empty, setName ensures a unique name
    setName(name);
    edgeCounter_ = 0;
};

MeshString::~MeshString()
{
    std::map<pwiFnvHash::FNVHash, MeshPoint*>::iterator piter;
    for (piter = pointMap_.begin(); piter != pointMap_.end(); ++piter) {
        delete piter->second;
    }

    std::map<pwiFnvHash::FNVHash, MeshEdge*>::iterator eiter;
    for (eiter = edgeMap_.begin(); eiter != edgeMap_.end(); ++eiter) {
        delete eiter->second;
    }
}

// Add an edge to the associativity data
bool
MeshString::addEdge(MLINT i1, MLINT i2,
    MLINT mid,
    MLINT aref,
    MLINT gref,
    const std::string &name,
    ParamVertex *pv1, ParamVertex *pv2, bool mapID)
{
    if (NULL != findEdgeByInds(i1, i2)) {
        // edge already exists
        return true;
    }
    MeshEdge *edge = new MeshEdge(i1, i2, mid, aref,
        gref, name, pv1, pv2);
    MeshTopo *existing = getMeshEdgeByName(edge->name_);
    if (existing) {
        delete edge;
        return false;
    }
    edgeMap_[edge->getHash()] = edge; // not the owner
    meshEdgeNameMap_[edge->getName()] = edge;  // owner

    if (mapID) {
        meshEdgeIDToNameMap_[mid] = edge->getName();
    }

    /// Assign incremented edgeCounter as edge order
    edge->setOrderCounter(edgeCounter_++);

    return true;
}
bool
MeshString::addEdge(
    const std::string &ref,
    MLINT mid,
    MLINT aref,
    MLINT gref,
    const std::string &name,
    ParamVertex *pv1, ParamVertex *pv2, bool mapID)
{
    MeshEdge *edge = new MeshEdge(ref, mid, aref,
        gref, name, pv1, pv2);
    MeshTopo *existing = getMeshEdgeByName(edge->name_);
    if (existing) {
        delete edge;
        return false;
    }
    meshEdgeNameMap_[edge->getName()] = edge;  // owner

    if (!edge->getRef().empty()) {
        meshEdgeRefToNameMap_[edge->getRef()] = edge->getName();
    }
    if (mapID) {
        meshEdgeIDToNameMap_[mid] = edge->getName();
    }

    /// Assign incremented edgeCounter as edge order
    edge->setOrderCounter(edgeCounter_++);

    return true;
}


// Find an edge in the associativity data
MeshEdge *
MeshString::findEdgeByInds(MLINT i1, MLINT i2) const
{
    pwiFnvHash::FNVHash hash = MeshEdge::computeHash(i1, i2);
    std::map<pwiFnvHash::FNVHash, MeshEdge*>::const_iterator iter;

    // Look in MeshEdge list first
    iter = edgeMap_.find(hash);
    if (iter != edgeMap_.end()) {
        return iter->second;
    }

    // unfound
    return NULL;
}


// Delete an edge from the associativity data
void
MeshString::deleteEdgeByInds(MLINT i1, MLINT i2)
{
    pwiFnvHash::FNVHash hash = MeshEdge::computeHash(i1, i2);
    std::map<pwiFnvHash::FNVHash, MeshEdge*>::iterator iter;
    iter = edgeMap_.find(hash);
    if (iter == edgeMap_.end()) {
        return;
    }
    MeshEdge *edge = iter->second;
    edgeMap_.erase(iter);

    MeshTopoRefToNameMap::iterator refIter = meshEdgeRefToNameMap_.find(edge->getRef());
    if (refIter != meshEdgeRefToNameMap_.end()) {
        meshEdgeRefToNameMap_.erase(refIter);
    }

    MeshTopoIDToNameMap::iterator idIter = meshEdgeIDToNameMap_.find(edge->getID());
    if (idIter != meshEdgeIDToNameMap_.end()) {
        meshEdgeIDToNameMap_.erase(idIter);
    }

    // nameMap is the owner
    MeshEdgeNameMap::iterator nameIter = meshEdgeNameMap_.find(edge->getName());
    if (nameIter != meshEdgeNameMap_.end()) {
        delete nameIter->second;
        meshEdgeNameMap_.erase(nameIter);
    }
}


MeshEdge *
MeshString::getMeshEdgeByName(const std::string &name) const
{
    MeshEdgeNameMap::const_iterator iter = meshEdgeNameMap_.find(name);
    if (iter == meshEdgeNameMap_.end()) {
        return NULL;
    }
    return iter->second;
}

MeshEdge *
MeshString::getMeshEdgeByRef(const std::string &ref) const
{
    MeshTopoRefToNameMap::const_iterator iter = meshEdgeRefToNameMap_.find(ref);
    if (iter == meshEdgeRefToNameMap_.end()) {
        return NULL;
    }
    return getMeshEdgeByName(iter->second);
}

MLINT
MeshString::getNumEdges() const
{
    return (MLINT)meshEdgeNameMap_.size();
}

void
MeshString::getMeshEdges(std::vector<const MeshEdge *> &edges) const
{
    edges.clear();;
    edges.resize(meshEdgeNameMap_.size());
    MeshEdgeNameMap::const_iterator iter;
    size_t i;
    for (i = 0, iter = meshEdgeNameMap_.begin(); iter != meshEdgeNameMap_.end(); ++iter, ++i) {
        edges[i] = iter->second;
    }

    // sort by creation order in the MeshString
    std::sort(edges.begin(), edges.end(), MeshTopo::OrderCompare);

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
